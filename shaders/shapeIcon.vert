#version 330

layout(location=0)in vec4 quadPosition;

uniform float uWidth;
uniform float uHeight;
uniform vec4 rect;
uniform vec4 uvs;

out vec2 texCoords;

void main(){
	gl_Position = vec4(
											(quadPosition.xy*rect.zw+rect.xy)/vec2(uWidth/2, uHeight/2)-vec2(1)
											,0,1);
	texCoords = uvs.xy + quadPosition.xy*uvs.zw ;
}

