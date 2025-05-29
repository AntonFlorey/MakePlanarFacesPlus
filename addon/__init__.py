bl_info = {
    "name": "MakePlanarFacesPlus",
    "author": "Anton Florey",
    "version": (1,0,0),
    "blender": (4,4,0),
    "location": "View3D",
    "warning": "",
    "wiki_url": "",
    "category": "Mesh"
}

if "bpy" in locals():
    import importlib
    importlib.reload(locals()["ui"])
    importlib.reload(locals()["operators"])
else:
    import bpy
    from .makeplanarfacesplus import operators
    from .makeplanarfacesplus import ui

def register():
    operators.register()
    ui.register()

def unregister():
    operators.unregister()
    ui.unregister()

if __name__ == "__main__":
    register()
