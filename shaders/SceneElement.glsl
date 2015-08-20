#ifdef COMPILING_VERTEX_SHADER

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

	vUV = mUV;
	gl_Position = uPV*uView*vVertex;
}


#endif

#ifdef COMPILING_FRAGMENT_SHADER

uniform sampler2D uMetalTex;

uniform uint uID;
uniform vec4 uColor;

in vec2 vUV;
in vec4 vNormal;
in vec4 vVertex;

void main(void){
	float metal = texture(uMetalTex, vUV*3).r;
	vec4 N = normalize(vNormal);

	gl_FragData[0] = uColor;
	gl_FragData[1] = vec4(N.xyz, uID/16384.f);
}

#endif

