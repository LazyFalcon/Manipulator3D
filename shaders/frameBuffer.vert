#version 330

layout(location=0)in vec4 quadPosition;


out vec2 texCoords;
out vec4 persPos;

void main(){
	gl_Position = (vec4(quadPosition.xy,0,1));
	persPos = gl_Position;
	// texCoords = quadPosition.zw;
	texCoords = (quadPosition.xy+vec2(1,1))/2;
}

