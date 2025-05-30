# Make Planar Faces Plus
A small geometry processing package for mesh planarization written in C++.

<a href="https://github.com/patr-schm/TinyAD"><img src="https://img.shields.io/badge/Powered%20by-TinyAD-blue" alt="Badge referencing TinyAD." style="height:20px"/></a>

## Here is an example to get you started

```python
# necessary imports 
from mpfp import make_planar_faces, MakePlanarSettings
# prepare mesh data (in praxis you would create this data from your mesh)
vertices = [[0,0,0], [1,0,0], [1,1,0], [0,1,1]]
faces = [[0,1,2,3]]
fixed_vertices = [0,1,2]
# here is a list of all available settings (with default values):
opt_settings = MakePlanarSettings()
opt_settings.optimization_rounds = 100
opt_settings.max_iterations = 100
opt_settings.closeness_weight = 10
opt_settings.min_closeness_weight = 0.0
opt_settings.verbose = True
opt_settings.projection_eps = 1e-16
opt_settings.w_identity = 1e-16
opt_settings.convergence_eps = 1e-16
# optimize
optimized_vertices = make_planar_faces(vertices, faces, fixed_vertices, opt_settings)
# print the result
print(optimized_vertices)
```

## How to encode your Mesh
You provide your mesh to the `make_planar_faces` function via the two parameters:

- `vertices`: A list of 3D vertex coordinates. You can provide them as a 2d list or a numpy array with shape (n, 3).
- `faces`: A list of all mesh faces. Each face has to be provided as a list of vertex indices in ccw or cw order.

## Details
The function provided by this module aims to make each face of a mesh planar. It solves a global optimization problem in order to make faces planar while preserving the objects shape as much as possible.

You can control the strength of this shape preservation objective via the `closeness_weight` and `min_closeness_weight` parameter. The algorithm will interpolate between the two while optimizing. If you struggle to get decent results, try increasing the `closeness_weight` and the number of optimization rounds.

The algorithm will always try to optimize the entire mesh. By providing the index list `fixed_vertices`, all selected vertices will not be ignored by the optimizer. This may be useful when you want to preserve certain features.
