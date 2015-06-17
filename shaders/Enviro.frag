#version 330

uniform sampler2D mytexture;

uniform vec4 lightVec;

out vec4 out_Color;

in vec2 uv;
in vec4 normal;
in vec4 vertex;


void main(void){
	vec4 color = texture(mytexture, uv);
	vec4 Normal = normalize(-normal);
	
	float bright = clamp(dot(lightVec, Normal), 0.3, 0.8);

	color = color*bright;
	
	
	out_Color = color;
}
