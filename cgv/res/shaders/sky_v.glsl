#version 330 core

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

uniform mat4 PVM;

out vec2 TexCoord;

void main() {
    TexCoord = texCoord;
    gl_Position = PVM * vec4(vertex, 1.0);
}
