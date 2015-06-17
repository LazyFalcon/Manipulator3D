 #version 330

out vec4 outColor;
in vec2 texCoords;

uniform sampler2D texFramebuffer;

void main(void){	
	vec4 color = texture(texFramebuffer, texCoords);
	// if(texCoords.x < 0.5)
	float y = texCoords.y*400*3.14;
	// texCoords = abs(texCoords*1.5 - vec2(1,1)*0.75);
		color.rgb = pow(color.rgb, vec3(1,1,1)*1.5)*1.3;
		color.rgb *= (1 + sin(2*y)*0.05);
	
	
	// color.r = sin(y);
	// outColor = color*vec4(0.0,1,0.0,1);
	// outColor = color*vec4(0.8,1,0.7,1);
	outColor = color;
}
