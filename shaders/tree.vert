#version 330


layout(location=0)in vec3 position;
layout(location=1)in vec2 uv;
layout(location=2)in vec3 normal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform float time;

out vec2 texCoords;
out float depth;

void main(){
	float d = sin(time+position.x*0.1)*position.z*0.01;
	gl_Position = projection*view*model*vec4(position+vec3(d,0,0), 1.0);
	
	depth = abs((view*model*vec4(position+vec3(d,0,0), 1.0)).z);
	texCoords = uv;
}
