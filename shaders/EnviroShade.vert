#version 330
#extension GL_ARB_shading_language_420pack: enable

layout(location=0)in vec3 Vertex;
layout(location=1)in vec2 UV;
layout(location=2)in vec3 Normal;

uniform mat4 u_PV;
uniform mat4 u_model;

void main(){
	gl_Position = u_PV*u_model*(vec4(Vertex,1));
}

