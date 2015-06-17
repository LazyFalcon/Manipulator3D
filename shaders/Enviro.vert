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
out vec4 normal;
out vec4 vertex;

void main(){
	normal = NM*(vec4(Normal,0));	
	// normal = view*model*(vec4(Normal,0));		
	vertex = view*model*(vec4(Vertex,1));
	
	uv = UV;
	gl_Position = projection*vertex;	
}

