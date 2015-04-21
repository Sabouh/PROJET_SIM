#version 330

uniform sampler3D grille;

in vec3 coord;

out vec4 outBuffer;

void main() {
  outBuffer = texture(grille,coord);
}
