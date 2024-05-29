in vec4 vPosition;
in vec4 vColor;

void main() {
    gl_FrontColor = vColor;
    gl_Position = vPosition;
}
