#ifdef COMPILING_VERTEX_SHADER

layout(location=0)in vec4 position;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform float u_size;

void main(){
	gl_PointSize = u_size;
	gl_Position = u_projection*u_view*position;
}

#endif

#ifdef COMPILING_FRAGMENT_SHADER

uniform vec4 u_color;

out vec4 out_Color;

void main(void){
	vec2 tmp = gl_PointCoord* 2.0 - vec2(1.0);

	float r = dot(tmp, tmp);

	if(r > 1)
		discard;

	out_Color = vec4(u_color.rgb, 1-r);
}


#endif

