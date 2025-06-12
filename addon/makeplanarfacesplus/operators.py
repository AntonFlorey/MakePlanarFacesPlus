import bpy
import bmesh
import numpy as np
import mpfp
from mathutils import Vector
from bpy.props import BoolProperty
from bpy.props import FloatProperty
from bpy.props import IntProperty

def _active_object_is_edit_mesh(context : bpy.types.Context):
    active_object = context.active_object
    return active_object is not None and active_object.type == 'MESH' and context.mode == 'EDIT_MESH'

def write_custom_split_property_row(layout : bpy.types.UILayout, text, data, prop_name, split_factor, active=True):
    custom_row = layout.row().split(factor=split_factor, align=True)
    col_1, col_2 = (custom_row.column(), custom_row.column())
    col_1.label(text=text)
    col_2.prop(data, prop_name, text="")
    custom_row.active = active

class MESH_OT_MakePlanarFacesPlusOperator(bpy.types.Operator):
    """ Make all faces of the selected mesh planar. """
    bl_label = "Make Planar Faces Plus"
    bl_idname  = "mesh.make_planar_faces_plus"
    bl_options = { "UNDO" }

    fix_selected_vertices: BoolProperty(
        name="Pin Selected Vertices",
        description="When set to true, all selected vertices stay in place",
        default=True
    )
    optimization_rounds: IntProperty(
        name="Optimization Rounds",
        description="In each round, the shape preservation weight gets smaller. Last round optimizes for planarity only",
        default=50,
        min=0
    )
    max_iterations_per_round: IntProperty(
        name="Max Iterations per Round",
        description="The maximum number of inner optimization rounds",
        default=5,
        min=0
    )
    initial_shape_preservation_weight: FloatProperty(
        name="Intial Shape Preservation Weight",
        description="Controls the initial force that pulls vertices to their original position",
        default=5,
        min=0
    )
    target_shape_preservation_weight: FloatProperty(
        name="Target Shape Preservation Weight",
        description="Controls the final force that pulls vertices to their original position",
        default=0,
        min=0       
    )
    edge_length_preservation_blend_factor: FloatProperty(
        name="Edge Length Preservation Factor",
        description="Controls how much edge lengths should be preserved",
        default=0.5,
        min=0,
        max=1,
        subtype="FACTOR"
    )
    verbose: BoolProperty(
        name="Print to console",
        description="When set to true, every optimization round info gets printed to the console",
        default=True
    )
    convergence_eps: FloatProperty(
        name="Convergence Eps",
        description="Optimization is stopped early when the newton decrement or objective function improvement is smaller than this value",
        default=1e-16,
        min=0
    )

    def draw(self, context):
        layout = self.layout
        split_factor = 0.7
        write_custom_split_property_row(layout, "Pin Selected Vertices", self.properties, "fix_selected_vertices", split_factor)
        write_custom_split_property_row(layout, "Edge Length Preservation Factor", self.properties, "edge_length_preservation_blend_factor", split_factor)
        # header, subpanel = layout.panel("Advanced Settings" ,default_closed=True)
        layout.separator()
        layout.label(text="Advanced Settings", icon="SETTINGS")
        write_custom_split_property_row(layout, "Optimization Rounds", self.properties, "optimization_rounds", split_factor)
        write_custom_split_property_row(layout, "Max Iterations per Round", self.properties, "max_iterations_per_round", split_factor)
        write_custom_split_property_row(layout, "Initial Shape Preservation Weight", self.properties, "initial_shape_preservation_weight", split_factor)
        write_custom_split_property_row(layout, "Target Shape Preservation Weight", self.properties, "target_shape_preservation_weight", split_factor)
        write_custom_split_property_row(layout, "Convergence Eps", self.properties, "convergence_eps", split_factor)
        write_custom_split_property_row(layout, "Print Optimization Info", self.properties, "verbose", split_factor)

    def invoke(self, context, event):
        wm = context.window_manager
        return wm.invoke_props_dialog(self, title="Make Planar Faces Options")
    
    def execute(self, context):
        ao = context.active_object
        active_mesh = ao.data
        active_bmesh = bmesh.from_edit_mesh(active_mesh)
        active_bmesh.verts.ensure_lookup_table()

        # Collect all mesh data
        selected_vertices = [v.index for v in active_bmesh.verts if v.select]
        vertex_index_map = {}
        inverse_vertex_index_map = {}
        compact_vertex_coords = []
        for i, vertex in enumerate(active_bmesh.verts):
            vertex_index_map[vertex.index] = i
            inverse_vertex_index_map[i] = vertex.index 
            compact_vertex_coords.append(vertex.co)
        compact_selected_vertices = [vertex_index_map[v_id] for v_id in selected_vertices]
        compact_faces = [[vertex_index_map[v.index] for v in f.verts] for f in active_bmesh.faces if len(f.verts) > 3]
        
        # Apply optimization settingsddddd
        make_planar_settings = mpfp.MakePlanarSettings()
        make_planar_settings.optimization_rounds = self.optimization_rounds
        make_planar_settings.max_iterations_per_round = self.max_iterations_per_round
        make_planar_settings.initial_shape_preservation_weight = max(self.initial_shape_preservation_weight, self.target_shape_preservation_weight)
        make_planar_settings.target_shape_preservation_weight = self.target_shape_preservation_weight
        make_planar_settings.edge_length_preservation_blend_factor = self.edge_length_preservation_blend_factor
        make_planar_settings.verbose = self.verbose
        make_planar_settings.convergence_eps = self.convergence_eps

        # Optimize  
        optimized_vertex_positions = mpfp.make_planar_faces(np.array(compact_vertex_coords), 
                                                            compact_faces, 
                                                            compact_selected_vertices if self.fix_selected_vertices else [],
                                                            make_planar_settings)
        
        # Write the result back to mesh
        for i, optimized_pos in enumerate(optimized_vertex_positions):
            active_bmesh.verts[inverse_vertex_index_map[i]].co = Vector(optimized_pos)
        
        bmesh.update_edit_mesh(active_mesh)

        # Done
        active_bmesh.free()

        # Toggle modes to update distortion analysis
        bpy.ops.object.mode_set(mode='OBJECT')
        bpy.ops.object.mode_set(mode='EDIT')

        return { "FINISHED" }

    @classmethod
    def poll(self, context):
        return _active_object_is_edit_mesh(context)
    
def draw_make_planar_plus_operator(self, context):
    """Draw the operator as an option on a menu."""
    layout = self.layout
    layout.operator(MESH_OT_MakePlanarFacesPlusOperator.bl_idname)

def register():
    bpy.utils.register_class(MESH_OT_MakePlanarFacesPlusOperator)
    bpy.types.VIEW3D_MT_edit_mesh_clean.append(draw_make_planar_plus_operator)

def unregister():
    bpy.utils.unregister_class(MESH_OT_MakePlanarFacesPlusOperator)
    bpy.types.VIEW3D_MT_edit_mesh_clean.remove(draw_make_planar_plus_operator)
