#include <iostream>
#include <fstream>
#include <glm/glm.hpp>
#include <vector>

#define GLFW_INCLUDE_GLCOREARB
#define GL_GLEXT_PROTOTYPES

#define BUFFER_OFFSET(bytes) ((GLvoid*) (bytes))

#include <GLFW/glfw3.h>


using namespace glm;
using namespace std;

typedef vec2 point2;

const int recursionDepth = 7;
int pointCount = 0;

void computePointsRecursive(point2 vertices[], vector<point2>& points, const int currentIteration, const int& maxDepth) {
    if(maxDepth == currentIteration) {
        for(int i = 0; i < 3; i++) {
            point2 vertex = vertices[i];
            points.push_back(vertices[i]);
        }

        return;
    }

    const point2 midVertex1 = (vertices[0] + vertices[1]) / 2.0f;
    const point2 midVertex2 = (vertices[1] + vertices[2]) / 2.0f;
    const point2 midVertex3 = (vertices[0] + vertices[2]) / 2.0f;

    point2 triangle1[] = {vertices[0], midVertex1, midVertex3};
    point2 triangle2[] = {midVertex1, vertices[1], midVertex2};
    point2 triangle3[] = {midVertex3, midVertex2, vertices[2]};

    computePointsRecursive(triangle1, points, currentIteration + 1, maxDepth);
    computePointsRecursive(triangle2, points, currentIteration + 1, maxDepth);
    computePointsRecursive(triangle3, points, currentIteration + 1, maxDepth);
}

void computeTriangles(vector<point2>& points, const int& maxDepth) {

    // A triangle in plane z = 0
    point2 vertices[3] = { point2(1.0, -1.0), point2(0.0, 1.0), point2(-1.0, -1.0)};

    computePointsRecursive(vertices, points, 1, maxDepth);
}

string readFile(const string& fileName);

void initShaders();

void initBuffersTriangles2D() {
    vector<point2> points;
    computeTriangles(points, recursionDepth);

    GLuint abuffer;
    glGenVertexArrays(1, &abuffer);
    glBindVertexArray(abuffer);

    pointCount = points.size();

    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(point2)*points.size(), points.data(), GL_STATIC_DRAW);
}

void displayTriangles2D(GLFWwindow* window) {
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, pointCount);

    // Draw
    glfwSwapBuffers(window);
    glFlush();
}

int initDisplayTriangles2D() {

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    GLFWwindow* window = glfwCreateWindow(1200, 1200, "Hello World", nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    initBuffersTriangles2D();
    initShaders();

    glfwSetWindowRefreshCallback(window, &displayTriangles2D);
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glPointSize(5.0);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

int sierpisky_triangles_2d() {
    return initDisplayTriangles2D();
}

