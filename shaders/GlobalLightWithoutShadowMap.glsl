#ifdef COMPILING_VERTEX_SHADER
// https://msdn.microsoft.com/en-us/library/windows/desktop/ee416324%28v=vs.85%29.aspx

layout(location=0)in vec4 mVertex;

out vec2 vUV;

void main(){
	gl_Position = (vec4(mVertex.xy,0,1));
	vUV = (mVertex.xy+vec2(1,1))/2;
}

#endif

#ifdef COMPILING_FRAGMENT_SHADER

out vec4 outColor;

uniform sampler2D uDiffuseTex;
uniform sampler2D uNormalTex;
uniform sampler2D uDepthTex;

uniform mat4  uInvPV;

uniform vec4  uColor;
uniform vec4  uEyePos;
uniform vec4  uLightDir;
uniform float uEnergy;

in vec2 vUV;
float ambient = 0.1;
float lambert(vec4 n, vec4 l, float energy){
	return clamp( dot(l.xyz, n.xyz), ambient, 1000.0 ) * energy;
	// return ambient + dot(l.xyz, n.xyz)*energy;
}
float spectacular(vec4 n, vec4 l, vec4 e){
	vec4 reflection = normalize(reflect(l, n));
	float ndoth = max(0.0, dot(reflection, e));
	return pow(ndoth, 128);
}
vec4 calcWorldPos(float depth){
	vec4 viewSpace = vec4(vUV*2-1, depth*2 - 1, 1);
	vec4 worldPos = uInvPV*viewSpace;
	worldPos.xyz /= worldPos.w;
	worldPos.w = 1;
	return worldPos;
}

void main(void){
	vec4 diffuse = texture(uDiffuseTex, vUV);
	vec4 normal = texture(uNormalTex, vUV);
	float depth = texture(uDepthTex, vUV).r;

	vec4 worldPos = calcWorldPos(depth);
	vec4 eyeVec = normalize(worldPos - uEyePos);


	// outColor = diffuse*lambert(uLightDir, normal, uEnergy)*uColor*ambient + spectacular(uLightDir, normal, eyeVec)*uColor;
	outColor = diffuse*lambert(uLightDir, normal, uEnergy)*uColor + spectacular(uLightDir, normal, eyeVec)*uColor;
	// outColor = diffuse + spectacular(uLightDir, normal, eyeVec)*uColor;
	// outColor = diffuse*lambert(uLightDir, normal, uEnergy);
	// outColor = normal;
	// outColor = vec4(depth);

	// outColor = abs(diffuse);

}

#endif

