#version 330

uniform sampler2D texture;
// uniform vec3 color;

in vec2 texCoords;
in vec2 quad;
in vec4 in_color;

out vec4 out_Color;

void main(void){
	float r = texture2D(texture, texCoords).r;

	out_Color = vec4(in_color.abg, r*in_color.r);
}
