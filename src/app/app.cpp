#include <iostream>

#include <Eigen/Dense>
#include <TinyAD/Scalar.hh>

#include <module.hh>

using Eigen::MatrixXd;

// Choose autodiff scalar type for 3 variables
using ADouble = TinyAD::Double<3>;

int main()
{
	std::cout << "Hello World" << std::endl;

	MatrixXd m(2, 2);
	m(0, 0) = 3;
	m(1, 0) = 2.5;
	m(0, 1) = -1;
	m(1, 1) = m(1, 0) + m(0, 1);
	std::cout << m << std::endl;

	// Init a 3D vector of active variables and a 3D vector of passive variables
	Eigen::Vector3<ADouble> x = ADouble::make_active({ 0.0, -1.0, 1.0 });
	Eigen::Vector3<double> y(2.0, 3.0, 5.0);

	// Compute angle using Eigen functions and retrieve gradient and Hessian w.r.t. x
	ADouble angle = acos(x.dot(y) / (x.norm() * y.norm()));
	Eigen::Vector3d g = angle.grad;
	Eigen::Matrix3d H = angle.Hess;

	std::cout << "TinyAD Gradient: " << g << std::endl;

	say_hello();

	// Simple planar faces test

	std::vector<Vec3d> simple_test_vertices = { Vec3d(0,0,0), Vec3d(1,0,0), Vec3d(1,1,0), Vec3d(0,1,1) };
	std::vector<Vec3d> simple_test_vertices_scaled = { Vec3d(0,0,0), Vec3d(2,0,0), Vec3d(2,2,0), Vec3d(0,2,2) };
	std::vector<std::vector<int>> simple_test_faces = { {0, 1, 2, 3} };
	std::vector<int> simple_test_constraints = { 0, 1, 2 };
	double simple_test_surface_area = 1;
	MakePlanarSettings simple_test_settings;
	simple_test_settings.max_iterations = 100;
	simple_test_settings.closeness_weight = 1.0;
	simple_test_settings.closeness_weight_decay = 0.99;
	simple_test_settings.convergence_eps = 1e-14;

	std::vector<Vec3d> simple_test_result = make_planar_faces(simple_test_vertices, simple_test_faces, simple_test_constraints, simple_test_surface_area, simple_test_settings);

	std::cout << "Simple planarization test result: \n" << simple_test_result[3] << std::endl;
	std::cout << "Scaled test..." << std::endl;

	std::vector<Vec3d> simple_test_result_scaled = make_planar_faces(simple_test_vertices_scaled, simple_test_faces, simple_test_constraints, 4, simple_test_settings);
	std::cout << "Simple planarization test result (scaled): \n" << simple_test_result_scaled[3] << std::endl;
}
