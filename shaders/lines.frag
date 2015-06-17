#version 330

uniform vec4 color;
out vec4 out_Color;

void main(void){
	// out_Color = vec4(0.2,1,0, 1);
	out_Color = color;
}
