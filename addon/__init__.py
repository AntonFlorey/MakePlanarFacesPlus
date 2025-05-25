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
    importlib.reload(locals()["properties"])
else:
    import bpy
    from .makeplanarfacesplus import properties
    from .makeplanarfacesplus import operators
    from .makeplanarfacesplus import ui

def register():
    # Properties first!
    properties.register()
    # Other stuff later
    operators.register()
    ui.register()

def unregister():
    operators.unregister()
    ui.unregister()
    properties.unregister()

if __name__ == "__main__":
    register()
