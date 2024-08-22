cmake_minimum_required(VERSION 3.28)
project(interactive_cg_ch2)

set(CMAKE_CXX_STANDARD 17)

link_libraries(glfw)
link_libraries(GL)

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
