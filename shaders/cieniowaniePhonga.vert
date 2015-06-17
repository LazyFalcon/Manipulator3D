#version 330


layout(location=0)in vec3 position;
layout(location=1)in vec2 uv;
layout(location=2)in vec3 normal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec4 lightPos;
uniform vec4 lightDir;

out vec2 texCoords;
out vec3 Normal;
out vec3 light_dir;
out vec3 eye_dir;
out vec3 lightVector;
void main(){
	
	gl_Position=projection*view*model*vec4(position, 1.0);
	Normal = (view*model * vec4(normal, 0.0)).xyz;
	light_dir = (view*(lightPos-model*vec4(position, 1.0))).xyz;
	eye_dir=(-view*model*vec4(position, 1.0)).xyz;
	lightVector=(view*lightDir).xyz;
	texCoords=uv;
}

