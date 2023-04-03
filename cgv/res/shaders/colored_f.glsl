#version 330 core

out vec4 pixelColor;

in vec4 i_c;

void main(void) {
	pixelColor=i_c;
}
