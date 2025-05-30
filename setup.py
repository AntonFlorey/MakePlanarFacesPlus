import os
import pathlib
from setuptools import setup

from distutils.file_util import copy_file
from setuptools.command import build_ext
from setuptools import Extension

# modified version of prebuilt binaries package
# https://github.com/tim-mitchell/prebuilt_binaries
class PrebuiltExtension(Extension):
    def __init__(self, input_file, package=None):
        print("DEBUG INPUT FILE:", input_file)
        name = pathlib.Path(input_file).stem + pathlib.Path(input_file).suffix
        print("DEBUG NAME:", name)
        if package is not None:
            name = f'{package}.{name}'
        if not os.path.exists(input_file):
            raise ValueError(f'Prebuilt extension file not found\n{input_file}')
        self.input_file = input_file
        super().__init__(name, ['no-source-needed.c'])

# modified version of prebuilt binaries package
# https://github.com/tim-mitchell/prebuilt_binaries
class prebuilt_binary(build_ext.build_ext):
    def run(self):
        for ext in self.extensions:
            fullname = self.get_ext_fullname(ext.name)
            dest_filename = os.path.join(os.path.join(self.build_lib, "mpfp"), fullname)
            dest_folder = os.path.dirname(dest_filename)
            print("DEBUG", fullname)
            print("DEBUG", dest_filename)
            print("DEBUG", dest_folder)
            if not os.path.exists(dest_folder):
                os.makedirs(dest_folder)

            copy_file(
                ext.input_file, dest_filename, verbose=self.verbose,
                dry_run=self.dry_run
            )
        if self.inplace:
            self.copy_extensions_to_source()

prebuild_bin_file = pathlib.Path(__file__).parent.resolve() / "out/build/Release/_cpp_mpfp.cp311-win_amd64.pyd"

print(prebuild_bin_file)

ext_module = PrebuiltExtension(prebuild_bin_file)

with open("mpfp/README.md", "r") as f:
    long_description = f.read()

setup(
    name='mpfp',
    version='1.0.0',
    description="A small geometry processing package for mesh planarization",
    long_description=long_description,
    long_description_content_type="text/markdown",
    author="Anton Florey",
    author_email="anton.florey@googlemail.com",
    url="https://github.com/AntonFlorey/MakePlanarFacesPlus",
    packages=["mpfp"],
    license_files=("mpfp/LICENSE.txt",),
    cmdclass={
        'build_ext': prebuilt_binary,
    },
    ext_modules=[ext_module],
    install_requires=["numpy"],
    python_requires="~=3.11"
)
