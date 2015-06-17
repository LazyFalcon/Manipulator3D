#version 330

layout(location=0)in vec4 position;

uniform mat4 projection;
uniform mat4 view;
uniform float size;

void main(){
	gl_PointSize = size;
	gl_Position = projection*view*position;
}