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

There are two objective terms that both favor shape preservation. One penalizes the distance of all vertices to their original position and the other one penalizes changes in edge lengths. You can blend these two objectives via the `Edge Length Preservation Factor` parameter: A value of 0 will turn off the edge length objective, a value of 1 will turn off the vertex distance objective. The default value 0.5 will lead to a balance between the two.

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

## Settings & Performance
To gain more insight into how each parameter effects the optimization process, I conducted a few small experiments. You can use the insights gained from them to adjust the optimizer settings.

### The good news
For most low-poly models with around 100 to 1k vertices, the default settings produce good quality results in a couple of seconds. The plots below show the algorithms behaviour when tested on Blenders "Suzanne" mesh with an intial shape preservation weight of 5. You can already get a planar mesh (face distortion < 1e-16) after 30 optimization rounds with as little as 2 optimization steps per round. 

<p float="middle">
    <img src ="images/SuzanneLowResTimes.png" width=370>
    <img src ="images/SuzanneLowResHeatmap.png" width=370>
</p>

<sub>The images above show results on the "Suzanne" mesh.</sub> 

### The bad news

Due to the global optimization approach, the provided operator quickly becomes slow on large meshes. Also note that some meshes are way harder to make planar than others. For tricky inputs, the optimization process can slow down a lot and is likely to get stuck in local minima. One example for this is the "Suzanne" Mesh with one round of subdivision applied to it. You have to increase optimization rounds and iterations per round to get consistently good results. In some rare cases the optimizer fails (black squares). If this happens to you, just restart the process with slightly different settings. 

<p float="middle">
    <img src ="images/SuzanneHighResTimes.png" width=370>
    <img src ="images/SuzanneHighResHeatmap.png" width=370>
</p>

<sub>The images above show results on the "Suzanne" mesh with one applied subdivision round.</sub> 

## Installation
### Blender Extension
Download the addon [here](https://github.com/AntonFlorey/MakePlanarFacesPlus/releases). After that, start Blender (version 4.4) and go to  
 **Edit->Preferences->Get-Extensions**. Click on the little drop-down menu in the upper right corner and select `Install from Disk...`. Lastly, select the downloaded zip-folder. The addon should now be installed.

### Build for your system
Since this addon relies on some code written in C++, you need to compile it on your system if no available release works for you. It took me quite a while to get this working on windows, but maybe you reading this are a bit more tech savvy than I am. Here are the steps needed to built this addon for your system:

1. Make sure you have a C++ compiler and Cmake installed. 
2. Install OpenMP and make sure Cmake can locate it via `find_package`.
3. You also need the python-dev package of python **version 3.11** (for Blender 4.4). In general, you have to compile for the python version that comes with your Blender installation.
4. Now git clone the [mpfp repository](https://github.com/AntonFlorey/mpfp) and navigate to the mpfp root folder. Make sure to clone with the option `--recurse-submodules` set.
5. From here, type the following command to build the python module:
```bash
# Make sure to use python3.11 here!
python -m pip install build # if not installed already
python -m build
```
6. The easiest way to create a working Blender extension now is to clone this repository and add the wheel you just created (should be in a folder named *dist*) to the *addon/wheels* folder. Then add a list called `wheels` to the `blender_manigest.toml` file, containing the path to your wheel. It should look like this:

```python
wheels = ["./wheels/name-of-your-wheel.whl"]
```
7. Now zip the `addon` folder. You should now be able to install it as a Blender extension.

If you followed all steps successfully for an operating system not listed [here](https://github.com/AntonFlorey/MakePlanarFacesPlus/releases), **please** contact me so we can make this addon available for more people.

## Acknowledgements
Special thanks to Patrick and all other contributors to TinyAD! This project would have been impossible without this awesome library.
