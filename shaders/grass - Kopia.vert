#version 330
#extension GL_ARB_shading_language_420pack: enable

layout(location=0)in vec3 vertex;
layout(location=1)in vec2 uv;
layout(location=2)in vec3 normal;
layout(location=4)in vec4 position;

layout(binding=0)uniform sampler2D h_map;
layout(binding=1)uniform sampler2D mixmap;

uniform mat4 projection;
uniform mat4 view;
uniform vec2 pos_offset;
uniform float h_offset;
uniform float size;

uniform vec4 lightPos;
uniform vec4 sunDir;

out vec2 in_texCoords;
out vec3 in_sunDirVert;
out vec3 lightDir;

float ter_size=2.50;

void main(){
	lightDir = normalize(vec3(-1,1,1));
	
	vec2 texCoords = (chunk_position+position+pos_rec)/(size);
	
	float h = texture(h_map,texCoords).x*height-h_rec+0.5;
	
	mat4 transform = mat4( cos(position.z), -sin(position.z), 0.0, position.x,
													sin(position.z),  cos(position.z), 0.0, position.y,
													0.0, 0.0, position.w, h,
													0.0, 0.0, 0.0, 1.0 
	);
	
	gl_Position = projection*view*transform*vertex;
	
	in_sunDirVert = (view*(vec4(10,10,500,0)-posit)).xyz;
	
	in_texCoords = uv;
}
