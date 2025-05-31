# MakePlanarFacesPlus

![banner](images/MakePlanarFacesPlusBanner.png)

A Blender Addon to flatten mesh faces

<a href="https://www.github.com/AntonFlorey/MakePlanarFacesPlus/releases"><img src="https://img.shields.io/github/v/release/AntonFlorey/MakePlanarFacesPlus" alt="Badge displaying release version." style="height:20px"/></a> <a href="https://www.github.com/AntonFlorey/MakePlanarFacesPlus/releases"><img src="https://img.shields.io/github/downloads/AntonFlorey/MakePlanarFacesPlus/total.svg" alt="Repo total downloads count." style="height:20px"/></a> <a href="https://github.com/AntonFlorey/MakePlanarFacesPlus/blob/main/LICENSE"><img src="https://img.shields.io/github/license/AntonFlorey/MakePlanarFacesPlus" alt="Badge displaying license." style="height:20px"/></a> <a href="https://github.com/AntonFlorey/MakePlanarFacesPlus"><img src="https://img.shields.io/github/stars/AntonFlorey/MakePlanarFacesPlus?style=social" alt="Badge displaying count of GitHub stars." style="height:20px"/></a>

<a href="https://github.com/patr-schm/TinyAD"><img src="https://img.shields.io/badge/Powered%20by-TinyAD-blue" alt="Badge referencing TinyAD." style="height:20px"/></a>

<a href="https://buymeacoffee.com/antonflorei" target="_blank"><img src="https://www.buymeacoffee.com/assets/img/custom_images/yellow_img.png" alt="Buy Me A Coffee" style="height: 37px !important;width: 170px !important;box-shadow: 0px 3px 2px 0px rgba(190, 190, 190, 0.5) !important;-webkit-box-shadow: 0px 3px 2px 0px rgba(190, 190, 190, 0.5) !important;" ></a>

## Usage
With the addon enabled, select any mesh object, switch to edit mode and navigate to the Mesh panel in the viewport sidebar (toggle with 'N') and click on the `Make Planar Faces Plus` button. You can also find the operator under **Mesh->Clean Up->Make Planar Faces Plus**.

## Details
Similar to Blender's built in `Make Planar Faces` operator, the method provided by this addon aims to make each face of a mesh planar. However, instead of updating positions locally, it solves a global optimization problem in order to make faces planar while preserving the objects shape as much as possible.

You can control the strength of this shape preservation objective via the `Shape Preservation Weight` and `Target Shape Preservation Weight` parameter. The algorithm will interpolate between the two while optimizing. If you struggle to get decent results, try increasing the `Shape Preservation Weight` and the number of optimization rounds.

The algorithm will always try to optimize the entire mesh. By enabling the `Fix Selected Vertices` option, all selected vertices will not be affected by the operator. This may be useful when you want to preserve certain features (In fact, this is what motivated me to develop this tool).

## Performance
Due to the global optimization approach, the provided operator quickly becomes slow on large meshes. Also note that some meshes are way harder to make planar than others. For tricky inputs, the optimization process can slow down a lot and is likely to get stuck in local minima. In these cases, it might be helpful to first triangulate very bad regions and run blenders *make planar faces* operator as a preprocessing step.

## Installation
### Blender Extension
Download the addon [here](https://github.com/AntonFlorey/MakePlanarFacesPlus/releases). After that, start Blender (version 4.4) and go to  
 **Edit->Preferences->Get-Extensions**. Click on the little drop-down menu in the upper right corner and select `Install from Disk...`. Lastly, select the downloaded zip-folder. The addon should now be installed.

### Build for your system
Since this addon relies on some code written in C++, you need to compile it on your system if no available release works for you. It took me quite a while to get this working on windows, but maybe you reading this are a bit more tech savvy than I am. Here are the steps needed to built this addon for your system:

1. Make sure you have a C++ compiler and Cmake installed. 
2. You also need the python-dev package of python **version 3.11** (for Blender 4.4). In general, you have to compile for the python version that comes with your Blender installation.
3. Install the [Eigen3](https://eigen.tuxfamily.org/index.php?title=Main_Page) C++ library and make sure Cmake can find it.
4. Now git clone this repository and navigate to the folder containing this README file. Make sure to also clone the submodules!
5. From here, type the following commands to build the python module:

```bash
# Current working directory must be MakePlanarFacesPlus
mkdir Builds (or some other folder name you like)
cd Builds
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release
```

6. Now build a python wheel. For this change the `prebuild_bin_file` variable in `setup.py` to the path to the `_cpp_mpfp.cp11-your_os_specs.*` file that is located in the *Builds* folder you just created. It should look like this:

```python
prebuild_bin_file = pathlib.Path(__file__).parent.resolve() / "Builds/_cpp_mpfp.cp11-your_os_specs.so"
```

7. Build the python wheel with the following command:

```bash
# Make sure to use the same python version used to compile the _cpp_mpfp module
python setup.py bdist_wheel
```

8. The easiest way to create a working addon now is to download an existing release [here](https://github.com/AntonFlorey/MakePlanarFacesPlus/releases). Unzip it and add the wheel you just created (should be in a folder called *dist*) to the *wheels* folder of the addon. Then add the path of the new wheel to the wheels list in the `blender_manigest.toml` file.

9. Zip everything again and install the addon as a python extension :)

If you followed all steps successfully for an operating system not listed [here](https://github.com/AntonFlorey/MakePlanarFacesPlus/releases), **please** contact me so we can make this addon available for more people.

## Acknowledgements
Special thanks to Patrick and all other contributors to TinyAD! This project would have been impossible without this awesome libraray.
