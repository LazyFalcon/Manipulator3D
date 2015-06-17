#version 330

uniform sampler2D mainTex;
uniform vec4 color;
in vec2 uv;
out vec4 outColor;

void main(){
	vec4 alpha = texture(mainTex, uv);

	outColor = alpha.a*color;
	// outColor = vec4(1,0.5,0,1);
}

