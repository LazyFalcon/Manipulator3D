#version 330

uniform sampler2D mainTex;

uniform vec4 uColor;
out vec4 out_Color;

in vec2 texCoords;

void main(void){
	vec4 color = texture(mainTex, texCoords);
	
	out_Color = vec4(uColor.rgb, color.a*uColor.a);	
	// out_Color = vec4(1,1,1,1);	
}
