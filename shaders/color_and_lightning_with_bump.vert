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
    gl_Position = vPosition;
    fNormal = vec4(vNormal.x, vNormal.y, vNormal.z, 0.0);

    float rotAngle = radians(40.0f);
    mat4 rotation = mat4(1.0f);
    float sinTheta = sin(rotAngle);
    float cosTheta = cos(rotAngle);
    rotation[1][1] = cosTheta;
    rotation[2][1] = -sinTheta;
    rotation[1][2] = sinTheta;
    rotation[2][2] = cosTheta;

    vec4 rotatedPosition = rotation * vPosition;
    vec4 rotatedNormal = rotation * fNormal;
    if (pow(rotatedPosition.x, 2) + pow(rotatedPosition.y, 2) <= 0.1) {

        fNormal.x = -rotatedNormal.x;
        fNormal.y = -rotatedNormal.y;
        fNormal = transpose(rotation) * fNormal;
    }

    if (pow(vPosition.x, 2) + pow(vPosition.y, 2) <= 0.1) {

        fNormal.x = fNormal.x + vPosition.x;
        fNormal.y = fNormal.y + vPosition.y;
        fNormal = normalize(fNormal);
    }

    fViewer = vec4(0.0, 0.0, 1.0, 0.0);
    fLight = normalize(lightPosition - vPosition);
}
