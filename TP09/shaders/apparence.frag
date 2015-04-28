#version 330

out vec4 outBuffer;

uniform sampler2D eauTex;
uniform sampler2D foretTex;
uniform sampler2D rocheTex;
uniform sampler2D neigeTex;



uniform sampler2D terrain;

in vec2 coord;
in vec3 p;

void main() {
     
     if(p.z > 0.75){
	outBuffer = texture(eauTex,coord);
     }
     else if(p.z > 0.25){
	outBuffer = texture(foretTex,coord);
     }
     else if(p.z > 0.1){
     	outBuffer = texture(rocheTex,coord);
     }
     else{
        outBuffer = texture(neigeTex,coord);
     }
     
}
