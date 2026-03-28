#version 330

in vec3 inVertex;
in vec3 inColor;
in vec3 inTexCoord;
in vec3 inNormal;

uniform mat4 view;
uniform mat4 proj;
uniform mat4 model;

out vec3 passColor;
out vec3 passPos;
out vec3 passTexCoord;
out vec3 passNormal;

void main() {
    gl_Position  = proj * view * model * vec4(inVertex, 1.0);
    passColor    = inColor;
    passPos      = vec3(model * vec4(inVertex, 1.0));
    passNormal   = inNormal;
    passTexCoord = inTexCoord;
}