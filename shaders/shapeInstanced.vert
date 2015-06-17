#version 330


layout(location=0)in vec4 vec;
layout(location=1)in vec4 info;

uniform mat4 projection;

out vec2 dimensions;
out vec2 position;
out vec2 normalizedPos;
out vec4 info_;

void main(){
	dimensions = info.zw;
	info_ = info;
	position = vec2(vec.x*info.z, vec.y*info.w);
	normalizedPos = vec.xy*2-vec2(1);
	gl_Position = projection*vec4(position+floor(info.xy), 0, 1);

}
