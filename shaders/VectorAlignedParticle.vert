#version 330
#extension GL_ARB_shading_language_420pack: enable

layout(location=0)in vec4 Vertex;
layout(location=1)in vec2 UV;
layout(location=2)in vec4 Position;
layout(location=3)in vec4 VectorX;
layout(location=4)in vec4 VectorY;
layout(location=5)in vec4 TexCoords;

uniform mat4 projection;
uniform mat4 view;

out vec2 uv;
out float fading;

void main(){
	uv = TexCoords.xy + UV.yx*TexCoords.zw;
	fading = clamp(Position.w, 0, 1);
	vec4 vertex = Vertex.x*VectorX + Vertex.y*VectorY;
	gl_Position = projection*view*vec4(vertex.xyz+Position.xyz,1);
		
}

