#version 330

uniform mat3      invModel;
uniform vec3      vPos;
uniform sampler2D tex;

out vec4 fragment;

void main() {
    fragment = vec4(1.0, 0.0, 0.0, 1.0);
}