#version 330
#extension GL_ARB_shading_language_420pack: enable

layout(location=0)in vec3 Vertex;
layout(location=1)in vec2 UV;
layout(location=2)in vec3 Normal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec2 uv;

void main(){
	uv = UV;

	gl_Position = projection*view*model*(vec4(Vertex,1));
}

