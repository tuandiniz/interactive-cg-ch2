#include <iostream>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

using namespace glm;
using namespace std;

typedef vec2 point2;

void init() {
    const int numPoints = 5;
    point2 points[numPoints];

    // A triangle in plane z = 0
    point2 vertices[3] = {point2(-1.0, -1.0), point2(0.0, 1.0), point2(1.0, 1.0)};

    // An arbitrary initial point inside the triangle
    points[0] = point2(0.25, 0.50);

    // Compute and store numPoints -1
    for (int i = 1; i < numPoints; i++) {
        auto midPoint = (vertices[rand() % 3] + points[i - 1]) / 2.0f;
        points[i] = midPoint;
    }
}

int glfTest();
int main() {
//    init();
//    return glfTest();

    auto c = 1;
    c++;
    cout << c << endl;
    c++;
    cout << c << endl;
}

int glfTest() {
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}