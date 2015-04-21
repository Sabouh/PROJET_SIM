#version 330

out vec4 outBuffer;

uniform sampler2D terrain;

in vec2 coord;

void main() {
  outBuffer = texture(terrain,coord);
}
