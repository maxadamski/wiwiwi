#version 330 core

in vec2 TexCoord;
out vec4 color;

uniform sampler2D DiffuseMap0;

void main() {
    color = texture(DiffuseMap0, TexCoord);
}
