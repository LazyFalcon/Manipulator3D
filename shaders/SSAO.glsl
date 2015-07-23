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

const float bias = 0.169;
const float scale = 95.01;
const float intensity = 5.1;
const vec2 sampleRadius = 95/vec2(1400, 720);
const float randomSize = 644;


vec4 getNormal(vec2 uv){
	// return View * vec4(texture(normalTex, uv).xyz, 0);
	return texture(uNormalBuffer, uv);
}
vec4 getPosition(vec2 uv, out float depth){
	depth = texture(uDepthBuffer, uv).r;
	// vec3 viewSpace = vec3(uv*2-vec2(1), 0);
	vec4 viewSpace = vec4(uv*2-1, depth*2 - 1, 1);
	vec4 worldPos = uInvPV*viewSpace;
	worldPos.xyz /= worldPos.w;
	worldPos.w = 1;
	// vec4 worldPos = vec4(viewSpace, 1);

	return worldPos;
}

vec2 getRandom(vec2 uv){
	// return normalize(texture(uSSAORandom, uv*randomSize)).xy;
	return normalize(texture(uSSAORandom, uv*randomSize)).xy*2 - vec2(1);
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
	vec2(1,1),
	vec2(-1,1),
	vec2(-1,-1),
	vec2(1,-1)
);


void main(void){
	float depth;
	vec4 position = getPosition(vUV, depth);
	vec4 normal = getNormal(vUV);
	// vec2 rand = getRandom(vUV);
	vec2 rand = getRandom(position.xz*vUV);
	// vec2 rand = getRandom(normal.xz);

	float ao = 0.0;
	// float radius = sampleRadius/(1 - depth*1.9);
	// float radius = sampleRadius*depth*1.0;
	float radius = sampleRadius.x;

	const int iterations = 4;
	for(int i=0; i<iterations; ++i){
		vec2 refl = reflect(kernel[i], rand) * radius;
		vec2 coord = vec2(refl.x*0.707 - refl.y*0.707, refl.x*0.707 + refl.y*0.707);

		ao += computeAmbientOcclusion(vUV, refl*0.25, position, normal);
		ao += computeAmbientOcclusion(vUV, coord*0.5, position, normal);
		ao += computeAmbientOcclusion(vUV, refl*0.75, position, normal);
		ao += computeAmbientOcclusion(vUV, coord, position, normal);
	}

	// outAO = clamp(vec4(1.0-ao/4.0/float(iterations)*28*depth), 0.5, 2);
	// outAO = vec4(1.0-ao/4.0/float(iterations)*28*depth);
	outAO = vec4(1.1-ao/4.0/float(iterations)*28);
	// outAO = vec4(ao/4.0/float(iterations));
	// outAO = vec4(1-position.z/20);

}


#endif

