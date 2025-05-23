
cmake_minimum_required(VERSION 3.31)
project("makeplanarfacesplus")
set(CMAKE_CXX_STANDARD 17)

set(APP_SOURCE "${PROJECT_SOURCE_DIR}/src/app")
set(MODULE_SOURCE "${PROJECT_SOURCE_DIR}/src/module")
set(EXTERNALS "${PROJECT_SOURCE_DIR}/extern")

# set(DEigen3_DIR "/eigen-3.4.0/cmake/" CACHE PATH "The path to install to using the install target")
# list(APPEND CMAKE_PREFIX_PATH "C:/Users/Anton/Dev/libs;")
#list(APPEND CMAKE_PREFIX_PATH "C:/Users/Anton/Dev/libs/eigen-3.4.0")

# pybind11
add_subdirectory(${EXTERNALS}/pybind11)

# add_subdirectory(${EXTERNALS}/eigen-3.4.0)
# add_subdirectory(${EXTERNALS}/TINYAD)

# message(${Eigen3_DIR})
find_package (Eigen3 3.3 REQUIRED NO_MODULE)

# module
pybind11_add_module (
    testmodule 
    ${MODULE_SOURCE}/module.cpp
)

target_include_directories (
    testmodule 
    PRIVATE ${MODULE_SOURCE}
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
)
