 #version 330

out float outColor;
in vec2 texCoords;

uniform sampler2D texFramebuffer;

void main(void){	
	float color = texture(texFramebuffer, texCoords).r;

	outColor = color;
}
