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
	float attenuation = 1.0-pow((clamp (l ,0.0,size)/size),0.3);
	// float attenuation = 1.0;
	float NdotL = clamp( dot(normalize(lightVec), -normal )*metal,0.1,1.0);

	// spectacular
	vec4 eyeVec = normalize(worldPos - eyePos);
	vec4 reflection = normalize(reflect(lightVec, normal));
	float NdotH = dot(reflection, eyeVec);
	float spectacular = clamp(pow(NdotH, 16),0,0.5)*metal;

	// ----
	float d = length(lightVec);
	// float attenuation = 1.0 / (1 + 0.35*d + 0.44*d*d);
	// float shadeIntensity = floor(energy*attenuation*NdotL*numShades)/numShades;
	// shadeIntensity = clamp(shadeIntensity, 0.1, 0.9);
	float shadeIntensity = energy*attenuation*NdotL;
	// outColor = color*(shadeIntensity + spectacular*color.r);
	outColor = vec4(1)*(shadeIntensity + spectacular*color.r);
	// outColor = color*0.7;
	// outColor = worldPos;








	// ----

	// outColor = color*NdotL*attenuation*energy;
	// outColor = color*NdotL;
	// outColor = vec4(1);
	// outColor = color;

}


	// float r = size/2;
	// vec4 L = worldPos-lightPos;
	// float distance = length(L);
	// float d = max(distance-r, 0);
	// L /= distance;

	// float denom = d/r + 1;
	// float attenuation = 1 / (denom*denom);
	// attenuation = (attenuation - size/2) / (1 - size/2);
	// attenuation = max(attenuation, 0);

		// float dot = max(dot(L, -normal), 0);