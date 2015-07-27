#ifdef COMPILING_VERTEX_SHADER

layout(location=0)in vec4 mVertex;

out vec2 vUV;

void main(){
	gl_Position = (vec4(mVertex.xy,0,1));
	vUV = (mVertex.xy+vec2(1,1))/2;
}


#endif

/// TODO: dodaæ zanikanie wartoœci wraz z odleg³oœci¹, mo¿na wtedy podbiæ wartoœci

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
// const float bias = -0.0069;
const float bias = 0.12069;
const float scale = 40.01;
// const float intensity = 3.1;
const float intensity = 25.1;
const vec2 sampleRadius = 175/vec2(1400, 720);
// const vec2 sampleRadius = 30/vec2(1400, 720);
const float randomSize = 60;


vec4 getNormal(vec2 uv){
	return texture(uNormalBuffer, uv);
}
vec4 getPosition(vec2 uv, out float depth){
	depth = texture(uDepthBuffer, uv).r;
	// vec4 viewSpace = vec4(uv*2-1, depth*2 - 1, 1);
	vec4 viewSpace = vec4(uv*2-1, depth, 1);
	vec4 worldPos = uInvPV*viewSpace;
	worldPos.xyz /= worldPos.w;
	worldPos.w = 1;

	return worldPos;
}

vec2 getRandom(vec2 uv){
	return normalize(texture(uSSAORandom, uv*randomSize)).xy;
}

float computeAmbientOcclusion(vec2 uv, vec2 dCoord, vec4 position, vec4 normal){
	float depth;
	// vec4 diff = getPosition(uv + dCoord, depth) - position;
	vec4 diff = getPosition(uv + dCoord, depth) - position;
	float d = length(diff);
	vec4 v = diff / d;
	return sqrt(max(0.0, dot(normal,v)-bias)) * (1.0/(1.0 + d*scale)) * intensity;
}

const vec2 kernel[32] = vec2[]
(
	vec2(  0.5244396,  0.6552907 ),
	vec2(  0.7802591,  0.1212579 ),
	vec2( -0.1709561,  0.9723814 ),
	vec2(  0.08371004, 0.3698372 ),
	vec2(  0.1981699, -0.0234299 ),
	vec2( -0.2241783, -0.4910517 ),
	vec2(  0.615613,  -0.6265193 ),
	vec2( -0.5282364,  0.02899032 ),
	vec2(  0.9069752, -0.2703182 ),
	vec2( -0.3698249,  0.3978794 ),
	vec2( -0.7183862, -0.6849706 ),
	vec2( -0.7184948,  0.6608582 ),
	vec2( -0.0925712, -0.9822884 ),
	vec2(  0.20099,   -0.4438035 ),
	vec2( -0.9637656,  0.1957591 ),
	vec2( -0.8576232, -0.262913 ),
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
	// vec2 rand = getRandom(position.xz*vUV);

	float ao = 0.0;
	depth = depth*2-1;
	vec2 radius = min(sampleRadius, sampleRadius/(1 - depth*0.9));
	// vec2 radius = sampleRadius/(1 - depth*2.9);

	const int iterations = 32;
	for(int i=0; i<iterations; ++i){
		vec2 rand = getRandom(position.xz*vUV*i);
		// vec2 refl = reflect(kernel[i], rand) * radius;
		// vec2 refl = normalize(kernel[i]) * radius;
		vec2 refl = kernel[i] * radius;
		ao += computeAmbientOcclusion(vUV, refl*rand, position, normal)*0.15;
		ao += computeAmbientOcclusion(vUV, refl*rand*0.75, position, normal)*0.35;
		ao += computeAmbientOcclusion(vUV, refl*rand*0.5, position, normal)*0.75;
		ao += computeAmbientOcclusion(vUV, refl*rand, position, normal)*length(refl)*15;
	}
	// ao *= ao;
	ao = clamp(ao/float(iterations), 0, 1);
	// ao = sqrt(ao);
	// ao = ceil(ao*6)/6;
	// outAO = clamp(vec4(1.0-ao/4.0/float(iterations)*28*depth), 0.5, 2);
	// outAO = vec4(1.0-ao/4.0/float(iterations)*28*depth);
	outAO = vec4(1.01-ao);
	// outAO = vec4(ao/4.0/float(iterations));
	// outAO = vec4(1-position.z/20);

}


#endif

