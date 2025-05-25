import bpy
from .properties import MakePlanarSettings
from .operators import MESH_OT_MakePlanarFacesPlusOperator

class VIEW3D_PT_MakePlanarFacesPlusPanel(bpy.types.Panel):
    bl_label = "Planar Plus"
    bl_idname  = "VIEW3D_PT_MakePlanarFacesPlusPanel"
    bl_space_type = "VIEW_3D"
    bl_region_type = "UI"
    bl_category = "Mesh"

    def draw(self, context: bpy.types.Context):
        layout = self.layout
        row = layout.row()
        row.label(text="Hello World :)")
        row = layout.row()
        row.operator(MESH_OT_MakePlanarFacesPlusOperator.bl_idname)

def register():
    bpy.utils.register_class(VIEW3D_PT_MakePlanarFacesPlusPanel)

def unregister():
    bpy.utils.unregister_class(VIEW3D_PT_MakePlanarFacesPlusPanel)