#version 460
in vec4 vPosition;
in vec4 vNormal;

out vec4 fNormal;
out vec4 fViewer;
out vec4 fLight;
out vec4 fHalfway;

uniform vec4 lightPosition;
uniform mat4 modelView;
uniform mat4 projection;

void main() {
    //gl_Position = projection * modelView * vPosition;
    gl_Position = vPosition;
    fNormal = vec4(vNormal.x, vNormal.y, vNormal.z, 0.0);
    fViewer = vec4(0.0, 0.0, 1.0, 0.0);
    fLight = normalize(lightPosition - vPosition);
}
