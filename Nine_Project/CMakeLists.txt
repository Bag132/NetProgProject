# CMakeList.txt : CMake project for Nine_Project, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("Nine_Project")

# Add source to this project's executable.
add_executable (Nine_Project "Nine_Project.cpp" "Nine_Project.h")
find_package(glad CONFIG REQUIRED)
target_link_libraries(main PRIVATE glad::glad)
find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(main PRIVATE glfw)
# TODO: Add tests and install targets if needed.
