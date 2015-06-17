#version 330


layout(location=0)in vec4 position;
layout(location=1)in vec2 uv;
layout(location=2)in vec4 normal;
layout(location=3)in vec4 tangent;
layout(location=4)in vec4 bitangent;

uniform mat4 projection;
uniform mat4 view;
uniform vec4 lightPos;
uniform vec4 lightDir;
uniform float translation;

layout(std140) uniform Bones {
    mat4 bones[100];
};

out vec2 texCoords;
out vec3 Normal;
out vec3 Vertex;
out vec3 Eye;
out vec3 pos;
out vec3 T;
out vec3 B;

void main(){
	int index = int(position.w);
	mat4 model;
	// if(index == 1)
		model = bones[index];
	// else
		// model = mat4(0);
	Vertex = (view*model*vec4(position.xyz, 1.0)).xyz;
	Normal = (view*model * normal).xyz;
	T = (view*model * tangent).xyz;
	B = (view*model * bitangent).xyz;
	Eye = -Vertex;
	
	texCoords = uv+vec2(0, translation);
	gl_Position = projection*vec4(Vertex, 1.0);
}


