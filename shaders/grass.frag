#version 330
uniform sampler2D mytexture;

in vec2 texCoords;
in float shading;

out vec4 out_Color;

void main(void){
	vec4 color = texture(mytexture, texCoords);
	if(color.a<0.6)
		discard;
	out_Color = vec4(color*shading); 
}
