#version 330

// input attributes
layout(location = 0) in vec3 position;
out vec2 coord;

void main() {
  gl_Position = vec4(position,1.0);
  coord = position.xy*0.5+vec2(0.5);
}

