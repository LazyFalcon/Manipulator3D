#version 330
#extension GL_ARB_shading_language_420pack: enable
uniform float specta;

out vec4 out_Color;
in vec2 in_texCoords;
in vec3 in_sunDirVert;
in vec3 lightDir;

//normalne terenu
layout(binding=2)uniform sampler2D texture;


void main(void){
	
	out_Color = vec4(color.rgb, 1.0); 
}
