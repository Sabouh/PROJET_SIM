#version 330

// input attributes
layout(location = 0) in vec3 position;

uniform mat4 mdvMat;
uniform mat4 projMat;
uniform sampler2D terrain;

out vec2 coord;
out vec3 p;

void main() {
  p = position;
  coord = position.xy*0.5+0.5;
  p.z = texture(terrain,coord).x; 
  gl_Position = projMat*mdvMat*vec4(p,1.0);
}

