#version 460
in vec4 vPosition;
in vec4 vNormal;
in vec2 vTextCoord;

out vec3 reflection;

uniform mat4 modelView;
uniform mat4 projection;

void main() {
    gl_Position = projection * modelView * vPosition;
    vec3 eyePos = normalize(modelView * vPosition).xyz;
    vec3 N = normalize((modelView * vNormal).xyz);
    reflection = reflect(eyePos, N);
}
