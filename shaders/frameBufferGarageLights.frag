 #version 330

out vec4 outColor;
in vec2 texCoords;

uniform sampler2D texFramebuffer;

void main(void){	
	vec4 color = texture(texFramebuffer, texCoords);
	// color.rgb = pow(color.rgb, vec3(1,1,1)*1.5)*1.3;
	
	outColor = color;
}
