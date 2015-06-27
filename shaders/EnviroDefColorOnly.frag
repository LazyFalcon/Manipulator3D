#version 330

uniform vec4 color;

uniform sampler2D metalTex;
uniform sampler2D shadowTex;
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
in vec4 in_vertexInShadow;

float shade(vec4 n, vec4 l, float e){
	// return clamp( dot(l, -n), 0.0, 1.0 ) * e;
	return clamp( dot(l, -n), 0.7, 1.0 ) * e;
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
vec2 kernel[5] = vec2[]
(
	vec2(0.0, 0.0), vec2(1.0, 1.0), vec2(-1.0, 1.0), vec2(-1.0, -1.0), vec2(1.0, -1.0)
);

const float bias = 0.0003;
void shadowSample(in vec3 coords, in vec2 offset, inout float factor){
	factor += float( texture(shadowTex, vec2(coords.xy + offset)).r*2 - 1 > coords.z-bias);
}


void main(void){
	// float metal = 1;
	float metal = texture(metalTex, uv*3).r;
	vec4 vertexInShadow = in_vertexInShadow;
	vertexInShadow.xyz /= vertexInShadow.w;
	vertexInShadow.xy = vertexInShadow.xy * 0.5 + vec2(0.5);// + vec2(0.5/512);
	// vec2 shadowTexCoords = in_vertexInShadow.xy/in_vertexInShadow.w * 0.5 + vec2(0.5);// + vec2(0.5/512);
	// float depthFromTex = texture(shadowTex, shadowTexCoords).r;
	// depthFromTex = depthFromTex*2 - 1;
	float lightDepth = in_vertexInShadow.z/in_vertexInShadow.w;
	lightDepth -= bias;

	vec4 N = normalize(in_normal);
	vec4 l_shade = vec4(0.2);
	vec4 l_spect = vec4(0);
	vec4 L = in_vertex - u_lightPos;
	float d = length(L);
	L /= d;

	float shadowFactor = 0.0;
	float PCFRadius = 1;
	float shadowMapStep = 0.5/1024;

	for(int i=0; i<5; i++){
		shadowSample(
			vertexInShadow.xyz,
			kernel[i] * shadowMapStep * PCFRadius,
			shadowFactor
		);
	}
	shadowFactor /= 5;


	float shadowIntensity = 1.0;
	// if(lightDepth < depthFromTex && dot(-N,L) > 0){
	// if(shadowFactor > 0.0 && dot(-N,L) > 0){
	if(shadowFactor > 0.0){
		// shadowIntensity = shadowFactor;
		shadowIntensity = 1.0;

		// N *= metal;
		vec4 E = in_vertex - u_eyePos;
		float e = length(E);
		E /= e;

		// vec4 l_shade = vec4(1);
		// vec4 l_spect = vec4(0);
		l_shade = shade(N,L, u_energy)*attenuation(d, u_size)*u_color*shadowIntensity;
		l_spect = spectacular(N, L, E)*metal*u_color*attenuation(e, 20.0);

		L = in_vertex - u_lightPos2;
		d = length(L);
		L /= d;

		l_shade += shade(N,L, u_energy2)*attenuation(d, u_size2)*u_color2*shadowIntensity;
		l_spect += spectacular(N, L, E)*metal*u_color2*attenuation(e, 20.0);
	}
	gl_FragData[0] = color*l_shade + l_spect;
	// gl_FragData[0] = shadowFactor;
	gl_FragData[1] = vec4(N.xyz, 1.0);
}
