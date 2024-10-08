#version 460
in vec4 vPosition;
in vec4 vColor;
in vec2 vTextCoord;

out vec4 fColor;
out vec2 fTextCoord;

uniform mat4 modelView;
uniform mat4 projection;

void main() {
    gl_Position = projection * modelView * vPosition;
    //gl_Position /= gl_Position.w;
    fColor = vColor;
    fTextCoord = vTextCoord;
    //vec4 projected = gl_Position / gl_Position.w;
    //fTextCoord = vec2((projected.x + 1)/2, (projected.y + 1)/2);
}
