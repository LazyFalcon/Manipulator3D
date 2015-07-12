#ifdef COMPILING_VERTEX_SHADER

layout(location=0)in vec4 quadPosition;

out vec2 p_uv;
out vec4 p_persPos;

void main(){
	gl_Position = (vec4(quadPosition.xy,0,1));
	p_uv = (quadPosition.xy+vec2(1,1))/2;
}

#endif

#ifdef COMPILING_FRAGMENT_SHADER

out vec4 outColor;

in vec2 p_uv;

uniform sampler2D texFramebuffer;

void main(void){
	vec4 color = texture(texFramebuffer, p_uv);
	outColor = color;
}

#endif

