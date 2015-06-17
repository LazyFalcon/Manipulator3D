#version 330
#extension GL_ARB_shading_language_420pack: enable

layout(location=0)in vec4 vec;
layout(location=1)in vec2 uvs;
layout(location=2)in vec4 position;
layout(location=3)in vec4 info;

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
out vec3 sines;

void main(){
	float pi = 3.1415*2;
	uv = uvs;
	float base = (time+position.x)/2;
	sines.x = sin((base)*pi)*0.25+0.25;
	sines.y = sin((base+0.3333)*pi)*0.25+0.25;
	sines.z = sin((base+0.6666)*pi)*0.25+0.25;
	// sines = normalize(sines);
	float scale = info.w;
	vec3 n = normalize(position.xyz);
	vec3 x = cross(n, vec3(0,0,1));
	vec3 y = cross(n,x);
	
	
	vec4 vertex = vec*scale;
	vertex.w = 0;
	// mat3 transform = mat3(x,y,n);
	// vec4 globalPos = vec4(transform*vec.xyz, 0)*scale + position;
	
	// mat4 view2 = mat4(
		// view[0],view[1],view[2],
		// 0,0,0,1
	// );
	// gl_Position = projection*view2*globalPos;
	gl_Position = projection*(vertex+view*position);
		
}

