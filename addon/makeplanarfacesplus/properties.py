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
    max_iters: IntProperty(
        name="Max Iterations",
        description="The maximum number of optimization rounds",
        default=200,
        min=0
    )
    closeness_weight: FloatProperty(
        name="Shape Preservation Weight",
        description="Controls the force that pulls vertices to their original position",
        default=1,
        min=0       
    )
    closeness_weight_decay: FloatProperty(
        name="Shape Preservation Weight Decay",
        description="Values between 0 and 1 will relax the shape preservation over time",
        default=0.95,
        min=0,
        max=1
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
    