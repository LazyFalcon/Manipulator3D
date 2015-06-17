#version 330



layout(location=0)in vec3 position;
layout(location=1)in vec2 uv;
layout(location=2)in vec3 normals;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec2 texCoords;


void main(){
	mat4 tmp = projection*view*model;
	vec4 pos = tmp*vec4(position, 1.0);
	gl_Position = pos.xyww;
	texCoords = uv;
}

