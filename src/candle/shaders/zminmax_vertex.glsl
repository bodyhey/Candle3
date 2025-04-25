#version 130

layout(location = 0) in vec3 inPosition;
uniform mat4 viewMatrix;

out float zCamera;

void main() {
    vec4 transformed = viewMatrix * vec4(inPosition, 1.0);
    zCamera = transformed.z;
}
