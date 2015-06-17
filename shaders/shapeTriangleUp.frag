#version 330

in vec2 position;
in vec2 dimensions;
out vec4 out_Color;

void main(void){
	if( position.x <= 1 || position.y <= 1 || position.x >= dimensions.x-1 || position.y >= dimensions.y-1)
		out_Color = vec4(0.4,0.4,0.4,1);
	else
		out_Color = vec4(1,1,1,0.05);
}
