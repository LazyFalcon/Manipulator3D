#version 330


layout(location=0)in vec4 vec;
layout(location=1)in vec2 uvs;
layout(location=2)in vec4 position;
layout(location=3)in vec4 texcoords;

uniform mat4 projection;
uniform mat4 view;

out float radius;
out float alpha_factor;
out vec4 pos;
out vec2 uv;
out vec2 uv2;
out vec2 mapping;

void main(){
	uv2 = texcoords.xy + texcoords.z*uvs;
	uv = uvs;
	mapping = vec.xy;
	 
	float scale = texcoords.w;
	// float scale = 50;
	alpha_factor = (60 - scale)/80;
	gl_Position = projection*(vec*scale + view*position);
	// gl_Position = projection*(vec*10 + view*vec4(0,0,-50,1));
	radius = scale/2;
	// pos = gl_Position;
	pos = vec*scale + view*position;
	
}
