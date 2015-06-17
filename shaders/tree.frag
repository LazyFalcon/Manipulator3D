#version 330

uniform float specta;

out vec4 out_Color;
in vec2 texCoords;
in float depth;

uniform sampler2D mytexture;

void main(void){
	vec4 color =	texture2D(mytexture, texCoords);
	if(color.a < 0.5)
		discard;
	out_Color = vec4(color*vec4(1+depth/1000,1+depth/1000,1+depth/700,1)); 
}
