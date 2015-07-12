#ifdef COMPILING_VERTEX_SHADER

layout(location=0)in vec2 position;

uniform mat4 u_projection;

void main(){
	gl_Position = u_projection*vec4(position,0,1);
}

#endif

#ifdef COMPILING_FRAGMENT_SHADER

uniform vec4 u_color;
out vec4 outColor;

void main(void){
	outColor = u_color;
}

#endif

