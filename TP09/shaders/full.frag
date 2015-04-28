#version 330

out vec4 outBuffer;

uniform vec3 light;

uniform sampler2D terrain;
//uniform sampler2D shadowMap;
uniform sampler2D eauTex;
uniform sampler2D foretTex;
uniform sampler2D rocheTex;
uniform sampler2D neigeTex;

in vec2 coord;
in vec3 normal;
in vec4 fragmentColor;
in vec3 p;
in  vec3 normalView;
in  vec3 eyeView;

void main() {


	 if(p.z > 0.79){
	   outBuffer = fragmentColor*texture(eauTex,coord);
         }
	 else if(p.z > 0.6){
	   outBuffer = fragmentColor*texture(rocheTex,coord);
         }
         else if(p.z > 0.25){
           outBuffer = fragmentColor*texture(foretTex,coord);
         }
         else if(p.z > 0.1){
     	   outBuffer = fragmentColor*texture(rocheTex,coord);
         }
         else{
           outBuffer = fragmentColor*texture(neigeTex,coord);
         }
	
}
