#ifdef COMPILING_VERTEX_SHADER

layout(location=0)in vec4 mVertex;
layout(location=1)in vec2 mPosition;
layout(location=2)in vec4 mUV;
layout(location=3)in vec2 mSize;
layout(location=4)in vec4 mColor;

uniform mat4 uPV;

out vec2 vUV;
out vec4 vColor;

void main(){
	vColor = mColor;
	gl_Position = uPV*(vec4(mVertex.xy*mSize+mPosition,0,1));
	vUV = mUV.xy + mVertex.xy*mUV.zw;
}

#endif

#ifdef COMPILING_FRAGMENT_SHADER

uniform sampler2D uTexture;

in vec2 vUV;
in vec4 vColor;

out vec4 outColor;

void main(void){
	float r = texture(uTexture, vUV).r;

	outColor = vec4(vColor.abg, r*vColor.r);
}

#endif

