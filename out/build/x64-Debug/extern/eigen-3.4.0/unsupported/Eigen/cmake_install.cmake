# Install script for directory: E:/Dev/MakePlanarFacesPlus/extern/eigen-3.4.0/unsupported/Eigen

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "E:/Dev/MakePlanarFacesPlus/out/install/x64-Debug")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Devel" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/eigen3/unsupported/Eigen" TYPE FILE FILES
    "E:/Dev/MakePlanarFacesPlus/extern/eigen-3.4.0/unsupported/Eigen/AdolcForward"
    "E:/Dev/MakePlanarFacesPlus/extern/eigen-3.4.0/unsupported/Eigen/AlignedVector3"
    "E:/Dev/MakePlanarFacesPlus/extern/eigen-3.4.0/unsupported/Eigen/ArpackSupport"
    "E:/Dev/MakePlanarFacesPlus/extern/eigen-3.4.0/unsupported/Eigen/AutoDiff"
    "E:/Dev/MakePlanarFacesPlus/extern/eigen-3.4.0/unsupported/Eigen/BVH"
    "E:/Dev/MakePlanarFacesPlus/extern/eigen-3.4.0/unsupported/Eigen/EulerAngles"
    "E:/Dev/MakePlanarFacesPlus/extern/eigen-3.4.0/unsupported/Eigen/FFT"
    "E:/Dev/MakePlanarFacesPlus/extern/eigen-3.4.0/unsupported/Eigen/IterativeSolvers"
    "E:/Dev/MakePlanarFacesPlus/extern/eigen-3.4.0/unsupported/Eigen/KroneckerProduct"
    "E:/Dev/MakePlanarFacesPlus/extern/eigen-3.4.0/unsupported/Eigen/LevenbergMarquardt"
    "E:/Dev/MakePlanarFacesPlus/extern/eigen-3.4.0/unsupported/Eigen/MatrixFunctions"
    "E:/Dev/MakePlanarFacesPlus/extern/eigen-3.4.0/unsupported/Eigen/MoreVectorization"
    "E:/Dev/MakePlanarFacesPlus/extern/eigen-3.4.0/unsupported/Eigen/MPRealSupport"
    "E:/Dev/MakePlanarFacesPlus/extern/eigen-3.4.0/unsupported/Eigen/NonLinearOptimization"
    "E:/Dev/MakePlanarFacesPlus/extern/eigen-3.4.0/unsupported/Eigen/NumericalDiff"
    "E:/Dev/MakePlanarFacesPlus/extern/eigen-3.4.0/unsupported/Eigen/OpenGLSupport"
    "E:/Dev/MakePlanarFacesPlus/extern/eigen-3.4.0/unsupported/Eigen/Polynomials"
    "E:/Dev/MakePlanarFacesPlus/extern/eigen-3.4.0/unsupported/Eigen/Skyline"
    "E:/Dev/MakePlanarFacesPlus/extern/eigen-3.4.0/unsupported/Eigen/SparseExtra"
    "E:/Dev/MakePlanarFacesPlus/extern/eigen-3.4.0/unsupported/Eigen/SpecialFunctions"
    "E:/Dev/MakePlanarFacesPlus/extern/eigen-3.4.0/unsupported/Eigen/Splines"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Devel" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/eigen3/unsupported/Eigen" TYPE DIRECTORY FILES "E:/Dev/MakePlanarFacesPlus/extern/eigen-3.4.0/unsupported/Eigen/src" FILES_MATCHING REGEX "/[^/]*\\.h$")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("E:/Dev/MakePlanarFacesPlus/out/build/x64-Debug/extern/eigen-3.4.0/unsupported/Eigen/CXX11/cmake_install.cmake")

endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "E:/Dev/MakePlanarFacesPlus/out/build/x64-Debug/extern/eigen-3.4.0/unsupported/Eigen/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
