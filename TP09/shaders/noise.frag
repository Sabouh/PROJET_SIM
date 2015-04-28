#version 330

uniform vec3 motion;
out vec4 outBuffer;

// perlin noise ( http://freespace.virgin.net/hugo.elias/models/m_perlin.htm )
float noiseFunc(float x, float y) {
  int n = int(x * 40.0 + y * 6400.0);
  n = (n << 13) ^ n;
  return 1.0 - float( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0;
}

float cosInterp(float a, float b, float x) {
  float ft= x*3.1415927;
  float f = (1.0-cos(ft))*0.5;

  return mix(a,b,f);
}

float smoothNoise(float x, float y) {
  float corners = noiseFunc(x-1.0,y-1.0)+noiseFunc(x+1.0,y-1.0)+noiseFunc(x-1.0,y+1.0)+noiseFunc(x+1.0,y+1.0);
  float sides   = noiseFunc(x-1.0,y)+noiseFunc(x+1.0,y)+noiseFunc(x,y-1.0)+noiseFunc(x,y+1.0);
  float center  = noiseFunc(x,y);

  return corners/16.0 + sides/8.0 + center/4.0;
}

float interpNoise(float x, float y) {
  float frx = fract(x);
  float fry = fract(y);

  float ix = floor(x);
  float iy = floor(y);

  float v1 = smoothNoise(ix    ,iy    );
  float v2 = smoothNoise(ix+1.0,iy    );
  float v3 = smoothNoise(ix    ,iy+1.0);
  float v4 = smoothNoise(ix+1.0,iy+1.0);

  float i1 = cosInterp(v1,v2,frx);
  float i2 = cosInterp(v3,v4,frx);

  return cosInterp(i1,i2,fry);
}

float perlinNoise(float x, float y) {
  const int n = 20;

  float total = 0.0;
  float p = 0.5;
  
  for(int i=0;i<n;++i) {
    float freq = pow(2.0,float(i));
    float amp = pow(p,float(i))*(float(n)-float(i))/(n/2);
 
    total += interpNoise(x*freq,y*freq)*amp;
  }  

  return total;
}

void main() {
  const float s = 100.0;
  float x = gl_FragCoord.x/s+motion.x;
  float y = gl_FragCoord.y/s+motion.y;
  float p = perlinNoise(x,y)+motion.z;

  if(p < -0.81){
    p = -0.81;
  }

  outBuffer = vec4(p*0.5+0.5);
}
