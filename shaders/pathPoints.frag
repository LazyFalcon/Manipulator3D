#version 330

uniform vec4 color;

out vec4 out_Color;

void main(void){
	vec2 tmp = gl_PointCoord* 2.0 - vec2(1.0);
	
	float r = dot(tmp, tmp);

	if(r > 1)
		discard;

	out_Color = vec4(color.rgb, 1-r);
}
