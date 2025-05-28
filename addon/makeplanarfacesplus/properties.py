import bpy

from bpy.props import BoolProperty
from bpy.props import FloatProperty
from bpy.props import IntProperty

class MakePlanarSettings(bpy.types.PropertyGroup):
    fix_selected_vertices: BoolProperty(
        name="Fix Selected Verts",
        description="When set to true, all selected vertices stay in place",
        default=True
    )
    optimization_rounds: IntProperty(
        name="Optimization Rounds",
        description="In each round, the shape preservation weight gets smaller. Last round optimizes for planarity only.",
        default=100,
        min=0
    )
    max_iters: IntProperty(
        name="Max Iterations",
        description="The maximum number of inner optimization rounds",
        default=100,
        min=0
    )
    closeness_weight: FloatProperty(
        name="Shape Preservation Weight",
        description="Controls the initial force that pulls vertices to their original position",
        default=5000,
        min=0
    )
    min_closeness_weight: FloatProperty(
        name="Target Shape Preservation Weight",
        description="Controls the final force that pulls vertices to their original position",
        default=0,
        min=0       
    )
    verbose: BoolProperty(
        name="Print to console",
        description="When set to true, every optimization iteration gets printed to the console",
        default=True
    )
    convergence_eps: FloatProperty(
        name="Convergence Eps",
        description="Optimization is stopped early when the newton decrement is smaller than this value",
        default=1e-16,
        min=0
    )

def register():
    bpy.utils.register_class(MakePlanarSettings)

def unregister():
    bpy.utils.unregister_class(MakePlanarSettings)
    