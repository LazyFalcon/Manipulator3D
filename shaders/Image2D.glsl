#ifdef COMPILING_VERTEX_SHADER

layout(location=0)in vec4 vertex;

uniform mat4 u_projection;
uniform vec4 u_box;

out vec2 p_uv;

void main(){
	vec4 newPlane = vec4(vertex.xy*u_box.zw + u_box.xy, 0.5, 1);

	p_uv = vertex.xy;
	gl_Position = u_projection*newPlane;
}

#endif

#ifdef COMPILING_FRAGMENT_SHADER

out vec4 outColor;
in vec2 p_uv;

uniform sampler2D u_texture;

void main(void){
	vec4 color = texture(u_texture, p_uv);
	// color = pow(color, vec4(18));
	outColor = color;
}

#endif

