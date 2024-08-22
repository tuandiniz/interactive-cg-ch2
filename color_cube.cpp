//
// Created by tuan on 30/05/24.
//
#include <iostream>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <string>

#define GLFW_INCLUDE_GLCOREARB
#define GL_GLEXT_PROTOTYPES
#include <GLFW/glfw3.h>
#include "utils.h"

#define BUFFER_OFFSET(bytes) ((GLvoid*) (bytes))

using namespace glm;
using namespace std;

typedef vec4 point4;
typedef vec4 color4;

int vertexCount = 0;
int rotationAxisIndex = 0;
bool isPerspective = false;

void populateBuffers(vector<point4>& pointsArray, vector<color4>& colorArray) {
    GLuint abuffer;
    glGenVertexArrays(1, &abuffer);
    glBindVertexArray(abuffer);

    vertexCount = pointsArray.size();

    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(point4) * vertexCount * 2, nullptr, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertexCount * sizeof(point4), pointsArray.data());
    glBufferSubData(GL_ARRAY_BUFFER, vertexCount * sizeof(point4), vertexCount * sizeof(point4), colorArray.data());
}

void quad(const point4 vertexList[], const color4 colorList[],
          vector<point4>& pointsArray, vector<color4>& colorArray,
          const int a, const int b, const int c, const int d) {

    // Triangle strip
    pointsArray.push_back(vertexList[a]);
    colorArray.push_back(colorList[a]);

    pointsArray.push_back(vertexList[b]);
    colorArray.push_back(colorList[b]);

    pointsArray.push_back(vertexList[c]);
    colorArray.push_back(colorList[c]);

    pointsArray.push_back(vertexList[c]);
    colorArray.push_back(colorList[c]);

    pointsArray.push_back(vertexList[d]);
    colorArray.push_back(colorList[d]);

    pointsArray.push_back(vertexList[a]);
    colorArray.push_back(colorList[a]);
}

void generateColorCube() {

    const point4 vertexList[8] = {
        point4(-1.0, -1.0, 1.0, 1.0), point4(-1.0, -1.0, -1.0, 1.0),
        point4(-1.0, 1.0, -1.0, 1.0),point4(-1.0, 1.0, 1.0, 1.0),
        point4(1.0, -1.0, 1.0, 1.0), point4(1.0, -1.0, -1.0, 1.0),
        point4(1.0, 1.0, -1.0, 1.0),point4(1.0, 1.0, 1.0, 1.0)
    };

    const color4 colorList[8] = {
        color4(0.0, 0.0, 0.0, 1.0), color4(1.0, 0.0, 0.0, 1.0),
        color4(1.0, 0.0, 1.0, 1.0), color4(0.0, 0.0, 1.0, 1.0),
        color4(0.0, 1.0, 0.0, 1.0), color4(1.0, 1.0, 0.0, 1.0),
        color4(1.0, 1.0, 1.0, 1.0), color4(0.0, 1.0, 1.0, 1.0),
    };

    vector<point4> pointsArray;
    vector<color4> colorArray;

    quad(vertexList, colorList, pointsArray, colorArray, 0, 3, 2, 1);
    quad(vertexList, colorList, pointsArray, colorArray, 2, 3, 7, 6);
    quad(vertexList, colorList, pointsArray, colorArray, 5, 6, 7, 4);
    quad(vertexList, colorList, pointsArray, colorArray, 0, 1, 5, 4);
    quad(vertexList, colorList, pointsArray, colorArray, 1, 2, 6, 5);
    quad(vertexList, colorList, pointsArray, colorArray, 3, 0, 4, 7);

    populateBuffers(pointsArray, colorArray);
}

GLuint initShadersColorCube() {
    auto vShader = readFile("../color_and_cmt.vert");
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

        if ( compiled == GL_FALSE ){
            int length = 0;
            glGetShaderiv ( shaderIndex, GL_INFO_LOG_LENGTH, &length );
            char* message = ( char* ) alloca ( length * sizeof ( char ) );
            glGetShaderInfoLog ( shaderIndex, length, &length, message );
            cout << "Failed to compile " << ( type == GL_VERTEX_SHADER ? "vertex" : "fragment" ) <<  "shader" << endl;
            cout << message << endl;
            glDeleteShader ( shaderIndex );
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
    glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

    const GLuint locColors = glGetAttribLocation(program, "vColor");
    glEnableVertexAttribArray(locColors);
    glVertexAttribPointer(locColors, 4, GL_FLOAT, GL_FALSE, 0
                          , BUFFER_OFFSET(vertexCount * sizeof(point4)));

    return program;
}

void displayColorCube(GLFWwindow* window) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);

    // Draw
    glfwSwapBuffers(window);
}

void idleFunc(GLFWwindow* window, double& delta, GLuint& modelViewLoc, mat4& transform, GLuint& projectionLoc) {

    // Speed in rads/s
    float angularVelocity = radians(120.0f);
    float rotation = angularVelocity * delta;

    vec3 rotationAxis = vec3();
    rotationAxis[rotationAxisIndex] = 1.0;

    mat4 projection(1.0);
    projection[2][2] = -0.1;
    projection[0][0] = 0.5;

    if (isPerspective) {
        projection = glm::frustum(-1.0, 1.0, -0.5, 0.5, 1.5, 10.0);
    }

    transform = rotate(mat4(1.0), rotation, rotationAxis) * transform;

//    glUniformMatrix4fv(modelViewLoc, 1, GL_FALSE, value_ptr(transform));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, value_ptr(projection));
    displayColorCube(window);
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (action == GLFW_PRESS) {
        if (button == GLFW_MOUSE_BUTTON_RIGHT) {
            rotationAxisIndex = 0;
        } else if (button == GLFW_MOUSE_BUTTON_LEFT) {
            rotationAxisIndex = 1;
        } else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
            rotationAxisIndex = 2;
        }
    }
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_P && action == GLFW_PRESS) {
        isPerspective = true;
    }

    if (key == GLFW_KEY_O && action == GLFW_PRESS)  {
        isPerspective = false;
    }
}

int initDisplayColorCube() {

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    GLFWwindow* window = glfwCreateWindow(1200, 600, "Hello World", nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    generateColorCube();
    auto programId = initShadersColorCube();

    glfwSetWindowRefreshCallback(window, &displayColorCube);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetKeyCallback(window, keyCallback);

    glClearColor(1.0, 1.0, 1.0, 1.0);
    glfwSwapInterval(1);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    auto previous = glfwGetTime();

    auto transform = mat4(1.0);
    transform = scale(transform, vec3(0.5, 0.5, 0.5)); // basic transformation
    GLuint modelViewLoc = glGetUniformLocation(programId, "modelView");
    GLuint projectionLoc = glGetUniformLocation(programId, "projection");

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        auto now = glfwGetTime();
        auto delta = now - previous;

        if (delta > 0.00833) { // 120 FPS
            previous = now;
            idleFunc(window, delta, modelViewLoc, transform, projectionLoc);
            auto translate = glm::translate(mat4(1.0), vec3(0.7, 0.0, -2.8));
            glUniformMatrix4fv(modelViewLoc, 1, GL_FALSE, value_ptr(translate * transform));
        }

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

int color_cube() {
    return initDisplayColorCube();
}

int main() {
    return color_cube();
}