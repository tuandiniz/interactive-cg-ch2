#version 460
in vec4 fColor;
in vec2 fTextCoord;

out vec4 fragColor;

uniform sampler2D textMap;

void main() {
    fragColor = fColor * texture(textMap, fTextCoord);
}