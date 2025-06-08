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

You can control the strength of this shape preservation objective via the `Intial Shape Preservation Weight` and `Target Shape Preservation Weight` parameter. The algorithm will interpolate between the two while optimizing. 

How fast the shape preservation weight decays is controlled by the `Optimization Rounds` setting. If set to 2, the first round will use the initial weight, the second round the target weight. More rounds will add more steps inbetween these two values, leading to a more graceful descent. 

The `Max Iterations per Round` setting determines how many optimization steps are performed per round (so for one weight value). A round will be stopped early if the objective funtion improvement falls below the `Convergence Eps` threshold. 

Here is an overview of the optimization process:

```python
# python pseudo-code of the optimization process
shape_weight = "Intial Shape Preservation Weight"
for opt_round in range("Optimization Rounds"):
    for opt_step in range("Max Iterations per Round"):
        do_optimization_step()
        if improvement < "Convergence Eps":
            break
    # the decay factor is chosen such that
    # shape_weight = "Target Shape Preservation Weight" in the last round
    shape_weight *= decay_factor
```

The algorithm will always try to optimize the entire mesh. By enabling the `Pin Selected Vertices` option, all selected vertices will not be affected by the operator. This may be useful when you want to preserve certain features (In fact, this is what motivated me to develop this tool).Similar to Blender's built in `Make Planar Faces` operator, the method provided by this addon aims to make each face of a mesh planar. However, instead of updating positions locally, it solves a global optimization problem in order to make faces planar while preserving the objects shape as much as possible.

You can control the strength of this shape preservation objective via the `Intial Shape Preservation Weight` and `Target Shape Preservation Weight` parameter. The algorithm will interpolate between the two while optimizing. 

How fast the shape preservation weight decays is controlled by the `Optimization Rounds` setting. If set to 2, the first round will use the initial weight, the second round the target weight. More rounds will add more steps inbetween these two values, leading to a more graceful descent. 

The `Max Iterations per Round` setting determines how many optimization steps are performed per round (so for one weight value). A round will be stopped early if the objective funtion improvement falls below the `Convergence Eps` threshold. 

Here is an overview of the optimization process:

```python
# python pseudo-code of the optimization process
shape_weight = "Intial Shape Preservation Weight"
for opt_round in range("Optimization Rounds"):
    for opt_step in range("Max Iterations per Round"):
        do_optimization_step()
        if improvement < "Convergence Eps":
            break
    # the decay factor is chosen such that
    # shape_weight = "Target Shape Preservation Weight" in the last round
    shape_weight *= decay_factor
```

The algorithm will always try to optimize the entire mesh. By enabling the `Pin Selected Vertices` option, all selected vertices will not be affected by the operator. This may be useful when you want to preserve certain features (In fact, this is what motivated me to develop this tool).

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
3. Now git clone the [mpfp repository](https://github.com/AntonFlorey/mpfp) and navigate to the mpfp root folder. Make sure to clone with the option `--recurse-submodules` set.
4. From here, type the following command to build the python module:
```bash
# Make sure to use python3.11 here!
python -m pip install build # if not installed already
python -m build
```
5. The easiest way to create a working Blender extension now is to clone this repository and add the wheel you just created (should be in a folder called *dist*) to the *addon/wheels* folder. Then add a list called `wheels` to the `blender_manigest.toml` file, containing the path to your wheel. It should look like this:

```python
wheels = ["./wheels/name-of-your-wheel.whl"]
```
6. Now zip the `addon` folder. You should now be able to install it as a Blender extension.

If you followed all steps successfully for an operating system not listed [here](https://github.com/AntonFlorey/MakePlanarFacesPlus/releases), **please** contact me so we can make this addon available for more people.

## Acknowledgements
Special thanks to Patrick and all other contributors to TinyAD! This project would have been impossible without this awesome library.
