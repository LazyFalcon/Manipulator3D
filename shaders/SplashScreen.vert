#version 330

layout(location=0)in vec4 quadPosition;


out vec2 texCoords;

void main(){
	gl_Position = (vec4(quadPosition.xy,0,1));
	texCoords = (quadPosition.xy+vec2(1,1))/2;
}

