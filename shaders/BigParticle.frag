#version 330
#extension GL_ARB_shading_language_420pack: enable

layout(binding=0)uniform sampler2D mytexture;
uniform mat4 projection;
uniform vec4 sunVector;
uniform vec4 lightPos;

out vec4 out_Color;

in vec2 uv;
in vec4 normal;
in vec4 vertex; // in view


void main(void){
	vec4 color = texture(mytexture, uv);
	normal = normalize(-normal);
	
	// sun light
	float bright = clamp(dot(sunVector, normal), 0.3, 0.8);
	// color = color*diff;
	
	// point light
	bright += clamp( dot( normalize(vertex-lightPos), normal   ), 0.2, 0.7);
	color = color*bright;
	
	color.a = 1;
	out_Color = color;
}
