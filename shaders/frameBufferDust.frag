 #version 330

out vec4 outColor;
in vec2 texCoords;

uniform sampler2D texFramebuffer;

void main(void){	
	vec4 color = texture(texFramebuffer, texCoords);
	
	color.a = clamp(color.a, 0, 0.9);
	
	color.rbg *= 1-color.a;
	
	outColor = color;
}
