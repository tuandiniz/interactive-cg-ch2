#include <iostream>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#define GLFW_INCLUDE_GLCOREARB
#define GL_GLEXT_PROTOTYPES

#define BUFFER_OFFSET(bytes) ((GLvoid*) (bytes))

#include <GLFW/glfw3.h>


using namespace glm;
using namespace std;

typedef vec3 point3;

const int recursionDepth = 11;
int tetraPointCount;
int rotationAxisIndex = 0;

void subdivideTriangle(const point3 v1, const point3 v2, const point3 v3, vector<point3>& points, vector<point3>& normals, const int currentIteration) {

    if(currentIteration == 0) {
        // triangle 1
        points.push_back(v1);
        points.push_back(v2);
        points.push_back(v3);


        normals.push_back(v1);
        normals.push_back(v2);
        normals.push_back(v3);

        // Flat shading
        // vec3 normal = normalize(v1 + v2 + v3);
        // normals.push_back(normal);
        // normals.push_back(normal);
        // normals.push_back(normal);

        return;
    }

    const point3 midVertex1 = normalize((v1 + v2) / 2.0f);
    const point3 midVertex2 = normalize((v2 + v3) / 2.0f);
    const point3 midVertex3 = normalize((v3 + v1) / 2.0f);


    point3 triangle1[4] = {v1, midVertex1, midVertex3};
    point3 triangle2[4] = {midVertex1, v2, midVertex2};
    point3 triangle3[4] = {midVertex3, midVertex2, v3};
    point3 triangle4[4] = {midVertex2, midVertex3, midVertex1};

    subdivideTriangle(triangle1[0], triangle1[1], triangle1[2], points, normals, currentIteration - 1);
    subdivideTriangle(triangle2[0], triangle2[1], triangle2[2], points, normals, currentIteration - 1);
    subdivideTriangle(triangle3[0], triangle3[1], triangle3[2], points, normals, currentIteration - 1);
    subdivideTriangle(triangle4[0], triangle4[1], triangle4[2], points, normals, currentIteration - 1);
}

void computeTriangles(vector<point3>& points, vector<point3>& normals, const int& depth) {

    // A tetrahedron the center of the unit cube
    point3 vertices[4] = { normalize(point3(-1.0, -1.0, -1.0)), normalize(point3(0.0, 1.0, -1.0)),
                           normalize(point3(1.0, -1.0, -1.0)), normalize(point3(0.0, -0.25, 1.0))};

    subdivideTriangle(vertices[0], vertices[1], vertices[2], points, normals, depth);
    subdivideTriangle(vertices[3], vertices[1], vertices[0], points, normals, depth);
    subdivideTriangle(vertices[3], vertices[0], vertices[2], points, normals, depth);
    subdivideTriangle(vertices[3], vertices[2], vertices[1], points, normals, depth);
}

string readFile(const string& fileName);

GLuint initShaders3D() {
    auto vShader = readFile("../shaders/color_and_lightning.vert");
    auto fShader = readFile("../shaders/color_and_lightning.frag");

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

    const GLuint locNormals = glGetAttribLocation(program, "vNormal");
    glEnableVertexAttribArray(locNormals);
    glVertexAttribPointer(locNormals, 3, GL_FLOAT, GL_FALSE, 0
                          , BUFFER_OFFSET(tetraPointCount * sizeof(point3)));

    return program;
}

void initBuffersTriangles3D() {
    vector<point3> points;
    vector<point3> normals;
    computeTriangles(points, normals, recursionDepth);

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
                    , tetraPointCount * sizeof(point3), normals.data());
}

void displayTriangles3D(GLFWwindow* window) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, tetraPointCount);

    // Draw
    glfwSwapBuffers(window);
}

void idleFunc(GLFWwindow* window, double delta, mat4& lightRotation,
        vec4& lightPosition, GLint& lightPositionLocation) {

    // Speed in rads/s
    float angularVelocity = radians(240.0f);
    float rotation = angularVelocity * delta;

    vec3 rotationAxis = vec3();
    rotationAxis[rotationAxisIndex] = 1.0;

    mat4 projection(1.0);
    projection[0][0] = 1.0;

    lightRotation = rotate(mat4(1.0), rotation, rotationAxis) * lightRotation;
    glUniform4fv(lightPositionLocation, 1, value_ptr(lightRotation * lightPosition));

    displayTriangles3D(window);
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
    GLuint programId = initShaders3D();

    glfwSetWindowRefreshCallback(window, &displayTriangles3D);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // // Color and light attributes
    GLint ambientProductLocation = glGetUniformLocation(programId, "ambientProduct");
    GLint diffuseProductLocation = glGetUniformLocation(programId, "diffuseProduct");
    GLint specularProductLocation = glGetUniformLocation(programId, "specularProduct");
    GLint lightPositionLocation = glGetUniformLocation(programId, "lightPosition");
    GLint shininess = glGetUniformLocation(programId, "shininess");

    vec4 ambientLightColor = vec4(0.3, 0.3, 0.3, 1.0);
    vec4 ambientMaterialColor = vec4(0.3, 0.1, 0.1, 1.0);
    vec4 ambientProduct = ambientLightColor * ambientMaterialColor;

    vec4 diffuseLightColor = vec4(0.6, 0.6, 0.6, 1.0);
    vec4 diffuseMaterialColor = vec4(0.8, 0.2, 0.2, 1.0);
    vec4 diffuseProduct = diffuseLightColor * diffuseMaterialColor;

    vec4 specularLightColor = vec4(0.8, 0.8, 0.8, 1.0);
    vec4 specularMaterialColor = vec4(0.7, 0.7, 0.7, 1.0);
    vec4 specularProduct = specularLightColor * specularMaterialColor;

    mat4 lightRotation(1.0);
    vec4 lightPosition = vec4(5.0, 5.0, 5.0, 1.0);

    glUniform4fv(lightPositionLocation, 1, value_ptr(lightPosition));
    glUniform1f(shininess, 85.0);
    glUniform4fv(ambientProductLocation, 1, value_ptr(ambientProduct));
    glUniform4fv(diffuseProductLocation, 1, value_ptr(diffuseProduct));
    glUniform4fv(specularProductLocation, 1, value_ptr(specularProduct));

    auto previous = glfwGetTime();

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        auto now = glfwGetTime();
        auto delta = now - previous;

        if (delta > 0.00833) { // 120 FPS
            previous = now;
            idleFunc(window, delta, lightRotation, lightPosition, lightPositionLocation);
        }

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

