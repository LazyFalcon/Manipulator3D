#version 330

uniform vec4 color;

in vec2 position;
in vec2 dimensions;
out vec4 out_Color;

void main(void){
	// if(abs(position.x)<dimensions.x/2-1 && abs(position.y)<dimensions.y/2-1 || abs(position.x)<dimensions.x/2-20 || abs(position.y)<dimensions.y/2-20   )
	if(abs(position.x)<dimensions.x/2-2 && abs(position.y)<dimensions.y/2-2 || abs(position.x)<dimensions.x/2-20 || abs(position.y)<dimensions.y/2-20   )
		discard;
	out_Color = vec4(color);
}
