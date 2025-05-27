# MakePlanarFacesPlus
A Blender Addon to flatten mesh faces

<a href="https://www.github.com/AntonFlorey/MakePlanarFacesPlus/releases"><img src="https://img.shields.io/github/v/release/AntonFlorey/MakePlanarFacesPlus" alt="Badge displaying release version." style="height:20px"/></a> <a href="https://www.github.com/AntonFlorey/MakePlanarFacesPlus/releases"><img src="https://img.shields.io/github/downloads/AntonFlorey/MakePlanarFacesPlus/total.svg" alt="Repo total downloads count." style="height:20px"/></a> <a href="https://github.com/AntonFlorey/MakePlanarFacesPlus/blob/main/LICENSE"><img src="https://img.shields.io/github/license/AntonFlorey/MakePlanarFacesPlus" alt="Badge displaying license." style="height:20px"/></a> <a href="https://github.com/AntonFlorey/MakePlanarFacesPlus"><img src="https://img.shields.io/github/stars/AntonFlorey/MakePlanarFacesPlus?style=social" alt="Badge displaying count of GitHub stars." style="height:20px"/></a>

<a href="https://github.com/patr-schm/TinyAD"><img src="https://img.shields.io/badge/Powered%20by-TinyAD-blue" alt="Badge referencing TinyAD." style="height:20px"/></a>

<a href="https://buymeacoffee.com/antonflorei" target="_blank"><img src="https://www.buymeacoffee.com/assets/img/custom_images/yellow_img.png" alt="Buy Me A Coffee" style="height: 37px !important;width: 170px !important;box-shadow: 0px 3px 2px 0px rgba(190, 190, 190, 0.5) !important;-webkit-box-shadow: 0px 3px 2px 0px rgba(190, 190, 190, 0.5) !important;" ></a>

## Usage
With the addon enabled, select any mesh object, switch to edit mode and navigate to the Mesh panel in the viewport sidebar (toggle with 'N') and click on the `Make Planar Faces Plus` button. You can also find the operator under **Mesh->Clean Up->Make Planar Faces Plus**.

## Details
Similar to Blender's built in `Make Planar Faces` operator, the method provided by this addon aims to make each face of a mesh planar. However, instead of updating positions locally, it solves a global optimization problem in order to make faces planar while preserving the objects shape as much as possible.

You can control the strength of this shape preservation objective via the `Shape Preservation Weight` and `Target Shape Preservation Weight` parameter. The algorithm will linearly interpolate between the two while optimizing. If you struggle to get decent results, try increasing the `Shape Preservation Weight` and the number of optimization rounds.

The algorithm will always try to optimize the entire mesh. By enabling the `Fix Selected Vertices` option, all selected vertices will not be affected by the operator. This may be useful when you want to preserve certain features of your mesh (In fact, this is what motivated me to develop this tool).

## Installation
If you are working on windows, the installation should be straightforward. For all of you coming from a different os, this might be a lot more complicated.
### Blender Extension
For all windows users, download the addon [here](https://github.com/AntonFlorey/MakePlanarFacesPlus/releases). After that, start Blender (version 4.4) and go to  
 **Edit->Preferences->Get-Extensions**. Click on the little drop-down menu in the upper right corner and select `Install from Disk...`. Lastly, select the downloaded zip-folder. The addon should now be installed.

### Build for your system
Since this addon relies on some code written in C++, you need to compile it first for other operating systems than windows. It took me quite a while to get this working on windows, but maybe you reading this are a bit more tech savvy than I am. Here are the steps needed to built this addon for your system:

1. Make sure you have a C++ compiler and Cmake installed. 
2. You also need the python-dev package of python **version 3.11** (for Blender 4.4). In general, you have to compile for the python version that comes with your Blender installation.
3. Install the [Eigen3](https://eigen.tuxfamily.org/index.php?title=Main_Page) C++ library and make sure Cmake can find it.
4. Now git clone this repository and navigate to the folder containing this README file. Make sure to also clone the submodules!
5. From here, type the following commands to build the python module:

```bash
mkdir Builds (or some other folder name you like)
cd Builds
cmake -DPYTHON_EXECUTABLE="path to your python3.11 installation" -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release
```

6. Locate the `mpfpmodule.cp311-your-os-specs` python extension file in your Build folder.
7. Now download the [windows version](https://github.com/AntonFlorey/MakePlanarFacesPlus/releases) of this addon, unzip it and replace the `mpfpmodule.cp311-win_amd64` file in the `makeplanarfacesplus/cpplibs` folder with your freshly compiled python module. 
8. Zip everything again and install the addon as a python extension :)

If you followed there steps successfully for an operating system not listed [here](https://github.com/AntonFlorey/MakePlanarFacesPlus/releases), **please** contact me so we can make this addon available for more people.
