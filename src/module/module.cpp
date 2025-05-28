#include "module.hh"

#include <iostream>
#include <math.h>
#include <format>
#include <array>
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


namespace MakePlanarFacesPlus
{

    double compute_bounding_box_diameter(const std::vector<Vec3d>& vertices)
    {
        if (vertices.size() == 0)
        {
            TINYAD_WARNING("Tried to compute the bounding box of an empty mesh!");
            return theodorus_constant;
        }
        Vec3d min_corner = vertices[0];
        Vec3d max_corner = vertices[1];
        for (size_t i = 1; i < vertices.size(); i++)
        {
            min_corner = min_corner.cwiseMin(vertices[i]);
            max_corner = max_corner.cwiseMax(vertices[i]);
        }
        return (max_corner - min_corner).norm();
    }

    std::vector<Vec3d> make_planar_faces(const std::vector<Vec3d>& vertices, const std::vector<std::vector<int>>& faces, const std::vector<int>& fixed_vertices, const MakePlanarSettings& settings)
    {
        // Compute a normalization factor to make the optimization invariant under mesh dimensions
        double bb_diameter = compute_bounding_box_diameter(vertices);
        double normalization_factor = theodorus_constant / bb_diameter;
        // Go over all faces and store connected edge segments for planarity objective
        std::vector<std::array<int, 4>> planarity_objective_indices;
        for (std::vector<int> face : faces)
        {
            int n_verts = face.size();
            TINYAD_ASSERT_GEQ(n_verts, 3);
            if (n_verts == 3) continue;
            for (int v_id = 0; v_id < n_verts; v_id++)
            {
                planarity_objective_indices.push_back({ face[v_id], face[(v_id + 1) % n_verts], face[(v_id + 2) % n_verts] , face[(v_id + 3) % n_verts] });
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
            TINYAD_ASSERT_EQ(C_cols, m);

            C = SparseMatrix(3 * n, m);
            C.setFromTriplets(C_triplets.cbegin(), C_triplets.cend());
            TINYAD_ASSERT_EQ(C.rows(), 3 * n);
            TINYAD_ASSERT_EQ(C.cols(), m);
        }
        else
        {
            C = TinyAD::identity<double>(3 * n);
        }

        // Add closeness term
        double closeness_weight = settings.initial_closeness_weight;
        func.add_elements<1>(TinyAD::range(n), [&](auto& element)->TINYAD_SCALAR_TYPE(element)
        {
            // Evaluate element using either double or TinyAD::Double
            using T = TINYAD_SCALAR_TYPE(element);

            // Get the vertex position
            Eigen::Vector3<T> current_v_pos = element.variables(element.handle);
            Eigen::Vector3d orig_v_pos = normalization_factor * vertices[element.handle];

            // Compute squared distance
            return closeness_weight * (current_v_pos - orig_v_pos).squaredNorm() / (double)n;
        });

        // Add planarity term
        int n_planarity_objectives = planarity_objective_indices.size();
        func.add_elements<4>(TinyAD::range(n_planarity_objectives), [&](auto& element)->TINYAD_SCALAR_TYPE(element)
        {
            // Evaluate element using either double or TinyAD::Double
            using T = TINYAD_SCALAR_TYPE(element);

            std::array<int, 4> current_vertex_indices = planarity_objective_indices[element.handle];

            std::vector<Eigen::Vector3<T>> vertex_coords = {
                element.variables(current_vertex_indices[0]),
                element.variables(current_vertex_indices[1]),
                element.variables(current_vertex_indices[2]),
                element.variables(current_vertex_indices[3])
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
                summed_determinants += 0.25 * sqr(TinyAD::col_mat(normalized_edges[i], normalized_edges[(i + 1) % 4], normalized_edges[(i + 2) % 4]).determinant());
            }

            return summed_determinants / (double)n_planarity_objectives;
        });

        // Init variables (scale the mesh to have a unit volume bounding box)
        Eigen::VectorXd x = func.x_from_data([&](long vertex_index)
        {
            return normalization_factor * vertices[vertex_index];
        });

        // Optimize
        TinyAD::LinearSolver solver;
        double decay = 1.0;
        if (settings.optimization_rounds > 1)
        {
            if (settings.initial_closeness_weight == 0)
            {
                decay = 0.0;
            }
            else if (settings.min_closeness_weight == 0)
            {
                // This rule is a bit random 
                decay = std::pow(std::min(0.1 * settings.initial_closeness_weight, 0.001) / settings.initial_closeness_weight, 1.0 / (settings.optimization_rounds - 1.0));
            }
            else
            {
                decay = std::pow(settings.min_closeness_weight / settings.initial_closeness_weight, 1.0 / (settings.optimization_rounds - 1.0));
            }
        }
        for (int opt_round = 0; opt_round < settings.optimization_rounds; opt_round++)
        {
            // update closeness weight
            if (settings.optimization_rounds == 1)
            {
                closeness_weight = settings.initial_closeness_weight;
            }
            else
            {
                // Interpolate between start and min closeness weight
                closeness_weight = settings.initial_closeness_weight * std::pow(decay, (double)opt_round);
                if (settings.min_closeness_weight == 0.0 && opt_round == settings.optimization_rounds - 1) closeness_weight = 0.0;
            }
            for (int iter = 0; iter < settings.max_iterations; iter++)
            {
                // eval function value, gradient and hessian
                auto [f, g, H_proj] = func.eval_with_hessian_proj(x, settings.projection_eps);

                // compute newton step direction
                Eigen::VectorXd d = TinyAD::newton_direction_reduced_basis(g, H_proj, C, solver, settings.w_identity);

                double newton_decrement = TinyAD::newton_decrement<double>(d, g);
                if (newton_decrement < settings.convergence_eps)
                {
                    if (settings.verbose) TINYAD_INFO("Newton decrement below convergence eps. Stopping early.");
                    break;
                }

                // line search for new x
                Eigen::VectorXd x_old = x;
                x = TinyAD::line_search(x, d, f, g, func);
                
                // Early stopping 
                if (x == x_old)
                {
                    if (settings.verbose) TINYAD_INFO("Line search couldn't find improvement. Stopping early.");
                    break;
                }
                double new_f = func.eval(x);
                if (std::abs(f - new_f) < settings.convergence_eps)
                    if (settings.verbose) TINYAD_INFO("Function improvement below convergence eps. Stopping early.");
                    break;
            }
            double curr_f = func.eval(x);
            if (settings.verbose) TINYAD_INFO("Energy | Closeness Weight after round " << opt_round << ": " << curr_f << " | " << closeness_weight);
        }
        if (settings.verbose) TINYAD_INFO("Final energy: " << func.eval(x));

        // Extract solution (scale back to original dimensions)
        std::vector<Vec3d> optimized_vertex_positions = std::vector<Vec3d>(n, Vec3d::Zero());
        func.x_to_data(x, [&](int v_id, const Eigen::Vector3d& _p) {
            optimized_vertex_positions[v_id] = _p / normalization_factor;
        });

        return optimized_vertex_positions; // return empty vector
    }

    void say_hello()
    {
        std::cout << "Hello World" << std::endl;
    }

}

PYBIND11_MODULE(mpfpmodule, m)
{
    m.doc() = "This is a module created with pybind11";
    m.def("say_hello", &MakePlanarFacesPlus::say_hello, "A function that prints Hello World");
    py::class_<MakePlanarFacesPlus::MakePlanarSettings>(m, "MakePlanarSettings")
        .def(py::init())
        .def_readwrite("optimization_rounds", &MakePlanarFacesPlus::MakePlanarSettings::optimization_rounds)
        .def_readwrite("max_iterations", &MakePlanarFacesPlus::MakePlanarSettings::max_iterations)
        .def_readwrite("closeness_weight", &MakePlanarFacesPlus::MakePlanarSettings::initial_closeness_weight)
        .def_readwrite("min_closeness_weight", &MakePlanarFacesPlus::MakePlanarSettings::min_closeness_weight)
        .def_readwrite("verbose", &MakePlanarFacesPlus::MakePlanarSettings::verbose)
        .def_readwrite("projection_eps", &MakePlanarFacesPlus::MakePlanarSettings::projection_eps)
        .def_readwrite("w_identity", &MakePlanarFacesPlus::MakePlanarSettings::w_identity)
        .def_readwrite("convergence_eps", &MakePlanarFacesPlus::MakePlanarSettings::convergence_eps);
    m.def("make_planar_faces", &MakePlanarFacesPlus::make_planar_faces, "Continuous optimization that makes quad faces planar with minimal geometric loss.");
}
