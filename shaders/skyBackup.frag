#version 330

out vec4 out_Color;
in vec2 texCoords;
in float height;
uniform sampler2D mytexture;

void main(void){
	// out_Color = texture2D(mytexture, texCoords);
	out_Color = height*vec4(0.31, 0.40, 0.56,1);
}
