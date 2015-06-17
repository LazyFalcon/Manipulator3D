#version 330

uniform vec4 color;

in vec2 position;
in vec2 dimensions;
out vec4 out_Color;

void main(void){
  vec2 pos2 = position;
	float r = sqrt(dot(position, position));
	if(r > dimensions.x/2 || r <dimensions.x/2-1)
		discard;
	// out_Color = vec4(0.4,0.4,0.4,1);
	out_Color = vec4(color);
}
