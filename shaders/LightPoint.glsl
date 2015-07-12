#ifdef COMPILING_VERTEX_SHADER

layout(location=0)in vec3 mVertex;

uniform mat4  uPV;
uniform mat4  uView;
uniform vec4  uLightPos;
uniform float uSize;

out vec4 vScreenPosition;

void main(){
	gl_Position = uPV*uView*(vec4(mVertex*uSize, 0)+uLightPos);
	vScreenPosition = gl_Position;
}


#endif

#ifdef COMPILING_FRAGMENT_SHADER

out vec4 outColor;

uniform sampler2D uNormalBuffer;
uniform sampler2D uDepthBuffer;

uniform vec4 uColor;
uniform float uSize;
uniform mat4 uInvPV;
uniform vec4 uLightPos;
uniform vec4 uEyePos;
uniform float uEnergy;

in vec4 vScreenPosition;
const float numShades = 7.0;

void main(void){

	vec3 viewSpace = vScreenPosition.xyz / vScreenPosition.w;
	vec2 uv = viewSpace.xy*0.5+0.5;

	vec4 normal = texture(uNormalBuffer, uv);

	float metal = (normal.w+0.2);
	normal.w = 0;

	viewSpace.z = texture(uDepthBuffer, uv).r*2 - 1;

	vec4 worldPos = uInvPV*vec4(viewSpace, 1);
	worldPos.xyz /= worldPos.w;
	worldPos.w = 1;

	vec4 lightVec = worldPos-uLightPos;
	float l = length( lightVec );
	float attenuation = clamp(1.0 - l/(uSize), 0.0, 1.0);
	attenuation *= attenuation;

	float NdotL = clamp( dot(normalize(lightVec), -normal ),0.0,1.0);

	float d = length(lightVec);
	// float shadeIntensity = floor(energy*attenuation*NdotL*numShades)/numShades;
	float shadeIntensity = uEnergy*attenuation*NdotL*0.5;
	outColor = uColor*shadeIntensity*0.5;

}


#endif

/// first pass of light - draw to stencil buffer
