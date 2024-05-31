//
// Created by tuan on 30/05/24.
//
#include <iostream>
#include <fstream>
#include <glm/glm.hpp>
#include <vector>
#include <string>

#define GLFW_INCLUDE_GLCOREARB
#define GL_GLEXT_PROTOTYPES
#include <GLFW/glfw3.h>

#define BUFFER_OFFSET(bytes) ((GLvoid*) (bytes))

using namespace glm;
using namespace std;

typedef vec4 point4;
typedef vec4 color4;

int vertexCount = 0;

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

    pointsArray.push_back(vertexList[d]);
    colorArray.push_back(colorList[d]);
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

    quad(vertexList, colorList, pointsArray, colorArray, 0, 1, 3, 2);
    quad(vertexList, colorList, pointsArray, colorArray, 3, 2, 7, 6);
    quad(vertexList, colorList, pointsArray, colorArray, 4, 7, 5, 6);
    quad(vertexList, colorList, pointsArray, colorArray, 0, 1, 4, 5);
    quad(vertexList, colorList, pointsArray, colorArray, 2, 1, 6, 5);
    quad(vertexList, colorList, pointsArray, colorArray, 3, 0, 7, 4);

    populateBuffers(pointsArray, colorArray);
}

string readFile(const string& fileName);

void initShadersColorCube() {
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
}

void displayColorCube(GLFWwindow* window) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, vertexCount);

    // Draw
    glfwSwapBuffers(window);
}

int initDisplayColorCube() {

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
    generateColorCube();
    initShadersColorCube();

    glfwSetWindowRefreshCallback(window, &displayColorCube);
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

int color_cube() {
    return initDisplayColorCube();
}