#version 330

in vec4 color;
out vec4 out_Color;

void main(void){
		out_Color = color.abgr;
}
