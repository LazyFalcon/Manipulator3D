#version 330

uniform vec4 color;

in vec2 position;
in vec2 dimensions;
out vec4 out_Color;

void main(void){
	if( position.x <= -dimensions.x/2+1 || position.y <= -dimensions.x/2+1 || position.x >= dimensions.x/2-1 || position.y >= dimensions.y/2-1)
		// out_Color = vec4(0.4,1,0,1);
		// out_Color = vec4(color*0.4,1);
		out_Color = vec4(color);
		// out_Color = vec4(0.4,1,0,1);
	else
		// out_Color = vec4(0.4,1,0,0.05);
		// out_Color = vec4(color*0.1,0.05);
		out_Color = vec4(color.xyz*0.125,0.05);
		// out_Color = vec4(1,0.4,0,0.05);
}
