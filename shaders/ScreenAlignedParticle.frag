#version 330
#extension GL_ARB_shading_language_420pack: enable

layout(binding=0)uniform sampler2D mainTex;

uniform vec4 sunVector;
uniform vec4 lightPos;

out vec4 out_Color;

in vec2 uv;
in float fading;

void main(void){
	vec4 color = texture(mainTex, uv);
	
	color.rgb *= color.a;
	color.a *= fading;
	// color.a = 1;
	out_Color = color;
	// out_Color = vec4(1,0,0,1);

	
}
