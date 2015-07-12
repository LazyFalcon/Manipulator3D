#ifdef COMPILING_VERTEX_SHADER

layout(location=0)in vec4 mVertex;
layout(location=1)in vec4 mInfo;
layout(location=2)in vec4 mColor;

uniform float uWidth;
uniform float uHeight;

out vec4 p_color;

void main(){
	vec2 dimensions = mInfo.zw;
	p_color = mColor.abgr;
	vec2 position = vec2(mVertex.x*mInfo.z, mVertex.y*mInfo.w);
	gl_Position = vec4((position+floor(mInfo.xy))/vec2(uWidth/2, uHeight/2)-vec2(1), 0, 1);

}

#endif

#ifdef COMPILING_FRAGMENT_SHADER
out vec4 outColor;

in vec4 p_color;

void main(void){
		outColor = p_color.abgr;
}

#endif

