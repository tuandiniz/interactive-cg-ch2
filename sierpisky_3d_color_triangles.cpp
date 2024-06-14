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

typedef vec3 point3;

const int recursionDepth = 3;
int tetraPointCount;
point3 colorPallete[4] = {point3 {1.0, 0.0, 0.0}, point3 {0.0, 1.0, 0.0},
                    point3 {0.0, 0.0, 1.0},point3 {1.0, 1.0, 0.0}};

void computePointsRecursive(point3 vertices[], vector<point3>& points, vector<point3>& colors, const int currentIteration) {
    if(currentIteration == 0) {
        // triangle 1
        colors.push_back(colorPallete[0]);
        colors.push_back(colorPallete[0]);
        colors.push_back(colorPallete[0]);
        points.push_back(vertices[3]);
        points.push_back(vertices[1]);
        points.push_back(vertices[0]);

        // triangle 2
        colors.push_back(colorPallete[1]);
        colors.push_back(colorPallete[1]);
        colors.push_back(colorPallete[1]);
        points.push_back(vertices[3]);
        points.push_back(vertices[2]);
        points.push_back(vertices[1]);

        // triangle 3
        colors.push_back(colorPallete[2]);
        colors.push_back(colorPallete[2]);
        colors.push_back(colorPallete[2]);
        points.push_back(vertices[3]);
        points.push_back(vertices[0]);
        points.push_back(vertices[2]);

        // triangle 4
        colors.push_back(colorPallete[3]);
        colors.push_back(colorPallete[3]);
        colors.push_back(colorPallete[3]);
        points.push_back(vertices[2]);
        points.push_back(vertices[0]);
        points.push_back(vertices[1]);

        return;
    }

    const point3 midVertex1 = (vertices[0] + vertices[1]) / 2.0f;
    const point3 midVertex2 = (vertices[1] + vertices[2]) / 2.0f;
    const point3 midVertex3 = (vertices[0] + vertices[2]) / 2.0f;
    const point3 midVertex4 = (vertices[0] + vertices[3]) / 2.0f;
    const point3 midVertex5 = (vertices[1] + vertices[3]) / 2.0f;
    const point3 midVertex6 = (vertices[2] + vertices[3]) / 2.0f;

    point3 tetra1[4] = {vertices[0], midVertex1, midVertex3, midVertex4};
    point3 tetra2[4] = {midVertex1, vertices[1], midVertex2, midVertex5};
    point3 tetra3[4] = {midVertex3, midVertex2, vertices[2], midVertex6};
    point3 tetra4[4] = {midVertex4, midVertex5, midVertex6, vertices[3]};

    computePointsRecursive(tetra1, points, colors, currentIteration - 1);
    computePointsRecursive(tetra2, points, colors, currentIteration - 1);
    computePointsRecursive(tetra3, points, colors, currentIteration - 1);
    computePointsRecursive(tetra4, points, colors, currentIteration - 1);
}

void computeTriangles(vector<point3>& points, vector<point3>& colors, const int& depth) {

    // A tetrahedron the center of the unit cube
    point3 vertices[4] = { point3(-1.0, -1.0, 1.0), point3(0.0, 1.0, 1.0),
                           point3(1.0, -1.0, 1.0), point3(0.0, -0.25, -1.0)};

    computePointsRecursive(vertices, points, colors, depth);
}

string readFile(const string& fileName);

void initShaders3D() {
    auto vShader = readFile("../orto_colors.vert");
    auto fShader = readFile("../flat_color.frag");

    struct Shader {
        string src;
        GLenum type;
    } shaders[2] = {
            Shader{vShader, GL_VERTEX_SHADER},
            Shader{fShader, GL_FRAGMENT_SHADER}
    };

    GLuint program = glCreateProgram();

    for(const auto &[src, type]: shaders) {
        const GLuint shaderIndex = glCreateShader(type);
        const char* srcAsChar = src.c_str();

        glShaderSource(shaderIndex, 1, &srcAsChar, nullptr);
        glCompileShader(shaderIndex);

        GLint compiled;
        glGetShaderiv(shaderIndex, GL_COMPILE_STATUS, &compiled);

        if(!compiled) {
            cout << "Quebrou: " << type << endl;
            exit(EXIT_FAILURE);
        }

        glAttachShader(program, shaderIndex);
    }

    glLinkProgram(program);

    GLint linked;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);

    if(!linked) {
        cout << "Nao linkou!" << endl;
        exit(EXIT_FAILURE);
    }

    glUseProgram(program);

    const GLuint loc = glGetAttribLocation(program, "vPosition");
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

    const GLuint locColors = glGetAttribLocation(program, "vColor");
    glEnableVertexAttribArray(locColors);
    glVertexAttribPointer(locColors, 3, GL_FLOAT, GL_FALSE, 0
                          , BUFFER_OFFSET(tetraPointCount * sizeof(point3)));
}

void initBuffersTriangles3D() {
    vector<point3> points;
    vector<point3> colors;
    computeTriangles(points, colors, recursionDepth);

    GLuint abuffer;
    glGenVertexArrays(1, &abuffer);
    glBindVertexArray(abuffer);

    tetraPointCount = points.size();

    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(point3) * tetraPointCount * 2, nullptr, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, tetraPointCount * sizeof(point3), points.data());
    glBufferSubData(GL_ARRAY_BUFFER, tetraPointCount * sizeof(point3)
                    , tetraPointCount * sizeof(point3), colors.data());
}

void displayTriangles3D(GLFWwindow* window) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, tetraPointCount);

    // Draw
    glfwSwapBuffers(window);
}

int initDisplayTriangles3D() {

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
    initBuffersTriangles3D();
    initShaders3D();

    glfwSetWindowRefreshCallback(window, &displayTriangles3D);
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glEnable(GL_DEPTH_TEST);
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

int sierpisky_triangles_3d() {
    return initDisplayTriangles3D();
}

