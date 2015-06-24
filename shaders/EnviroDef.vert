#version 330
#extension GL_ARB_shading_language_420pack: enable

layout(location=0)in vec3 Vertex;
layout(location=1)in vec2 UV;
layout(location=2)in vec3 Normal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 NM;

out vec2 uv;
out vec4 in_normal;
out vec4 in_vertex;

void main(){
	in_normal = NM*(vec4(Normal,0));
	in_vertex = model*(vec4(Vertex,1));

	uv = UV;
	vec4 aa = projection*view*in_vertex;
	gl_Position = aa;
}

