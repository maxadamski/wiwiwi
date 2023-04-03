#version 330 core

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;

void main() {
    TexCoord = texCoord;
    //Normal = mat3(transpose(inverse(M))) * normal;
    Normal = normal;
    FragPos = vec3(M * vec4(vertex, 1.0));
    gl_Position = P*V*M * vec4(vertex, 1.0);
}
