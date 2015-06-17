#version 330

uniform vec4 color;

in vec2 position;
in vec2 dimensions;
out vec4 out_Color;

void main(void){
	if( position.x <= 1 || position.y <= 1 || position.x >= dimensions.x-1 || position.y >= dimensions.y-1)
		// out_Color = vec4(0.4,1,0,1);
		out_Color = vec4(color);
	else
		// out_Color = vec4(0.4,1,0,0.05);
		out_Color = vec4(color/8);
}
