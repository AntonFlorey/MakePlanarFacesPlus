#include "module.hh"
#include "out.hh"

#include <iostream>
#include <format>
#include <unordered_map>

#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>

#include <TinyAD/ScalarFunction.hh>
#include <TinyAD/Utils/NewtonDirection.hh>
#include <TinyAD/Utils/NewtonDecrement.hh>
#include <TinyAD/Utils/LineSearch.hh>
#include <TinyAD/Utils/Out.hh>

#include <Eigen/IterativeLinearSolvers>

namespace py = pybind11;

std::vector<Vec3d> make_planar_faces(const std::vector<Vec3d>& vertices, const std::vector<std::vector<int>>& faces, const std::vector<int>& fixed_vertices, double surface_area, const MakePlanarSettings& settings)
{
    // filter faces
    std::vector<std::vector<int>> filtered_faces;
    int skipped_large_faces = 0;
    for (std::vector<int> face : faces)
    {
        int verts = face.size();
        MP_ASSERT_GEQ(verts, 3);
        if (verts == 3) continue;
        if (verts > 4) {
            skipped_large_faces++;
            continue;
        }
        filtered_faces.push_back(face);
    }
    MP_INFO(std::format("Skipped {} faces that have more than 4 vertices.", skipped_large_faces));

	// Put faces into buckets
	std::unordered_map<int, std::vector<int>> faces_by_verts;
	for (std::vector<int> face : filtered_faces)
	{
		int verts = face.size();
        if (faces_by_verts.find(verts) == faces_by_verts.end())
        {
            faces_by_verts[verts] = face;
        }
        else
        {
            faces_by_verts[verts].insert(faces_by_verts[verts].end(), face.begin(), face.end());
        }
	}
	
	// Set up a TinyAD function
	int n = vertices.size();
	auto func = TinyAD::scalar_function<3>(TinyAD::range(n));

	// Compute base reduction matrix for fixed vertices
    // Matrix C maps from m-dim to n-dim space.
    // It is the identity map for all unconstrained vertices.
    SparseMatrix C;
    if (fixed_vertices.size() != 0)
    {
        const int m = 3 * (n - fixed_vertices.size());
        std::vector<bool> vertex_is_fixed_array = std::vector<bool>(n, false);
        for (int vertex_id : fixed_vertices)
        {
            vertex_is_fixed_array[vertex_id] = true;
        }

        std::vector<Triplet> C_triplets;
        C_triplets.reserve(m);
        int C_cols = 0;
        for (int vertex_id = 0; vertex_id < n; vertex_id++)
        {
            if (!vertex_is_fixed_array[vertex_id])
            {
                C_triplets.emplace_back(3 * vertex_id + 0, C_cols++, 1.0);
                C_triplets.emplace_back(3 * vertex_id + 1, C_cols++, 1.0);
                C_triplets.emplace_back(3 * vertex_id + 2, C_cols++, 1.0);
            }
        }
        MP_ASSERT_EQ(C_cols, m);

        C = SparseMatrix(3 * n, m);
        C.setFromTriplets(C_triplets.cbegin(), C_triplets.cend());
        MP_ASSERT_EQ(C.rows(), 3 * n);
        MP_ASSERT_EQ(C.cols(), m);
    }
    else
    {
        C = TinyAD::identity<double>(3 * n);
    }
    
    // Add closeness term
    double closeness_weight = settings.closeness_weight;
    func.add_elements<1>(TinyAD::range(n), [&](auto& element)->TINYAD_SCALAR_TYPE(element)
    {
        // Evaluate element using either double or TinyAD::Double
        using T = TINYAD_SCALAR_TYPE(element);

        // Get the vertex position
        Eigen::Vector3<T> current_v_pos = element.variables(element.handle);
        Eigen::Vector3d orig_v_pos = vertices[element.handle];

        // Compute squared distance
        return closeness_weight * (current_v_pos - orig_v_pos).squaredNorm() / (n * surface_area);
    });

    // Add planarity term
    int n_faces = filtered_faces.size();
    func.add_elements<4>(TinyAD::range(n_faces), [&](auto& element)->TINYAD_SCALAR_TYPE(element)
    {
        // Evaluate element using either double or TinyAD::Double
        using T = TINYAD_SCALAR_TYPE(element);

        std::vector<int> current_face = filtered_faces[element.handle];
        MP_ASSERT_EQ(current_face.size(), 4);
        
        std::vector<Eigen::Vector3<T>> vertex_coords = {
            element.variables(current_face[0]),
            element.variables(current_face[1]),
            element.variables(current_face[2]),
            element.variables(current_face[3])
        };
        std::vector<Eigen::Vector3<T>> normalized_edges =
        {
            (vertex_coords[1] - vertex_coords[0]).normalized(),
            (vertex_coords[2] - vertex_coords[1]).normalized(),
            (vertex_coords[3] - vertex_coords[2]).normalized(),
            (vertex_coords[0] - vertex_coords[3]).normalized()
        };
        T summed_determinants = 0.0;
        for (size_t i = 0; i < 4; i++)
        {
            summed_determinants += sqr(TinyAD::col_mat(normalized_edges[i], normalized_edges[(i + 1) % 4], normalized_edges[(i + 2) % 4]).determinant());
        }
        
        return summed_determinants / (double)n_faces;
    });

    // Init variables
    Eigen::VectorXd x = func.x_from_data([&](long vertex_index)
    {
        return vertices[vertex_index];
    });

    // Optimize
    TinyAD::LinearSolver solver;
    for (int iter = 0; iter < settings.max_iterations; iter++)
    {
        // eval function value, gradient and hessian
        auto [f, g, H_proj] = func.eval_with_hessian_proj(x, settings.projection_eps);
        
        // compute newton step direction
        Eigen::VectorXd d = TinyAD::newton_direction_reduced_basis(g, H_proj, C, solver, settings.w_identity);
        double newton_decrement = TinyAD::newton_decrement<double>(d, g);
        if (settings.verbose) TINYAD_INFO("Energy | Newton decrement in iteration " << iter << ": " << f << " | " << newton_decrement);
        if (newton_decrement < settings.convergence_eps)
            break;

        // line search for new x
        x = TinyAD::line_search(x, d, f, g, func);

        // update closeness weight
        closeness_weight *= settings.closeness_weight_decay;
    }

    // Extract solution
    std::vector<Vec3d> optimized_vertex_positions = std::vector<Vec3d>(n, Vec3d::Zero());
    func.x_to_data(x, [&](int v_id, const Eigen::Vector3d& _p) {
        optimized_vertex_positions[v_id] = _p;
    });

    return optimized_vertex_positions; // return empty vector
}

void say_hello()
{
	std::cout << "Hello World" << std::endl;
}

PYBIND11_MODULE(testmodule, m)
{
    m.doc() = "This is a module created with pybind11";
    m.def("say_hello", &say_hello, "A function that prints Hello World");
    py::class_<MakePlanarSettings>(m, "MakePlanarSettings")
        .def(py::init())
        .def_readwrite("max_iterations", &MakePlanarSettings::max_iterations)
        .def_readwrite("closeness_weight", &MakePlanarSettings::closeness_weight)
        .def_readwrite("closeness_weight_decay", &MakePlanarSettings::closeness_weight_decay)
        .def_readwrite("verbose", &MakePlanarSettings::verbose)
        .def_readwrite("projection_eps", &MakePlanarSettings::projection_eps)
        .def_readwrite("w_identity", &MakePlanarSettings::w_identity)
        .def_readwrite("convergence_eps", &MakePlanarSettings::convergence_eps);
    m.def("make_planar_faces", &make_planar_faces, "Continuous optimization that makes quad faces planar with minimal geometric loss.");

}
