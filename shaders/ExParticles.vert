#version 330
#extension GL_ARB_shading_language_420pack: enable

layout(location=0)in vec3 vec;
layout(location=1)in vec2 uvs;
layout(location=2)in vec3 normal;
layout(location=3)in vec4 position;
layout(location=4)in vec4 info;

layout(binding=0)uniform sampler2D disptex;
uniform mat4 projection;
uniform mat4 view;
uniform float time;

out float radius;
out float rampage;
out vec4 pos;
out vec4 center;
out vec2 uvoffset;
out vec2 uv;
out vec2 mapping;
out vec4 dispColor;

void main(){
	// float pi = 6.282;
	float pi = 3.1415*2;
	vec4 color = texture(disptex, uvs);
	dispColor = color;
	uv = uvs;
	vec4 weights;
	float base = (time+position.x)/1.5;
	weights.x = color.r*( sin((base)*pi)*0.25 + 0.25 );
	weights.y = color.g*( sin((base+0.25)*pi)*0.25 + 0.25 );
	weights.z = color.b*( sin((base+0.5)*pi)*0.25 + 0.25 );
	weights.w = color.a*(sin((base+0.75)*pi)*0.25 + 0.25 );
	// weights /= (dot(weights, vec4(1,1,1,1)));
	
	// rampage = clamp(0.4*color+0.5*sin(time*3), 0, 1)*color;
	// rampage = (0.5+0.4*sin(time*3+gl_InstanceID))*dot(color*vec4(r,g,b,a), vec4(1,1,1,1));
	rampage = clamp(dot(vec4(1,1,1,1), weights)-0.1,0,1)*info.z/3;
	radius = (0.5+rampage)*info.w+0.4;
	gl_Position = projection*view*(vec4((vec*radius)+position.xyz,1));
		
}

