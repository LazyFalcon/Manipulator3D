 #version 330

out vec4 outColor;
in vec2 texCoords;

uniform sampler2D texFramebuffer;

void main(void){	
	vec4 color = texture(texFramebuffer, texCoords);
	// if(texCoords.x < 0.5)
	float y = texCoords.y*250*3.14;
	// color.rgb = pow(color.rgb, vec3(1,1,1)*clamp((1+texCoords.x + texCoords.y), 0.9,1.3)  )*(1 + sin(2*y)*0.03);
	// color.rgb = pow(color.rgb, vec3(1,1,1)*clamp((1+texCoords.x + texCoords.y), 0.9,1.3)  ) + sin(2*y)*0.009;
	color.rgb = pow(color.rgb, vec3(1,1,1)*clamp((1+texCoords.x + texCoords.y), 0.9,1.3+ sin(2*y)*0.09)  );

	outColor = color;
}
