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
	// float attenuation = 1.0-pow((clamp (l ,0.0,size-0.5)/size),0.2);
	float attenuation = 1.1-(clamp(l ,0.0,size)/size);
	// float attenuation = 1.0;
	// float NdotL = clamp( dot(normalize(lightVec), -normal )*metal,0.02,1.0);
	// float NdotL = clamp( dot(normalize(lightVec), -normal )*metal,0.0,1.0);
	float NdotL = clamp( dot(normalize(lightVec), -normal ),0.0,1.0);

	// spectacular
	vec4 eyeVec = normalize(worldPos - eyePos);
	vec4 reflection = normalize(reflect(-lightVec, normal));
	float NdotH = max(0.0, dot(reflection, eyeVec));
	// float NdotH = dot(normalize(eyeVec + lightVec), -normal);
	// float spectacular = clamp(pow(NdotH, 8),0,0.99)*metal;
	float spectacular = pow(NdotH, 32)*metal;
	// float spectacular = pow(NdotH, 32);

	// ----
	float d = length(lightVec);
	// float attenuation = 1.0 / (1 + 0.35*d + 0.44*d*d);
	// float shadeIntensity = floor(energy*attenuation*NdotL*numShades)/numShades;
	// shadeIntensity = clamp(shadeIntensity, 0.1, 0.9);
	float shadeIntensity = energy*attenuation*NdotL;
	// outColor = color*(shadeIntensity + spectacular*color.r);
	outColor = vec4(1)*(shadeIntensity + spectacular*color.r);

}

