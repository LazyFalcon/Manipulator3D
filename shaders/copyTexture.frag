 #version 330

out vec4 outColor;
in vec2 texCoords;

uniform sampler2D texFramebuffer;

void main(void){
	vec4 color = texture(texFramebuffer, texCoords);
	color = pow(color, vec4(18));
	outColor = color;
}
