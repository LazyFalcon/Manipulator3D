#version 330

uniform sampler2D tex;
uniform float time;

in vec2 uvs;

out vec4 out_Color;

void main(void){
	// float color = 0.3;
	float color = texture(tex, uvs).g;
	// color = abs(color);
	out_Color = vec4(color,color,color,1);    
}
