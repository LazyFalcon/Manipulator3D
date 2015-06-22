#version 330

uniform vec4 color;

uniform sampler2D metalTex;

in vec2 uv;
in vec4 normal;
in vec4 vertex;
in float depth;
in vec4 wv;


void main(void){
	float metal = texture(metalTex, uv*2);
	vec4 Normal = normalize(normal);
	gl_FragData[0] = color;
	gl_FragData[1] = vec4(Normal.xyz, metal);
}
