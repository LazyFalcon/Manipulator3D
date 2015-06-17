#version 330
#extension GL_ARB_shading_language_420pack: enable

layout(binding=0)uniform sampler2D noiseTex;
layout(binding=1)uniform sampler2D rampageTex;

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
in vec3 sines;

void main(void){
	float pi = 3.1415926f;
	vec4 noise = texture(noiseTex, uv);
	float disp = clamp(0.2+dot(noise.rgb, sines)*0.45,0,1);
	// vec4 color = texture(rampageTex, vec2(disp, 0.5));
	vec4 color = texture(rampageTex, vec2(disp, 0.5));
	
	color.a = noise.a;//dot(noise.rgb, vec3(1,1,1));
	
	// if(disp < 0.35)
		// discard;
	// color.a = 1;
	out_Color = color;
	// out_Color = vec4(1,1,1,1);
	
}
