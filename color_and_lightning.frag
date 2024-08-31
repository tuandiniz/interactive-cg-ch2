#version 460
in vec4 fNormal;
in vec4 fViewer;
in vec4 fLight;

uniform vec4 ambientProduct;
uniform vec4 diffuseProduct;
uniform vec4 specularProduct;
uniform float shininess;
uniform vec4 lightPosition;

out vec4 fragColor;

void main() {

    vec4 normHalfway = normalize(fLight + fViewer);
    vec4 normNormal = normalize(fNormal);
    vec4 normLight = normalize(fLight);

    fragColor = ambientProduct
        + max(dot(normNormal, normLight), 0.0) * diffuseProduct
        + max(pow(max(dot(normNormal, normHalfway), 0.0), shininess), 0.0) * specularProduct;
}