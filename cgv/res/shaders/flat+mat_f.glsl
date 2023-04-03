#version 330 core

struct Material {
    vec3 ambient, diffuse, specular;
    float shine;
};

in vec2 TexCoord;
out vec4 color;

uniform Material mat;

void main() {
    color = vec4(mat.diffuse, 1.0);
}
