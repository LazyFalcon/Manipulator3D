#version 330

uniform vec4 color;

out vec4 outColor;

in vec2 uv;
in vec4 normal;
in vec4 vertex;
in float depth;
in vec4 wv;


void main(void){
	outColor = color;
}
