#version 330

// input attributes
layout(location = 0) in vec3 position;

uniform mat4 mvpMat; //matrice projection depuis lumière
uniform mat4 mvpDepthMat; // mvp depth matrix
uniform mat4 mdvMat;      // modelview matrix 
uniform mat4 projMat;     // projection matrix
uniform mat3 normalMat;   // normal matrix

uniform sampler2D terrain;

uniform vec3 light;

out vec2 coord;
out vec3 normalView;
out vec3 tangentView;
out vec2 uvcoord;
out vec3 eyeView;
out vec3 normal;
out vec4 fragmentColor;
out vec4 posLight;
out vec3 p;

vec3 calculNormal(){
	float alpha = 100;
	
	float ps = 1./512;
	float hh = texture(terrain,coord+vec2(0,ps)).x;
	float hb = texture(terrain,coord-vec2(0,ps)).x;
	float hd = texture(terrain,coord+vec2(ps,0)).x;
	float hg = texture(terrain,coord-vec2(ps,0)).x;
	
	float gx = (hd-hg)*alpha;
	float gy = (hh-hb)*alpha;
	
	vec3 v1 = vec3(1,0,gx);
	vec3 v2 = vec3(0,1,gy);
	
	return normalize(cross(v1,v2));
}

void main() {

  p = position;
  coord = position.xy*0.5+0.5;
  p.z = texture(terrain,coord).x; 
  normal = calculNormal();
  
  normalView  = normalize(normalMat*normal);
  tangentView = vec3(1.0,0,0)* normalMat;
  eyeView     = normalize((mdvMat*vec4(p,1.0)).xyz);
  uvcoord     = coord*5.0;

  gl_Position = projMat*mdvMat*vec4(p,1.0);

  posLight = (mvpDepthMat * vec4(p,1.0)) * 0.5 + 0.5;
  
  fragmentColor = vec4(dot(normal,light));

  eyeView = normalize((mdvMat*vec4(p,1.0)).xyz); 
  normalView  = normalize(normalMat*normal);

}



