#version 330

uniform sampler2D tex;

in vec2 texCoords;
out vec4 out_Color;

void main(void){
	vec4 color = texture(tex, texCoords);
	if(color.r < 0.1)
		discard;
	out_Color = vec4(1,1,1,1);
	if(color.r < 0.5)
		out_Color = vec4(0,1,0,1);
	else if(color.r < 1)
		out_Color = vec4(1,0,0,1);
}
