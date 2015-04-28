#version 330

out vec4 outBuffer;

uniform vec3 light;

uniform sampler2D terrain;
uniform sampler2D shadowmap;
uniform sampler2D eauTex;
uniform sampler2D foretTex;
uniform sampler2D rocheTex;
uniform sampler2D neigeTex;

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
in vec3 p;

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
	vec4 c;
	if(p.z > 0.79){
		c = texture(eauTex,coord);
	}
	else if(p.z > 0.6){
		c = texture(rocheTex,coord);
	}
	else if(p.z > 0.25){
		c = texture(foretTex,coord);
	}
	else if(p.z >0.1){
		c = texture(rocheTex,coord);
	}
	else{
		c = texture(neigeTex,coord);
	}
	
	float diff = max(dot(l,n),0.0);
	float spec = pow(max(dot(reflect(l,n),e),0.0),20.0);
	
	vec4 color = c*(diff+spec);
	
	//float v = texture(shadowmap, vec3(posLight.xy,(posLight.z-0.005)/posLight.w)).x;
	float v = 1.0;
	if( texture(shadowmap,posLight.xy).z < posLight.z-0.00001 ){ 
		v = 0.05;
	}
		
	outBuffer  = color*v;
}
