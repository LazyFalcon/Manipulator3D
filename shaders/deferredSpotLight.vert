#version 330
#extension GL_ARB_shading_language_420pack: enable

layout(location=0)in vec3 Vertex;
layout(location=1)in vec2 UV;
layout(location=2)in vec3 Normal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec4 position;
uniform vec3 size;

out vec4 persPos;

void main(){
	gl_Position = projection*view*(model*vec4(Vertex*vec3(size.z*2, size.z*2, size.y), 0) + position);
	persPos = gl_Position;
}

