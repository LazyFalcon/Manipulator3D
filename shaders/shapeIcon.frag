#version 330

uniform sampler2D mainTex;

in vec2 texCoords;
uniform vec4 uColor;
out vec4 out_Color;

void main(void){
	vec4 color = texture(mainTex, texCoords);

	out_Color = vec4(uColor.rgb, color.a*uColor.a);
}
