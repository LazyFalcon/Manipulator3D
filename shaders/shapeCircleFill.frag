#version 330

uniform vec4 color;

in vec2 position;
in vec2 dimensions;
in vec2 texCoords;
out vec4 out_Color;

void main(void){
	float r = sqrt(dot(position, position));
	if(r > dimensions.x/2)
		discard;
	out_Color = vec4(color);
}
