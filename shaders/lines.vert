#version 330

layout(location=0)in vec4 position;

uniform mat4 projection;

void main(){
	gl_Position = projection*position;
}
