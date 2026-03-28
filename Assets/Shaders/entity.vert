#version 330

in vec3 inVertex;
in vec3 inTexCoord;

uniform mat4 view;
uniform mat4 proj;
uniform mat4 model;

out vec3 passColor;
out vec3 passTexCoord;

void main() {
    gl_Position  = proj * view * model * vec4(inVertex, 1.0);
    passColor    = vec3(1.0, 0.0, 0.0);
    passTexCoord = inTexCoord;
}