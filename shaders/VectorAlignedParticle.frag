#version 330
#extension GL_ARB_shading_language_420pack: enable

layout(binding=0)uniform sampler2D mainTex;

out vec4 out_Color;

in vec2 uv;
in float fading;

void main(void){
	vec4 color = texture(mainTex, uv);
	
	color.a *= fading;
	color.rbg *= color.a;
	out_Color = color;
	// out_Color = vec4(0.5,0.5,0.5,1);
	
}
