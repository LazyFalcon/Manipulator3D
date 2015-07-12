#ifdef COMPILING_VERTEX_SHADER

layout(location=0)in vec4 position;

uniform mat4 u_projection;

void main(){
	gl_Position = u_projection*position;
}

#endif

#ifdef COMPILING_FRAGMENT_SHADER
out vec4 outColor;

uniform vec4 u_color;

void main(void){
	outColor = u_color;
}

#endif

