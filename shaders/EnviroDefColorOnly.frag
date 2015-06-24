#version 330

uniform vec4 color;

uniform sampler2D metalTex;
uniform vec4 u_eyePos;

uniform vec4 u_lightPos;
uniform vec4 u_color;
uniform float u_size;
uniform float u_energy;

uniform vec4 u_lightPos2;
uniform vec4 u_color2;
uniform float u_size2;
uniform float u_energy2;

struct PointLamp {
	vec4 position;
	vec4 color;
	float falloffDistance;
	float energy;
};



in vec2 uv;
in vec4 in_normal;
in vec4 in_vertex;

float shade(vec4 n, vec4 l, float e){
	return clamp( dot(l, -n), 0.0, 1.0 ) * e;
}
float spectacular(vec4 n, vec4 l, vec4 e){
	vec4 reflection = normalize(reflect(-l, n));
	float ndoth = max(0.0, dot(reflection, e));
	return pow(ndoth, 128);
}
float attenuation(float d, float s){
	// return 1.0 / (1.0 + 0.1*d + 0.01*d*d);
	return 1.1-(clamp(d, 0.0, s)/s);
}

// vec4 fullLightComputation(){}

void main(void){
	float metal = texture(metalTex, uv*2).r;
	vec4 N = normalize(in_normal);

	vec4 E = in_vertex - u_eyePos;
	float e = length(E);
	E /= e;
	vec4 L = in_vertex - u_lightPos;
	float d = length(L);
	L /= d;

	vec4 l_shade = shade(N,L, u_energy)*attenuation(d, u_size)*u_color;
	vec4 l_spect = spectacular(N, L, E)*metal*u_color*attenuation(e, 20.0);

	L = in_vertex - u_lightPos2;
	d = length(L);
	L /= d;

	l_shade += shade(N,L, u_energy2)*attenuation(d, u_size2)*u_color2;
	l_spect += spectacular(N, L, E)*metal*u_color2*attenuation(e, 20.0);

	gl_FragData[0] = color*l_shade + l_spect;
	gl_FragData[1] = vec4(N.xyz, metal);
}
