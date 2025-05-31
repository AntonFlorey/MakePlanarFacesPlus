
cmake_minimum_required(VERSION 3.22)
project("makeplanarfacesplus")
set(CMAKE_CXX_STANDARD 20)

if (MSVC)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
endif (MSVC)

set(APP_SOURCE "${PROJECT_SOURCE_DIR}/src/app")
set(MODULE_SOURCE "${PROJECT_SOURCE_DIR}/src/module")
set(EXTERNALS "${PROJECT_SOURCE_DIR}/extern")
set (PYBIND11_PYTHON_VERSION "3.11")

# pybind11
add_subdirectory(${EXTERNALS}/pybind11)
# TinyAD
add_subdirectory(${EXTERNALS}/TINYAD)
include_directories("${EXTERNALS}/TINYAD/include")

# message(${Eigen3_DIR})
find_package (Eigen3 3.3 REQUIRED NO_MODULE)

# module (pybind11)
pybind11_add_module (
    _cpp_mpfp
    ${MODULE_SOURCE}/module.cpp
)

target_include_directories (
    _cpp_mpfp 
    PRIVATE ${MODULE_SOURCE}
)

target_link_libraries(
    _cpp_mpfp
    PRIVATE Eigen3::Eigen
)

# module
add_library(
    modulelib
    ${MODULE_SOURCE}/module.cpp
)

target_include_directories (
    modulelib 
    PRIVATE ${MODULE_SOURCE}
)

target_link_libraries(
    modulelib
    PRIVATE Eigen3::Eigen
    PRIVATE pybind11::embed
)

# app
add_executable (
    pybind11app
    ${APP_SOURCE}/app.cpp
)

target_include_directories (
    pybind11app 
    PRIVATE ${APP_SOURCE}
    PRIVATE ${MODULE_SOURCE}
)

target_link_libraries(
    pybind11app
    PRIVATE pybind11::embed
    Eigen3::Eigen
    modulelib
)
