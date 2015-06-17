#version 330

layout(location=0)in vec4 quadPosition;

uniform mat4 projection;
uniform vec2 position;
uniform vec2 size;

out vec2 texCoords;

void main(){
	gl_Position = projection*(vec4(quadPosition.xy*size+position,0,1));
	texCoords = quadPosition.xy;
}

