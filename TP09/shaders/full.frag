#version 330

uniform vec3 light;

out vec4 outBuffer;

uniform sampler2D terrain;
uniform sampler2D montagneTex;
uniform sampler2D shadowmap;

uniform mat4 mdvMat;
uniform mat4 projMat;
uniform mat4 mvpMat;

in vec2 coord;
in vec2 uvcoord;
in vec3 normal;
in vec3 normalView;
in vec3 tangentView;
in vec3 eyeView;
in vec4 posLight;
in vec4 fragmentColor;

// simple normal mapping 
vec3 getModifiedNormal() {
  vec3 n   = normalize(normalView);
  vec3 t   = normalize(tangentView);
  vec3 b   = normalize(cross(n,t));
  mat3 tbn = mat3(t,b,n);
  vec3 tn  = normalize(normal*2.0-vec3(1.0));
  
  return normalize(tbn*tn);
}

void main() {
	vec3 n = getModifiedNormal();
	vec3 e = normalize(eyeView);
	vec3 l = normalize(light);
	vec4 c = texture(montagneTex,coord);
	
	float diff = max(dot(l,n),0.0);
	float spec = pow(max(dot(reflect(l,n),e),0.0),20.0);
	
	vec4 color = c*(diff+spec);
	
	float v = 1.0;
	//float v = texture(shadowmap, vec3(posLight.xy,(posLight.z-0.005)/posLight.w)).x;
	
	if( texture(shadowmap,posLight.xy).z < posLight.z-0.00001 ){ 
		v = 0.05;
	}
		
	//outBuffer = fragmentColor*texture(montagneTex,coord);
	outBuffer  = color*v;
}
