#pragma once

#include <vector>
#include <Eigen/Core>
#include <Eigen/SparseCore>

namespace MakePlanarFacesPlus
{

using Vec3d = Eigen::Vector3d;
using SparseMatrix = Eigen::SparseMatrix<double>;
using Triplet = Eigen::Triplet<double>;

constexpr double theodorus_constant = 1.7320508075688772;

struct MakePlanarSettings
{
	int optimization_rounds = 10;
	int max_iterations = 200;

	// Optimization settings
	double initial_closeness_weight = 1.0;
	double min_closeness_weight = 0.1;

	// Optimizer settings
	bool verbose = true;
	double projection_eps = 1e-9;
	double w_identity = 1e-9;
	double convergence_eps = 1e-6;
};

double compute_bounding_box_diameter(const std::vector<Vec3d>& vertices);

std::vector<Vec3d> make_planar_faces(const std::vector<Vec3d>& vertices, const std::vector<std::vector<int>>& faces, const std::vector<int>& fixed_vertices, const MakePlanarSettings& settings);

void say_hello();

}