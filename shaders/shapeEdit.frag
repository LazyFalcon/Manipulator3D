#version 330

uniform vec4 color;
// uniform vec4 bcgColor;

in vec2 position;
in vec2 dimensions;
in vec2 normalizedPos;
out vec4 out_Color;

void main(void){
	// if( position.x <= 1 || position.y <= 1 || position.x >= dimensions.x-1 || position.y >= dimensions.y-1)
		out_Color = vec4(color);
	// else
		// out_Color = vec4(bcgColor.rgb, bcgColor.a);
}
