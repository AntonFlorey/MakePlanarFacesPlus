import bpy
import bmesh
import numpy as np
from mathutils import Vector
from bpy.props import PointerProperty

from .properties import MakePlanarSettings
from .cpplibs import testmodule

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
    """ Make all quad faces of the selected mesh planar. N>4 gons are not supported at the moment. """
    bl_label = "Make Planar Faces Plus"
    bl_idname  = "mesh.make_planar_faces_plus"
    bl_options = { "UNDO" }

    opt_settings : PointerProperty(type=MakePlanarSettings)

    def draw(self, context):
        layout = self.layout
        opt_settings : MakePlanarSettings = self.opt_settings
        write_custom_split_property_row(layout, "Fix Selected Vertices", opt_settings, "fix_selected_vertices", 0.6)
        write_custom_split_property_row(layout, "Max Iterations", opt_settings, "max_iters", 0.6)
        write_custom_split_property_row(layout, "Shape Preservation Weight", opt_settings, "closeness_weight", 0.6)
        write_custom_split_property_row(layout, "Shape Preservation Decay", opt_settings, "closeness_weight_decay", 0.6)
        write_custom_split_property_row(layout, "Convergence Eps", opt_settings, "convergence_eps", 0.6)

    def invoke(self, context, event):
        wm = context.window_manager
        return wm.invoke_props_dialog(self, title="Make Planar Faces Options")
    
    def execute(self, context):
        ao = context.active_object
        active_mesh = ao.data
        active_bmesh = bmesh.from_edit_mesh(active_mesh)
        active_bmesh.verts.ensure_lookup_table()
        opt_settings : MakePlanarSettings = self.opt_settings

        # Collect all mesh data
        selected_vertices = [v.index for v in active_bmesh.verts if v.select]
        mesh_area = sum([f.calc_area() for f in active_bmesh.faces])
        vertex_index_map = {}
        inverse_vertex_index_map = {}
        compact_vertex_coords = []
        for i, vertex in enumerate(active_bmesh.verts):
            vertex_index_map[vertex.index] = i
            inverse_vertex_index_map[i] = vertex.index 
            compact_vertex_coords.append(vertex.co)
        compact_selected_vertices = [vertex_index_map[v_id] for v_id in selected_vertices]
        compact_faces = [[vertex_index_map[v.index] for v in f.verts] for f in active_bmesh.faces if len(f.verts) > 3]
        
        # Apply optimization settings
        make_planar_settings = testmodule.MakePlanarSettings()
        make_planar_settings.max_iterations = opt_settings.max_iters
        make_planar_settings.closeness_weight = opt_settings.closeness_weight
        make_planar_settings.closeness_weight_decay = opt_settings.closeness_weight_decay
        make_planar_settings.verbose = opt_settings.verbose
        make_planar_settings.convergence_eps = opt_settings.convergence_eps

        # Optimize
        optimized_vertex_positions = testmodule.make_planar_faces(np.array(compact_vertex_coords), 
                                                                  compact_faces, 
                                                                  compact_selected_vertices if opt_settings.fix_selected_vertices else [],
                                                                  mesh_area,
                                                                  make_planar_settings)
        
        # Write the result back to mesh
        for i, optimized_pos in enumerate(optimized_vertex_positions):
            active_bmesh.verts[inverse_vertex_index_map[i]].co = Vector(optimized_pos)
        
        bmesh.update_edit_mesh(active_mesh)

        # Done
        active_bmesh.free()
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
