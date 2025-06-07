# path to blender executable
$blender_path = "C:\Program Files\Blender Foundation\Blender 4.4\"

# all paths to copy
$base_path = $PSScriptRoot + "\"
$addon_path = $base_path + "addon\"
$lean_addon_path = $base_path + "lean_addon\"
$parent_path = Split-Path -parent $base_path
$mpfp_wheels_path = $parent_path + "\mpfp\wheelhouse\"

# filenames
$manifest = "blender_manifest.toml"
$license = "LICENSE"
$initfile = "__init__.py"

# target path
$build_path = ($PSScriptRoot + "\MakePlanarFacesPlusBuilds\")

if (!(Test-Path ($build_path))){
    New-Item -Path $build_path -ItemType Directory
}

# if it exists, delete old folder
if (Test-Path ($lean_addon_path)) {
    Write-Output "Removing old lean addon..."
    Remove-Item -Recurse ($lean_addon_path)
}

# copy everything necesarry for building to a new lean addon folder
New-Item -Path $lean_addon_path -ItemType Directory
Copy-Item ($addon_path + $manifest) -Destination $lean_addon_path
Copy-Item ($base_path + $license) -Destination $lean_addon_path
# remove first 10 lines of init file (hack to delete the blinfo)
Get-Content ($addon_path + $initfile) | Select-Object -Skip 11 | Set-Content ($lean_addon_path + $initfile)
# wheels
New-Item -Path ($lean_addon_path + "wheels\") -ItemType Directory
Get-ChildItem -Path ($mpfp_wheels_path) -Recurse -File -Filter mpfp*.whl | Copy-Item -Destination ($lean_addon_path + "wheels\")
# automatically add names to manifest
$wheel_names = Get-ChildItem $($lean_addon_path + "wheels\") -File -Filter mpfp*.whl | ForEach-Object {
    '  "./wheels/' + $_.Name + '",'
}
$wheel_list = @()
$wheel_list += "wheels = ["
$wheel_list += $wheel_names
$wheel_list += "]"
$wheel_list | Add-Content ($lean_addon_path + $manifest)
# python files
Copy-Item ($addon_path + "makeplanarfacesplus\") -Recurse -Filter *.py -Destination ($lean_addon_path + "makeplanarfacesplus\")
if (Test-Path ($lean_addon_path + "makeplanarfacesplus\__pycache__")){
    Write-Output "Removing pycache..."
    Remove-Item ($lean_addon_path + "makeplanarfacesplus\__pycache__")
}

# call the blender build command
Start-Process -NoNewWindow -FilePath ($blender_path + "blender.exe") -ArgumentList "--command extension build --source-dir $lean_addon_path --output-dir $build_path --split-platforms"
Start-Process -NoNewWindow -FilePath ($blender_path + "blender.exe") -ArgumentList "--command extension build --source-dir $lean_addon_path --output-dir $build_path"
