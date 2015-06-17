#version 330
#extension GL_ARB_shading_language_420pack: enable

layout(location=0)in vec3 Vertex;
layout(location=1)in vec2 UV;
layout(location=2)in vec3 Normal;

uniform mat4 projection;
uniform mat4 view;
uniform vec4 lightPos;
uniform float size;

out vec4 persPos;

void main(){
	gl_Position = projection*view*(vec4(Vertex*size, 0)+lightPos);
	persPos = gl_Position;
}

