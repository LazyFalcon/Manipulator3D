#ifdef COMPILING_VERTEX_SHADER

layout(location=0)in vec4 mVertex;
layout(location=1)in vec4 mInfo;
layout(location=2)in vec4 mColor;

uniform float uWidth;
uniform float uHeight;

out vec4 vColor;
out vec4 vInfo;
out vec2 vPosition;

void main(){
	vec2 dimensions = mInfo.zw;
	vInfo = mInfo;
	vColor = mColor.abgr;
	vPosition = vec2(mVertex.x*mInfo.z, mVertex.y*mInfo.w);
	gl_Position = vec4((vPosition+floor(mInfo.xy))/vec2(uWidth/2, uHeight/2)-vec2(1), 0, 1);
}

#endif

#ifdef COMPILING_FRAGMENT_SHADER
out vec4 outColor;

in vec4 vColor;
in vec4 vInfo;
in vec2 vPosition;

void main(void){
	if(vPosition.x >= 0 && vPosition.x <= 1 || vPosition.y >= 0 && vPosition.y <= 1 || vPosition.x >= 0 && vPosition.x <= 1 || vPosition.y >= 0 && vPosition.y <= 1)
		outColor = vColor*0.5;
	else
		outColor = vColor;
}

#endif

