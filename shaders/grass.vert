#version 330
#extension GL_ARB_shading_language_420pack: enable

layout(location=0)in vec3 vertex;
layout(location=1)in vec2 uv;
layout(location=2)in vec3 normal;
layout(location=3)in vec4 position;

uniform sampler2D ambientTexture;
uniform mat4 projection;
uniform mat4 view;
uniform float time;
uniform vec3 wind;
uniform vec3 lightDir;
uniform float timeNormalized;


out vec2 texCoords;
out float shading;

void main(){
	
	shading = dot(lightDir, -normal);
	shading = texture(ambientTexture, vec2(shading,timeNormalized));
	
	gl_Position = projection*view*(vec4(vertex+wind*vertex.z*0.2,1)+position);	
	texCoords = uv;
}

