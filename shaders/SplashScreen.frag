 #version 330

out vec4 outColor;
in vec2 texCoords;

uniform sampler2D tex;

void main(void){	
	vec4 color = texture(tex, texCoords);
	outColor = color;
}
