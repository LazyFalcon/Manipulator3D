#version 330


layout(location=0)in vec4 vec;
layout(location=1)in vec2 uv;

uniform mat4 projection;
out vec2 uvs;

void main(){
	uvs = uv;
	gl_Position = projection*vec4(vec.xy*100, 0, 1);
}
