in vec4 vPosition;

void main() {
    gl_FrontColor = vec4((vPosition + 1)/2.0);
    gl_Position = vPosition;
}
