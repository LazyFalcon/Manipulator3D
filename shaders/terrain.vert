#version 330
#extension GL_ARB_shading_language_420pack: enable

layout(location=0)in vec3 position;//0-256

layout(binding=0)uniform sampler2D h_map;

uniform mat4 projection;
uniform mat4 view;
uniform vec4 pos;
uniform vec2 positionOffset;
uniform float h_rec;
uniform float terrainTexels;
uniform float texelUnit;

out vec2 in_texCoords;
out vec4 Normal;
out vec4 Vertex;
out vec4 worldCoords;
out float depth;

vec4 cmpNormal(vec2 uv, float texelSize){
	vec4 h;
	h.x = texture(h_map, in_texCoords+vec2(0,1)*texelSize);
	h.y = texture(h_map, in_texCoords+vec2(0,-1)*texelSize);
	h.z = texture(h_map, in_texCoords+vec2(1,0)*texelSize);
	h.w = texture(h_map, in_texCoords+vec2(-1,0)*texelSize);
	
	vec4 n;
	n.x = h.z-h.w;
	n.y = h.x-h.y;
	n.z = 2;
	n.w = 0;
	return normalize(n);
}

void main(){
	// in_texCoords = (position.xy+pos/texelUnit+positionOffset)/(terrainTexels);// wspó³zedne na mapie zrzutowane na przedzia³ [0:1]
	in_texCoords = (position.xy+pos.xy/texelUnit+positionOffset)/(terrainTexels);// wspó³zedne na mapie zrzutowane na przedzia³ [0:1]
	float texelSize = 1/terrainTexels;
	Normal = cmpNormal(in_texCoords, texelSize); 

	float h = texture(h_map,in_texCoords);
	
	vec4 Vertex = vec4(position.xy+pos.xy/texelUnit,position.z + h, 1.0)*vec4(texelUnit,texelUnit,1,1);
	
	// in_texCoords = position.xy/4;
	worldCoords = vec4(in_texCoords, h/texelUnit/terrainTexels, 0);
	depth = abs((view*Vertex).z);
	gl_Position = projection*view*Vertex;	
}
