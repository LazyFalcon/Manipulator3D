#ifdef COMPILING_VERTEX_SHADER
// https://msdn.microsoft.com/en-us/library/windows/desktop/ee416324%28v=vs.85%29.aspx

layout(location=0)in vec3 mVertex;
layout(location=1)in vec2 mUV;
layout(location=2)in vec3 mNormal;

uniform mat4 uPV;
uniform mat4 uView;
uniform mat4 uModel;
uniform mat4 uNM;
uniform mat4 uShadowProjection;

out vec2 vUV;
out vec4 vNormal;
out vec4 vVertex;
out vec4 vVertexInShadow;

void main(){
	vNormal = uNM*(vec4(mNormal,0));
	// vNormal = model*(vec4(Normal,0));
	vVertex = uModel*(vec4(mVertex,1));

	vVertexInShadow = uShadowProjection*vVertex;

	vUV = mUV;
	gl_Position = uPV*uView*vVertex;
}


#endif

#ifdef COMPILING_FRAGMENT_SHADER


uniform sampler2D uMetalTex;
uniform sampler2DShadow uShadowTex;

uniform vec4  uColor;

uniform vec4  uEyePos;
uniform vec4  uLightPos;
// uniform vec4  uColor;
uniform float uSize;
uniform float uEnergy;

uniform vec4  uLightPos2;
uniform vec4  uColor2;
uniform float uSize2;
uniform float uEnergy2;

in vec2 vUV;
in vec4 vNormal;
in vec4 vVertex;
in vec4 vVertexInShadow;

float shade(vec4 n, vec4 l, float e){
	// return clamp( dot(l, -n), 0.0, 1.0 ) * e;
	return clamp( dot(l, -n), 0.7, 1.0 ) * e;
}
float spectacular(vec4 n, vec4 l, vec4 e){
	vec4 reflection = normalize(reflect(-l, n));
	float ndoth = max(0.0, dot(reflection, e));
	return pow(ndoth, 128);
}
float attenuation(float d, float s){
	// return 1.0 / (1.0 + 0.1*d + 0.01*d*d);
	return 1.1-(clamp(d, 0.0, s)/s);
}

vec2 kernel[5] = vec2[]
(
	vec2(0.0, 0.0),// vec2(1.0, 1.0), vec2(-1.0, 1.0), vec2(-1.0, -1.0), vec2(1.0, -1.0)
	vec2( -0.94201624, -0.39906216 ),
  vec2( 0.94558609, -0.76890725 ),
  vec2( -0.094184101, -0.92938870 ),
  vec2( 0.34495938, 0.29387760 )
);

const float DepthBias = -0.0007;


void main(void){
	float metal = texture(uMetalTex, vUV*3).r;
	vec4 vertexInShadow = vVertexInShadow;
	vertexInShadow.xyz /= vertexInShadow.w;
	vertexInShadow.xy = vertexInShadow.xy * 0.5 + vec2(0.5);
	vertexInShadow.z = vertexInShadow.z * 0.5 + 0.5;

	vec4 N = normalize(vNormal);
/*
	// vec4 L = vVertex - uLightPos;
	// float d = length(L);
	// L /= d;
	// vec4 l_shade = vec4(0.4 + clamp(dot(-N, L), -0.1,0.0));
	// vec4 l_shade = vec4(0.8);
	// vec4 l_spect = vec4(0);

	// float shadowFactor = 0.0;

	// float m = max( abs(N.z / N.x), abs(N.z / N.y) );
	// float m = max( abs(N.z), max(abs(N.x), abs(N.y) ));
	// float lightAngle = dot(-N,L);
	// float bias = (lightAngle * DepthBias) + DepthBias;


	// for(int i=0; i<5; i++){
	// for(int i=0; i<0; i++){
		// vec3 uvz = vec3(vertexInShadow.xy+kernel[i]/2048, vertexInShadow.z + bias);
		// shadowFactor += texture(uShadowTex, uvz);
	// }
	// shadowFactor = 0.25 + shadowFactor/5;
	// shadowFactor = 0.4 + shadowFactor/5;
	// shadowFactor = 0.3 + clamp(shadowFactor,0,1)-1;
	// shadowFactor = shadowFactor/5;


	// float shadowIntensity = 1.0;
	// float shadowIntensity = shadowFactor;
	// if(lightDepth < depthFromTex && dot(-N,L) > 0){
	// if(shadowFactor > 0.1 && dot(-N,L) > 0){
	// if(shadowFactor > 0.2 && lightAngle > -0.1){
	// if(false){
		// shadowIntensity = shadowFactor;
		// shadowIntensity = 1.0;

		// N *= metal;
		// vec4 E = vVertex - uEyePos;
		// float e = length(E);
		// E /= e;

		// l_shade = shade(N,L, uEnergy)*attenuation(d, uSize)*uColor*shadowIntensity;
		// l_spect = spectacular(N, L, E)*metal*uColor*attenuation(e, 20.0);

		// L = vVertex - uLightPos2;
		// d = length(L);
		// L /= d;

		// l_shade += shade(N,L, uEnergy2)*attenuation(d, uSize2)*uColor2*shadowIntensity;
		// l_spect += spectacular(N, L, E)*metal*uColor2*attenuation(e, 20.0);
	// }
	 */
	// gl_FragData[0] = color*l_shade*2 + l_spect;
	gl_FragData[0] = uColor;
	gl_FragData[1] = vec4(N.xyz, 0.0);
}

#endif

