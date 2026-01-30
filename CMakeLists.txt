cmake_minimum_required(VERSION 3.28)
project(interactive_cg_ch2)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

link_libraries(glfw)
link_libraries(X11)
link_libraries(GL)
link_libraries(libjpeg.a)
link_libraries(pthread)


add_executable(interactive_cg_ch2 main.cpp
        siaerpisky_2d_triangles.cpp
        sierpisky_3d_color_triangles.cpp
        Scene.cpp
        Scene.h
        Node.cpp
        Node.h
        utils.h
        utils.cpp)

add_executable(color_cube color_cube.cpp
        utils.h
        utils.cpp)

add_executable(recursive_sphere recursive_sphere.cpp
        utils.h
        utils.cpp)

add_executable(textured_color_cube textured_color_cube.cpp
        utils.h
        utils.cpp)

add_executable(environment_mapped_cube environment_mapped_cube.cpp
        utils.h
        utils.cpp)