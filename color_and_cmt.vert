#version 460
in vec4 vPosition;
in vec4 vColor;
out vec4 fColor;
uniform mat4 transform;

void main() {
    gl_Position = transform * vPosition;
    fColor = vColor;
}
