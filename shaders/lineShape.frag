#version 330

uniform vec4 color;

out vec4 out_Color;

in float yPos;

void main(void){
	float alpha = 0.5 - abs(yPos);
	out_Color = vec4(color.rgb, alpha*3*color.a);
}
