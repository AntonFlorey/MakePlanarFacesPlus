#pragma once

#include <vector>
#include <Eigen/Core>
#include <Eigen/SparseCore>

using Vec3d = Eigen::Vector3d;
using SparseMatrix = Eigen::SparseMatrix<double>;
using Triplet = Eigen::Triplet<double>;

struct MakePlanarSettings
{
	int optimization_rounds = 10;
	int max_iterations = 200;

	double initial_closeness_weight = 1.0;

	// optimization settings
	bool verbose = true;
	double projection_eps = 1e-9;
	double w_identity = 1e-9;
	double convergence_eps = 1e-6;
};

std::vector<Vec3d> make_planar_faces(const std::vector<Vec3d>& vertices, const std::vector<std::vector<int>>& faces, const std::vector<int>& fixed_vertices, double surface_area, const MakePlanarSettings& settings);

void say_hello();