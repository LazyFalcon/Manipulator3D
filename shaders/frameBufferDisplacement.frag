#version 330
#extension GL_ARB_shading_language_420pack: enable

out vec4 outColor;
in vec2 texCoords;

layout(binding=0)uniform sampler2D texFramebuffer;
layout(binding=1)uniform sampler2D fractalNoise;
uniform float time;

void main(void){	
	vec4 color;
	vec4 displacement = texture(fractalNoise, texCoords);
	color = texture(texFramebuffer, texCoords+(displacement.rg-vec2(0.5, 0.5))*0.005);
	color.rgb = pow(color.rgb, vec3(1,1,1)*2  )*(1+displacement.a*0.05);
	color.a = 1;
	outColor = color;
	// outColor = vec4(abs(displacement1), abs(displacement2),0,1);
}
