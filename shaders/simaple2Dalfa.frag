#version 330

uniform sampler2D texture;

in vec2 texCoords;

out vec4 out_Color;

void main(void){
	vec3 color = texture2D(texture, texCoords);
	out_Color = vec4(color); 
}


