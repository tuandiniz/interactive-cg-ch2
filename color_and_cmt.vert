#version 460
in vec4 vPosition;
in vec4 vColor;
out vec4 fColor;
uniform mat4 modelView;
uniform mat4 projection;

void main() {
    gl_Position = projection * modelView * vPosition;
    gl_Position /= gl_Position.w;
    fColor = vColor;
}
