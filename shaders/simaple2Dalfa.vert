#version 330

layout(location=0)in vec4 quadPosition;
layout(location=1)in vec2 position;
layout(location=2)in vec4 uv;
layout(location=3)in vec2 size;

uniform mat4 projection;

out vec2 texCoords;

void main(){
	gl_Position = projection*(vec4(quadPosition.xy*size+position,0,1));
	texCoords = uv.xy + quadPosition.xy*uv.zw;
}