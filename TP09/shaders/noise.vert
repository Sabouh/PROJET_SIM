#version 330

// input attributes 
layout(location = 0) in vec3 position; 

void main() {
  // no need for any particular transformation (Identity matrices)
  gl_Position = vec4(position,1);
}
