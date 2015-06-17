#version 330

out vec4 out_Color;
in vec2 texCoords;
uniform sampler2D mytexture;

void main(void){
	out_Color = texture2D(mytexture, texCoords);
}
