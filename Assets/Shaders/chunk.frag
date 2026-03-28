#version 330
#extension GL_ARB_texture_query_lod : enable

in vec3 passColor;
in vec3 passPos;
in vec3 passTexCoord;
in vec3 passNormal;

uniform mat3 invModel;
uniform vec3 vPos;

uniform sampler2DArray tex;

out vec4 fragment;

vec3 lightPos   = vec3(0, 100, 0);
vec3 lightColor = vec3(1, 1, 1);

float ambientStrength  = 0.1;
float specularStrength = 0.5;

void main() {
    vec3 normal = invModel * passNormal;
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - passPos);

    // Ambient lighting
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse lighting
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular lighting
    vec3 viewDir = normalize(vPos - passPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    float mipmapLevel = 0;
#ifdef GL_ARB_texture_query_lod
    mipmapLevel = textureQueryLOD(tex, passTexCoord.xy).x;
#endif
    vec4 pixel = textureLod(tex, passTexCoord, mipmapLevel);
    fragment = vec4(ambient + diffuse + specular, 1.f) * pixel;
}