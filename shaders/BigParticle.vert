#version 330
#extension GL_ARB_shading_language_420pack: enable

layout(location=0)in vec3 Vertex;
layout(location=1)in vec2 UV;
layout(location=2)in vec3 Normal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec2 uv;
out vec4 normal;
out vec4 vertex;

void main(){
	uv = UV;
	normal = view*model*(vec4(Normal,0));		
	vertex = view*model*(vec4(Vertex*1,1));
	
	gl_Position = projection*vertex;	
}

