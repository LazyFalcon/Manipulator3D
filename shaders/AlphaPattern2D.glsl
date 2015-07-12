#ifdef COMPILING_VERTEX_SHADER

layout(location=0)in vec4 vertex;

uniform mat4 projection;
uniform vec4 pattern;
uniform vec2 info;
//info.x: rotacja

out vec2 p_uv;

void main(){

	vec2 pos = vec2(
			vertex.x*cos(info.x)*pattern.z - vertex.y*sin(info.x)*pattern.w
			, vertex.y*cos(info.x)*pattern.w + vertex.x*sin(info.x)*pattern.z
		);
	gl_Position = projection*(vec4(pos.xy + pattern.xy,0,1));
	p_uv = (vertex.xy+vec2(0.5,0.5));
}

#endif

#ifdef COMPILING_FRAGMENT_SHADER

uniform sampler2D u_texture;

uniform vec4 u_Color;
out vec4 outColor;

in vec2 p_uv;

void main(void){
	float alpha = texture(u_texture, p_uv).a;

	outColor = vec4(u_Color.rgb, alpha*u_Color.a);
}

#endif

