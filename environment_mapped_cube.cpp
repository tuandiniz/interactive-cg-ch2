#include <iostream>

#define GLM_SWIZZLE
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
using namespace cimg_library;

typedef vec4 point4;
typedef vec4 color4;

int vertexCount = 0;
int rotationAxisIndex = 0;
bool isPerspective = false;

void populateBuffers(vector<point4>& pointsArray, vector<vec3>& normalArray, vector<vec2>& textArray) {
    GLuint abuffer;
    glGenVertexArrays(1, &abuffer);
    glBindVertexArray(abuffer);

    vertexCount = pointsArray.size();

    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);

    int bufferSize = vertexCount * ( sizeof(point4) * 2 + sizeof(vec2) );
    glBufferData(GL_ARRAY_BUFFER, bufferSize, nullptr, GL_STATIC_DRAW);
    int offset = 0;
    glBufferSubData(GL_ARRAY_BUFFER, offset, vertexCount * sizeof(point4), pointsArray.data());
    offset += vertexCount * sizeof(point4);
    glBufferSubData(GL_ARRAY_BUFFER, offset, vertexCount * sizeof(vec3), normalArray.data());
    offset += vertexCount * sizeof(vec3);
    glBufferSubData(GL_ARRAY_BUFFER, offset, vertexCount * sizeof(vec2), textArray.data());
}

void quad(const point4 vertexList[], vector<point4>& pointsArray,
                vector<vec3>& normalArray, vector<vec2>& textArray,
                const int a, const int b, const int c, const int d) {

    vec3 normal = cross(vec3(vertexList[b].xyz() - vertexList[a].xyz()),
        vec3(vertexList[c].xyz() - vertexList[b].xyz()));
    normal = normalize(normal);

    // Triangle strip
    pointsArray.push_back(vertexList[a]);
    normalArray.push_back(normal);
    textArray.emplace_back(0.0, 0.0);

    pointsArray.push_back(vertexList[b]);
    normalArray.push_back(normal);
    textArray.emplace_back(1.0, 0.0);

    pointsArray.push_back(vertexList[c]);
    normalArray.push_back(normal);
    textArray.emplace_back(1.0, 1.0);

    pointsArray.push_back(vertexList[c]);
    normalArray.push_back(normal);
    textArray.emplace_back(1.0, 1.0);

    pointsArray.push_back(vertexList[d]);
    normalArray.push_back(normal);
    textArray.emplace_back(0.0, 1.0);

    pointsArray.push_back(vertexList[a]);
    normalArray.push_back(normal);
    textArray.emplace_back(0.0, 0.0);
}

void generateColorCube() {

    const point4 vertexList[8] = {
        point4(-1.0, -1.0, 1.0, 1.0), point4(-1.0, -1.0, -1.0, 1.0),
        point4(-1.0, 1.0, -1.0, 1.0),point4(-1.0, 1.0, 1.0, 1.0),
        point4(1.0, -1.0, 1.0, 1.0), point4(1.0, -1.0, -1.0, 1.0),
        point4(1.0, 1.0, -1.0, 1.0),point4(1.0, 1.0, 1.0, 1.0)
    };

    vector<point4> pointsArray;
    vector<vec3> normalArray;
    vector<vec2> textArray;

    quad(vertexList, pointsArray, normalArray, textArray, 0, 3, 2, 1);
    quad(vertexList, pointsArray, normalArray, textArray, 2, 3, 7, 6);
    quad(vertexList, pointsArray, normalArray, textArray, 5, 6, 7, 4);
    quad(vertexList, pointsArray, normalArray, textArray, 0, 1, 5, 4);
    quad(vertexList, pointsArray, normalArray, textArray, 1, 2, 6, 5);
    quad(vertexList, pointsArray, normalArray, textArray, 3, 0, 4, 7);

    populateBuffers(pointsArray, normalArray, textArray);
}

void imgToArray(CImg<unsigned char> img, GLubyte(* img_arr)[512][3]) {
    for (int i = 0; i < img.width(); i++) {
        for(int j = 0; j < img.height(); j++) {
            for(int k = 0; k < img.spectrum(); k++) {
                img_arr[i][j][k] = img(i, j, 0, k);
            }
        }
    }
}

void initCubeTextures(GLuint program, const string& frontFile,
                      const string& backFile,
                      const string& leftFile,
                      const string& rightFile,
                      const string& topFile,
                      const string& bottomFile) {

    GLubyte front[512][512][3],
            back[512][512][3],
            top[512][512][3],
            bottom[512][512][3],
            left[512][512][3],
            right[512][512][3];
    imgToArray(readImage(frontFile), front);
    imgToArray(readImage(backFile), back);
    imgToArray(readImage(leftFile), left);
    imgToArray(readImage(rightFile), right);
    imgToArray(readImage(topFile), top);
    imgToArray(readImage(bottomFile), bottom);

    GLuint textures[1];
    glGenTextures(1, textures);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textures[0]);

    glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, 512, 512, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, left);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, 512, 512, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, right);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, 512, 512, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, top);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, 512, 512, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, bottom);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, 512, 512, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, front);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, 512, 512, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, back);

    GLuint textLoc;
    textLoc = glGetUniformLocation(program, "textMap");

    glUniform1i(textLoc, 0);
}

GLuint initShadersColorCube() {
    auto vShader = readFile("../shaders/env_cube_map.vert");
    auto fShader = readFile("../shaders/env_cube_map.frag");

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

    int offset = 0;
    const GLuint loc = glGetAttribLocation(program, "vPosition");
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(offset));

    offset += vertexCount * sizeof(point4);
    const GLuint locNormals = glGetAttribLocation(program, "vNormal");
    glEnableVertexAttribArray(locNormals);
    glVertexAttribPointer(locNormals, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(offset));

    offset += vertexCount * sizeof(vec3);
    const GLuint locTextures = glGetAttribLocation(program, "vTextCoord");
    glEnableVertexAttribArray(locTextures);
    glVertexAttribPointer(locTextures, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(offset));

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
    float angularVelocity = radians(50.0f);
    float rotation = angularVelocity * delta;

    vec3 rotationAxis = vec3();
    rotationAxis[rotationAxisIndex] = 1.0;

    mat4 projection(1.0);
    projection[0][0] = 1.0;

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
    GLFWwindow* window = glfwCreateWindow(1200, 1200, "Hello World", nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    generateColorCube();
    auto programId = initShadersColorCube();
    initCubeTextures(programId, "../textures/front.jpg",
        "../textures/back.jpg",
        "../textures/left.jpg",
        "../textures/right.jpg",
        "../textures/top.jpg",
        "../textures/bottom.jpg");

    glfwSetWindowRefreshCallback(window, &displayColorCube);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetKeyCallback(window, keyCallback);

    glClearColor(1.0, 1.0, 1.0, 1.0);
    glfwSwapInterval(1);

    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
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
            //auto translate = glm::translate(mat4(1.0), vec3(0.0, 0.0, -2.8));
            glUniformMatrix4fv(modelViewLoc, 1, GL_FALSE, value_ptr(transform));
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