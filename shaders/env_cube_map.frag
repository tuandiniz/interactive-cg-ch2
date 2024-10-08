#version 460
#extension GL_NV_shadow_samplers_cube : enable
in vec3 reflection;

out vec4 fragColor;

uniform samplerCube textMap;

void main() {
    fragColor = texture(textMap, reflection);
}