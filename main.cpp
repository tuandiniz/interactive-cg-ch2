#include <iostream>
#include <fstream>
#include <glm/glm.hpp>

#define GLFW_INCLUDE_GLCOREARB
#define GL_GLEXT_PROTOTYPES

#define BUFFER_OFFSET(bytes) ((GLvoid*) (bytes))

#include <GLFW/glfw3.h>


using namespace glm;
using namespace std;

typedef vec2 point2;

const int numPoints = 5000;

void computePoints(const int numPoints, point2* points) {

    // A triangle in plane z = 0
    point2 vertices[3] = {point2(-1.0, -1.0), point2(0.0, 1.0), point2(1.0, -1.0)};

    // An arbitrary initial point inside the triangle
    points[0] = point2(0.25, 0.50);

    // Compute and store numPoints -1
    for (int i = 1; i < numPoints; i++) {
        auto midPoint = (vertices[rand() % 3] + points[i - 1]) / 2.0f;
        points[i] = midPoint;
    }
}

string readFile(const string& fileName) {
    ifstream file(fileName);
    string line;
    string shaderSource  = "";
    if(file.is_open()) {
        while(getline(file, line)) {
            shaderSource.append(line);
            shaderSource.append("\n");
        }
    }

    return shaderSource;
}

void initShaders() {
    auto vShader = readFile("../orto.vert");
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
    glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
}

void initBuffers(const int numPoints) {
    point2 points[numPoints];
    computePoints(numPoints, points);

    GLuint abuffer;
    glGenVertexArrays(1, &abuffer);
    glBindVertexArray(abuffer);

    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
}

void display(GLFWwindow* window) {

    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_POINTS, 0, numPoints);

    // Draw
    glfwSwapBuffers(window);
}

int initDisplay() {

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    GLFWwindow* window = glfwCreateWindow(640, 640, "Hello World", nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    initBuffers(numPoints);
    initShaders();

    glfwSetWindowRefreshCallback(window, &display);
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glPointSize(2.0);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

//int sierpisky_triangles_2d();
int sierpisky_triangles_3d();

int main() {
    // initDisplay();
    // sierpisky_triangles_2d();
    sierpisky_triangles_3d();
}


