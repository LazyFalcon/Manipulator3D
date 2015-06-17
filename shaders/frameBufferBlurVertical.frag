#version 330

out vec4 outColor;
in vec2 texCoords;

uniform sampler2D texFramebuffer;
uniform float width;

void main(void){	
	float blurSize = 1.0/512.0;
	vec4 color = vec4(0,0,0,0);
	// if(texCoords.x<0.5){
   color += texture(texFramebuffer, vec2(texCoords.x, texCoords.y - 4.0*blurSize)) * 0.05;
   color += texture(texFramebuffer, vec2(texCoords.x, texCoords.y - 3.0*blurSize)) * 0.09;
   color += texture(texFramebuffer, vec2(texCoords.x, texCoords.y - 2.0*blurSize)) * 0.13;
   color += texture(texFramebuffer, vec2(texCoords.x, texCoords.y - blurSize)) * 0.15;
   color += texture(texFramebuffer, vec2(texCoords.x, texCoords.y)) * 0.16;
   color += texture(texFramebuffer, vec2(texCoords.x, texCoords.y + blurSize)) * 0.15;
   color += texture(texFramebuffer, vec2(texCoords.x, texCoords.y + 2.0*blurSize)) * 0.13;
   color += texture(texFramebuffer, vec2(texCoords.x, texCoords.y + 3.0*blurSize)) * 0.09;
   color += texture(texFramebuffer, vec2(texCoords.x, texCoords.y + 4.0*blurSize)) * 0.05;
	// }
	// else
		// color += texture(texFramebuffer, vec2(texCoords.x, texCoords.y));
	
	// color.rgb = pow(color.rgb, vec3(1,1,1)*2  );
	// color.a = 1;
	outColor = color;
}
