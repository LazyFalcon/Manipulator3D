#ifdef COMPILING_VERTEX_SHADER

layout(location=0)in vec4 vertex;

uniform float uWidth;
uniform float uHeight;
uniform vec4 u_rect;
uniform vec4 u_uvs;

out vec2 p_uv;

void main(){
	gl_Position = vec4(
											(vertex.xy*u_rect.zw+u_rect.xy)/vec2(uWidth/2, uHeight/2)-vec2(1)
											,0,1);
	p_uv = u_uvs.xy + vertex.xy*u_uvs.zw ;
}

#endif

#ifdef COMPILING_FRAGMENT_SHADER
out vec4 outColor;

uniform sampler2D u_texture;

uniform vec4 uColor;

in vec2 p_uv;

void main(void){
	vec4 color = texture(u_texture, p_uv);
	outColor = vec4(uColor.rgb, color.a*uColor.a);
}

#endif

