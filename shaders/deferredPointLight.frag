 #version 330

out vec4 outColor;

uniform sampler2D normalTex;
uniform sampler2D depthTex;

uniform vec4 color;
uniform float size;
uniform mat4 invPV;
uniform vec4 lightPos;
uniform vec4 eyePos;
uniform float energy;

in vec4 persPos;
const float numShades = 7.0;

void main(void){

	vec3 viewSpace = persPos.xyz / persPos.w;
	vec2 uv = viewSpace.xy*0.5+0.5;

	vec4 normal = texture(normalTex, uv);

	float metal = (normal.w+0.2);
	normal.w = 0;

	viewSpace.z = texture(depthTex, uv).r*2 - 1;

	vec4 worldPos = invPV*vec4(viewSpace, 1);
	worldPos.xyz /= worldPos.w;
	worldPos.w = 1;

	vec4 lightVec = worldPos-lightPos;
	float l = length( lightVec );
	float attenuation = clamp(1.0 - l/size, 0.0, 1.0);
	attenuation *= attenuation;
	float NdotL = clamp( dot(normalize(lightVec), -normal ),0.0,1.0);

	float d = length(lightVec);
	// float shadeIntensity = floor(energy*attenuation*NdotL*numShades)/numShades;
	float shadeIntensity = energy*attenuation*NdotL*0.5;
	outColor = color*shadeIntensity*0.5;

}

