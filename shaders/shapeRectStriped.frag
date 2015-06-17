#version 330

uniform vec4 color;

in vec2 position;
in vec2 dimensions;
out vec4 out_Color;

void main(void){
	// if(position.y)
	// if( abs(ceil(sin(position.x)*10))>5)
	if(mod(gl_FragCoord.x, 7) < 5)
		// out_Color = vec4(0.4,1,0,1);
		// out_Color = vec4(color*0.4,1);
		out_Color = vec4(color);
		// out_Color = vec4(0.4,1,0,1);
	else
		discard;
}
