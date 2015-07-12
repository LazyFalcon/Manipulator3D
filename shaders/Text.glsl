#ifdef COMPILING_VERTEX_SHADER

layout(location=0)in vec4 quadPosition;
layout(location=1)in vec2 position;
layout(location=2)in vec4 uv;
layout(location=3)in vec2 size;
layout(location=4)in vec4 color;

uniform mat4 u_projection;

out vec2 p_uv;
out vec2 p_quad;
out vec4 p_color;

void main(){
	p_quad = quadPosition.xy;
	p_color = color;
	gl_Position = u_projection*(vec4(quadPosition.xy*size+position,0,1));
	p_uv = uv.xy + quadPosition.xy*uv.zw;
}

#endif

#ifdef COMPILING_FRAGMENT_SHADER

uniform sampler2D texture;

in vec2 p_uv;
in vec2 p_quad;
in vec4 p_color;

out vec4 outColor;

void main(void){
	float r = texture2D(texture, p_uv).r;

	outColor = vec4(p_color.abg, r*p_color.r);
}

#endif

