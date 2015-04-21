#version 330

// input attributes
layout(location = 0) in vec3 position;
uniform sampler2D terrain;

out vec2 coord;

void main() {
  gl_Position = vec4(position,1.0);
  vec2 pixelSize = 1./vec2(textureSize(terrain,0))
  //
  coord = vec2(position.x,position.y)*0.5+vec2(0.5);
  vec3 hauteur = texture(terrain,coord).xyz;
  float gx = ;
  float gy =;
}

