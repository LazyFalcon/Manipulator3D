#version 330
#extension GL_ARB_shading_language_420pack: enable

layout(location=0)in vec4 Vertex;
layout(location=1)in vec2 UV;
layout(location=2)in vec4 Position;
layout(location=3)in vec4 Info;
layout(location=4)in vec4 Vector;
layout(location=5)in vec4 TexCoords;

uniform mat4 projection;
uniform mat4 view;

out vec2 uv;
out float fading;

void main(){
	uv = TexCoords.xy + UV.yx*TexCoords.zw;
	fading = clamp(Info.x*4, 0, 1);
	// uv = UV;
	vec4 scale = Info.zwxy;
	
	vec4 newY = vec4( normalize( cross(Vector.xyz, vec3(0,0,1)) ), 0 );
	vec4 newX = vec4( normalize( cross(Vector.xyz, newY.xyz) ), 0 );

	vec4 vertex = Vertex*scale;
	vertex.w = 0;
	vec4 newVertex = vertex.x*newX + vertex.y*newY;
	newVertex.w = 0;
	gl_Position = projection*view*(newVertex+Position);
		
}

