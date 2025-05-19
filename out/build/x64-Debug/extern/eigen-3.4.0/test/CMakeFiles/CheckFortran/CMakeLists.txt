cmake_minimum_required(VERSION 3.31.6-msvc6)
set(CMAKE_MODULE_PATH "E:/Dev/MakePlanarFacesPlus/extern/eigen-3.4.0/cmake")
project(CheckFortran Fortran)
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/result.cmake"
  "set(CMAKE_Fortran_COMPILER \"${CMAKE_Fortran_COMPILER}\")\n"
  
  )