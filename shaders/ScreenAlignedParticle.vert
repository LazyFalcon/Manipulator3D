#version 330
#extension GL_ARB_shading_language_420pack: enable

layout(location=0)in vec4 Vertex;
layout(location=1)in vec2 UV;
layout(location=2)in vec4 Position;
layout(location=3)in vec4 Info;
layout(location=4)in vec4 TexCoords;

uniform mat4 projection;
uniform mat4 view;

out vec2 uv;
out float fading;

void main(){
	uv = TexCoords.xy + UV.yx*TexCoords.zw;
	fading = clamp(Info.z*4, 0, 1);
	vec4 vertex = Vertex.yxzw*Info;
	gl_Position = projection*(vertex+view*Position);
	// float distance = 
}

