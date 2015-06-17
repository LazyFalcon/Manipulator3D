#version 330


layout(location=0)in vec4 position;
layout(location=1)in vec2 uv;
layout(location=2)in vec4 normal;
layout(location=3)in vec4 tangent;
layout(location=4)in vec4 bitangent;

uniform mat4 projection;
uniform mat4 view;
// uniform mat4 normalMatrix;
uniform vec4 lightPos;
uniform vec4 lightDir;
uniform vec4 uvMorph;

layout(std140) uniform Bones {
    mat4 bones[100];
};

out vec2 texCoords;
out vec2 texCoordsTrue;
out vec3 Normal;
out vec4 Vertex;
out vec3 Eye;
out vec3 T;
out vec3 B;

void main(){
	int index = int(position.w);
	mat4 model;
	// if(index == 1)
		model = bones[index];
	// else
		// model = mat4(0);
	Vertex = (model*vec4(position.xyz, 1.0));
	// Normal = (view*model * normal).xyz;
	Normal = (model * normal).xyz;
	// Normal = (normalMatrix * normal).xyz;
	Eye = -Vertex.xyz;
	
	texCoords = abs(vec2(uvMorph.x-uv.x, uvMorph.y-uv.y));
	texCoordsTrue = uv.xy;
	gl_Position = projection*(view*model)*vec4(position.xyz, 1.0);
}
//s³oñce per vertex, latarka per pixel
