#version 330

out vec4 out_ao;

uniform sampler2D normalTex;
uniform sampler2D depthTex;
uniform sampler2D SSAORandom;


uniform mat4 invPV;
uniform mat4 View;

in vec2 texCoords;

const float bias = 0.569;
// const float scale = 1.1;
const float scale = 145.01;
// const float scale = 0.1;
// const float intensity = 2000.1;
const float intensity = 5.1;
// const float sampleRadius = 0.01;
const vec2 sampleRadius = 55/vec2(1400, 720);
const float randomSize = 644;


vec4 getNormal(vec2 uv){
	// return View * vec4(texture(normalTex, uv).xyz, 0);
	return texture(normalTex, uv);
}
/*
vec4 getPosition(vec2 uv){
	vec3 viewSpace = vec3(uv*2-vec2(1), 0);
	viewSpace.z = (texture(depthTex, uv).r*2 - 1);
	// viewSpace.z = texture(depthTex, uv).r;
	vec4 worldPos = invPV*vec4(viewSpace, 1);
	worldPos.xyz /= worldPos.w;
	worldPos.w = 1;
	// vec4 worldPos = vec4(viewSpace, 1);

	return worldPos;
} */
vec4 getPosition(vec2 uv, out float depth){
	depth = texture(depthTex, uv).r;
	// vec3 viewSpace = vec3(uv*2-vec2(1), 0);
	vec4 viewSpace = vec4(uv*2-1, depth*2 - 1, 1);
	vec4 worldPos = invPV*viewSpace;
	worldPos.xyz /= worldPos.w;
	worldPos.w = 1;
	// vec4 worldPos = vec4(viewSpace, 1);

	return worldPos;
}

vec2 getRandom(vec2 uv){
	// return normalize(texture(SSAORandom, uv*randomSize)).xy;
	return normalize(texture(SSAORandom, uv*randomSize)).xy*2 - vec2(1);
}

float computeAmbientOcclusion(vec2 uv, vec2 dCoord, vec4 position, vec4 normal){
	float depth;
	vec4 diff = getPosition(uv + dCoord, depth) - position;
	float d = length(diff);
	vec4 v = diff / d;
	// return clamp(max(0.0, dot(normal,v)-bias) * (1.0/(1.0 + d*scale)) * intensity, 0, 1);
	return max(0.0, dot(normal,v)-bias) * (1.0/(1.0 + d*scale)) * intensity;
}

vec2 kernel[4] = vec2[]
(
	// vec2(1,0),
	// vec2(-1,0),
	// vec2(0,1),
	// vec2(0,-1)
	vec2(1,1),
	vec2(-1,1),
	vec2(-1,-1),
	vec2(1,-1)
);


void main(void){
	float depth;
	vec4 position = getPosition(texCoords, depth);
	vec4 normal = getNormal(texCoords);
	// vec2 rand = getRandom(texCoords);
	vec2 rand = getRandom(position.xz*texCoords);
	// vec2 rand = getRandom(normal.xz);

	float ao = 0.0;
	// float radius = sampleRadius/(1 - depth*1.9);
	float radius = sampleRadius*depth*1.0;
	// float radius = sampleRadius;

	const int iterations = 4;
	for(int i=0; i<iterations; ++i){
		vec2 refl = reflect(kernel[i], rand) * radius;
		vec2 coord = vec2(refl.x*0.707 - refl.y*0.707, refl.x*0.707 + refl.y*0.707);

		ao += computeAmbientOcclusion(texCoords, refl*0.25, position, normal);
		ao += computeAmbientOcclusion(texCoords, coord*0.5, position, normal);
		ao += computeAmbientOcclusion(texCoords, refl*0.75, position, normal);
		ao += computeAmbientOcclusion(texCoords, coord, position, normal);
	}

	// out_ao = clamp(vec4(1.0-ao/4.0/float(iterations)*28*depth), 0.5, 2);
	// out_ao = vec4(1.0-ao/4.0/float(iterations)*28*depth);
	out_ao = vec4(1.1-ao/4.0/float(iterations)*28);
	// out_ao = vec4(ao/4.0/float(iterations));
	// out_ao = vec4(1-position.z/20);

}

