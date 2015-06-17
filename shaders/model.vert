#version 330


layout(location=0)in vec3 position;
layout(location=1)in vec2 uv;
layout(location=2)in vec3 normal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec2 texCoords;
// out vec3 Normal;
// out float sunLight;
// out vec3 eyeDir;
// out vec3 spotDir;
// out vec3 spotVector;
void main(){
	
	gl_Position=projection*view*model*vec4(position, 1.0);
	// Normal = (view*model * vec4(normal, 0.0)).xyz;
	// eyeDir=(-view*model*vec4(position, 1.0)).xyz;
	// sunVector=(view*vec4(1,1,1,0)).xyz;
	
	// spotDir= (view*model*vec4(position, 1)).xyz;
	// spotVector=(view*vec4(0,0,0,1)).xyz;
	// vec3 tmp=(view*vec4(0,0,500,0)).xyz;
	// sunLight=dot(normalize(Normal), normalize(tmp));
	// sunLight=clamp(sunLight,0,1);
	texCoords=uv;
}
//s³oñce per vertex, latarka per pixel
