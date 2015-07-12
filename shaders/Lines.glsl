#ifdef COMPILING_VERTEX_SHADER

layout(location=0)in vec4 position;

uniform mat4 projection;

void main(){
	gl_Position = projection*position;
}


#endif

#ifdef COMPILING_FRAGMENT_SHADER

uniform vec4 color;
out vec4 out_Color;

void main(void){
	out_Color = color;
}

#endif

