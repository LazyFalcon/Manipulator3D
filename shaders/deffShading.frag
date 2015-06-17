 #version 330

out vec4 outColor;
in vec2 texCoords;

uniform sampler2D colorTex;
uniform sampler2D normalTex;
uniform sampler2D depthTex;

uniform mat4 invPV;
uniform mat4 invP;
uniform mat4 invV;
uniform mat4 ainvV;
uniform mat4 view;

void main(void){	

	vec4 lightCenter = vec4(30, 30, 10, 1);
	vec4 lightCenter2 = vec4(-30, 30, 10, 1);
	float lightSize_var = 50.0;
	vec4 color = texture(colorTex, texCoords);
	vec4 normal = texture(normalTex, texCoords);
	float depth = texture(depthTex, texCoords).r*2 - 1;
	
	vec3 viewSpace;
	viewSpace.xy = texCoords*2 - vec2(1.0);
	viewSpace.z = depth;
	
	vec4 worldPos = invPV*vec4(viewSpace, 1);
	worldPos.xyz /= worldPos.w;
	worldPos.w = 1;
	vec4 dir = worldPos-lightCenter;
	float l = length( dir);
	float dist = 1.0-pow((clamp (l ,0.0,lightSize_var)/lightSize_var),0.5);
	float lambert = clamp( dot(normalize(dir), -normal ),0.0,1.0);
	
	dir = worldPos-lightCenter2;
	l = length( dir);
	float dist2 = 1.0-pow((clamp (l ,0.0,lightSize_var)/lightSize_var),0.5);
	float lambert2 = clamp( dot(normalize(dir), -normal ),0.0,1.0);
	
	
	outColor = color*(lambert +lambert2)*(vec4(1,1,0.6,0)*dist+vec4(1,0.8,0.6,0)*dist2) ;

}
