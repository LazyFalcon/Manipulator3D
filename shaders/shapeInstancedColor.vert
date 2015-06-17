#version 330
layout(location=0)in vec4 mVertex;
layout(location=1)in vec4 mInfo;
layout(location=2)in vec4 mColor;

uniform float uWidth;
uniform float uHeight;

out vec4 info;
out vec2 dimensions;
out vec2 position;
out vec4 color;

void main(){
	info = mInfo;
	dimensions = info.zw;
	color = mColor.abgr;
	position = vec2(mVertex.x*info.z, mVertex.y*info.w);
	// gl_Position = projection*vec4(position+floor(info.xy), 0, 1);
	gl_Position = vec4((position+floor(info.xy))/vec2(uWidth/2, uHeight/2)-vec2(1), 0, 1);

}
