#version 330


layout(location=0)in vec3 position;
layout(location=1)in vec2 uv;
layout(location=2)in vec3 normal;

uniform mat4 projection;
uniform mat4 view;
uniform vec3 Trans;



void main(){
	gl_Position = projection*view*vec4(Trans+position, 1.0);
}
