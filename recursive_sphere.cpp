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

const int recursionDepth = 6;
int tetraPointCount;
point3 colorPallete[4] = {point3 {1.0, 0.0, 0.0}, point3 {0.0, 1.0, 0.0},
                    point3 {0.0, 0.0, 1.0},point3 {1.0, 1.0, 1.0}};

void subdivideTriangle(const point3 v1, const point3 v2, const point3 v3, const int colorIndex,
        vector<point3>& points, vector<point3>& colors, const int currentIteration) {

    if(currentIteration == 0) {
        // triangle 1
        colors.push_back(colorPallete[colorIndex]);
        colors.push_back(colorPallete[colorIndex]);
        colors.push_back(colorPallete[colorIndex]);
        points.push_back(normalize(v1));
        points.push_back(normalize(v2));
        points.push_back(normalize(v3));

        return;
    }

    const point3 midVertex1 = (v1 + v2) / 2.0f;
    const point3 midVertex2 = (v2 + v3) / 2.0f;
    const point3 midVertex3 = (v3 + v1) / 2.0f;


    point3 triangle1[4] = {v1, midVertex1, midVertex3};
    point3 triangle2[4] = {midVertex1, v2, midVertex2};
    point3 triangle3[4] = {midVertex3, midVertex2, v3};
    point3 triangle4[4] = {midVertex2, midVertex3, midVertex1};

    subdivideTriangle(triangle1[0], triangle1[1], triangle1[2], 0, points, colors, currentIteration - 1);
    subdivideTriangle(triangle2[0], triangle2[1], triangle2[2], 1, points, colors, currentIteration - 1);
    subdivideTriangle(triangle3[0], triangle3[1], triangle3[2], 2, points, colors, currentIteration - 1);
    subdivideTriangle(triangle4[0], triangle4[1], triangle4[2], 3, points, colors, currentIteration - 1);
}

void computeTriangles(vector<point3>& points, vector<point3>& colors, const int& depth) {

    // A tetrahedron the center of the unit cube
    point3 vertices[4] = { point3(-1.0, -1.0, -1.0), point3(0.0, 1.0, -1.0),
                           point3(1.0, -1.0, -1.0), point3(0.0, -0.25, 1.0)};

    subdivideTriangle(vertices[0], vertices[1], vertices[2], 0, points, colors, depth);
    subdivideTriangle(vertices[3], vertices[1], vertices[0], 0, points, colors, depth);
    subdivideTriangle(vertices[3], vertices[0], vertices[2], 0, points, colors, depth);
    subdivideTriangle(vertices[3], vertices[2], vertices[1], 0, points, colors, depth);
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
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
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

int recursive_sphere() {
    return initDisplayTriangles3D();
}

int main() {
    recursive_sphere();
}
