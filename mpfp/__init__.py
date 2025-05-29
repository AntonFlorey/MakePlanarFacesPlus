"""
A small geometry processing package for mesh planarization.

Here is a small example to get you started::

    # necessary imports 
    import numpy as np
    from mpfp import make_planar_faces, MakePlanarSettings
    # prepare mesh data (in praxis you would create this data from your mesh class)
    vertices = [np.array([0,0,0]), np.array([1,0,0]), np.array([1,1,0]), np.array([0,1,1])]
    faces = [[0,1,2,3]]
    fixed_vertices = [0,1,2]
    opt_settings = MakePlanarSettings()
    # here is a list of all available settings (with default values):
    opt_settings.optimization_rounds = 250
    opt_settings.max_iterations = 100
    opt_settings.closeness_weight = 5000
    opt_settings.min_closeness_weight = 0.0
    opt_settings.verbose = True
    opt_settings.projection_eps = 1e-9
    opt_settings.w_identity = 1e-9
    opt_settings.convergence_eps = 1e-16
    # optimize
    optimized_vertices = make_planar_faces(vertices, faces, fixed_vertices, opt_settings)
    # print the result
    print(optimized_vertices)
"""

from ._cpp_mpfp import MakePlanarSettings, make_planar_faces

__all__ = [
    "MakePlanarSettings",
    "make_planar_faces"
]
