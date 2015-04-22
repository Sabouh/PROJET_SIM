#version 330

out vec4 outBuffer;

uniform sampler2D terrain;

in vec2 coord;
in vec3 normal;
in vec4 fragmentColor;

void main() {
	//outBuffer = texture(terrain,coord);
	outBuffer = vec4(normal,1.0);
	//outBuffer = fragmentColor;
}
