#version 330

out vec4 outBuffer;

uniform sampler2D terrain;
//uniform sampler2D shadowMap;
uniform sampler2D montagneTex;

in vec2 coord;
in vec3 normal;
in vec4 fragmentColor;

void main() {
	outBuffer = fragmentColor*texture(montagneTex,coord);
}
