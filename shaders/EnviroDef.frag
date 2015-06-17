#version 330

uniform sampler2D mytexture;

uniform vec4 lightVec;

in vec2 uv;
in vec4 normal;
in vec4 vertex;
in float depth;
in vec4 wv;


void main(void){
	vec4 Normal = normalize(normal);
	vec4 color = texture(mytexture, uv);
	gl_FragData[0] = color;
	gl_FragData[1] = vec4(Normal.xyz, 0);
}
