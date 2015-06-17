#version 330

layout(location=0)in vec4 quadPosition;
layout(location=1)in vec2 position;
layout(location=2)in vec4 uv;
layout(location=3)in vec2 size;
layout(location=4)in vec4 color;

uniform mat4 projection;

out vec2 texCoords;
out vec2 quad;
out vec4 in_color;

void main(){
	quad = quadPosition.xy;
	in_color = color;
	gl_Position = projection*(vec4(quadPosition.xy*size+position,0,1));
	texCoords = uv.xy + quadPosition.xy*uv.zw;
	// texCoords = uv.xy + (quadPosition.xy+vec2(0.5))*uv.zw;
}

