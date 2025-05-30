#pragma once

#include <vector>
#include <Eigen/Core>
#include <Eigen/SparseCore>

namespace MakePlanarFacesPlus
{

using Vec3d = Eigen::Vector3d;
using SparseMatrix = Eigen::SparseMatrix<double>;
using Triplet = Eigen::Triplet<double>;

struct MakePlanarSettings
{
	int optimization_rounds = 100;
	int max_iterations = 100;

	// Optimization settings
	double initial_closeness_weight = 10.0;
	double min_closeness_weight = 0.0;

	// Optimizer settings
	bool verbose = true;
	double projection_eps = 1e-16;
	double w_identity = 1e-16;
	double convergence_eps = 1e-16;
};

std::vector<Vec3d> make_planar_faces(const std::vector<Vec3d>& vertices, const std::vector<std::vector<int>>& faces, const std::vector<int>& fixed_vertices, const MakePlanarSettings& settings);

}