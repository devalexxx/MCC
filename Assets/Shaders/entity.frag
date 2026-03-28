#version 330
#extension GL_ARB_texture_query_lod : enable

in vec3 passColor;
in vec3 passTexCoord;

uniform mat3      invModel;
uniform vec3      vPos;
uniform sampler2D tex;

out vec4 fragment;

void main() {
    float mipmapLevel = 0;
#ifdef GL_ARB_texture_query_lod
    mipmapLevel = textureQueryLOD(tex, passTexCoord.xy).x;
#endif
    vec4 pixel = textureLod(tex, passTexCoord.xy, mipmapLevel);
    fragment   = vec4(passColor, 1.0) * pixel;
}