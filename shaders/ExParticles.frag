#version 330
#extension GL_ARB_shading_language_420pack: enable

// layout(binding=0)uniform sampler2D disptex;
layout(binding=1)uniform sampler2D mytexture;

uniform mat4 projection;
uniform vec4 sunVector;
uniform vec4 lightPos;

out vec4 out_Color;

// out float gl_FragDepth; //[0, 1]
in vec4 dispColor;
in float radius;
in float rampage;
in vec4 pos;// position after transformations
in vec4 center;
in vec2 uvoffset;
in vec2 uv;
in vec2 mapping;

void main(void){
	float pi = 3.1415926f;
	vec4 color = texture(mytexture, vec2(rampage, 0.5));
	// vec4 dispColor = texture(disptex, uv);
	// color *= 1+0.9*dispColor.r;
	color.a = 1;
	if(radius -rampage > 6.0)
		discard;
	out_Color = color;
	
}
