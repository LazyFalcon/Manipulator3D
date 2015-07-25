#ifdef COMPILING_VERTEX_SHADER

layout(location=0)in vec4 mVertex;

out vec2 vUV;

void main(){
	gl_Position = (vec4(mVertex.xy,0,1));
	vUV = (mVertex.xy+vec2(1,1))/2;
}


#endif

#ifdef COMPILING_FRAGMENT_SHADER
out vec4 outAO;

uniform sampler2D uNormalBuffer;
uniform sampler2D uDepthBuffer;
uniform sampler2D uSSAORandom;

uniform mat4 uInvPV;
uniform mat4 uView;

in vec2 vUV;

// const float bias = -0.169;
// const float scale = 50.01;
// const float intensity = 150.1;
const float bias = -0.069;
const float scale = 10.01;
const float intensity = 3.1;
// const vec2 sampleRadius = 95/vec2(1400, 720);
const vec2 sampleRadius = 15/vec2(1400, 720);
const float randomSize = 60;


vec4 getNormal(vec2 uv){
	return texture(uNormalBuffer, uv);
}
vec4 getPosition(vec2 uv, out float depth){
	depth = texture(uDepthBuffer, uv).r;
	vec4 viewSpace = vec4(uv*2-1, depth*2 - 1, 1);
	vec4 worldPos = uInvPV*viewSpace;
	worldPos.xyz /= worldPos.w;
	worldPos.w = 1;

	return worldPos;
}

vec2 getRandom(vec2 uv){
	return normalize(texture(uSSAORandom, uv*randomSize)).xy;
	// return normalize(texture(uSSAORandom, uv*randomSize)).xy*2 - vec2(1);
}
/*
float computeAmbientOcclusion(vec2 uv, vec2 dCoord, vec4 position, vec4 normal){
	float depth;
	vec4 diff = getPosition(uv + dCoord, depth) - position;
	float d = length(diff);
	vec4 v = diff / d;
	// return clamp(max(0.0, dot(normal,v)-bias) * (1.0/(1.0 + d*scale)) * intensity, 0, 1);
	return max(0.0, dot(normal,v)-bias) * (1.0/(1.0 + d*scale)) * intensity;
} */
float computeAmbientOcclusion(vec2 uv, vec2 dCoord, vec4 position, vec4 normal){
	float depth;
	vec4 diff = getPosition(uv + dCoord, depth) - position;
	float d = length(diff);
	vec4 v = diff / d;
	return max(0.0, dot(normal,v)-bias) * (1.0/(1.0 + d*scale)) * intensity;
}

vec2 kernel[16] = vec2[]
(
	// vec2(1,1),
	// vec2(-1,-1),
	// vec2(-1,1),
	// vec2(1,-1)
	// vec2(1,0),
	// vec2(0,-1),
	// vec2(-1,0),
	// vec2(0,1)

	vec2( -0.94201624,  -0.39906216 ),
	vec2(  0.94558609,  -0.76890725 ),
	vec2( -0.094184101, -0.92938870 ),
	vec2(  0.34495938,   0.29387760 ),
	vec2( -0.91588581,   0.45771432 ),
	vec2( -0.81544232,  -0.87912464 ),
	vec2( -0.38277543,   0.27676845 ),
	vec2(  0.97484398,   0.75648379 ),
	vec2(  0.44323325,  -0.97511554 ),
	vec2(  0.53742981,  -0.47373420 ),
	vec2( -0.26496911,  -0.41893023 ),
	vec2(  0.79197514,   0.19090188 ),
	vec2( -0.24188840,   0.99706507 ),
	vec2( -0.81409955,   0.91437590 ),
	vec2(  0.19984126,   0.78641367 ),
	vec2(  0.14383161,  -0.14100790 )
);


void main(void){
	float depth;
	vec4 position = getPosition(vUV, depth);
	vec4 normal = getNormal(vUV);
	// vec2 rand = getRandom(vUV);
	vec2 rand = getRandom(position.xz*vUV);
	// vec2 rand = getRandom(normal.xz);

	float ao = 0.0;
	vec2 radius = max(sampleRadius, sampleRadius/(1 - depth*2.9));

	// float radius = sampleRadius*depth*1.0;
	// float radius = sampleRadius.x;

	const int iterations = 16;
	for(int i=0; i<iterations; ++i){
		// vec2 refl = reflect(kernel[i], rand) * radius;
		vec2 refl = kernel[i] * radius;
		ao += computeAmbientOcclusion(vUV, refl, position, normal);
	}
	ao = clamp(ao/1.0/float(iterations), 0, 2);
	ao = sqrt(ao);
	// outAO = clamp(vec4(1.0-ao/4.0/float(iterations)*28*depth), 0.5, 2);
	// outAO = vec4(1.0-ao/4.0/float(iterations)*28*depth);
	outAO = vec4(1.3-ao);
	// outAO = vec4(ao/4.0/float(iterations));
	// outAO = vec4(1-position.z/20);

}


#endif

