#version 330
#extension GL_ARB_shading_language_420pack: enable

layout(location=0)in vec4 Vertex;

uniform mat4 MVP;
uniform vec4 Info;

out vec2 uv;

void main(){
	uv = Vertex.xy+vec2(0.5,0.5);
	vec4 vertex = Vertex*vec4(Info.x, Info.y, 0, 1);
	// vertex.z = 1;
	vertex.w = 1;
	gl_Position = MVP*vertex;
	// gl_Position = vertex;
		
}

