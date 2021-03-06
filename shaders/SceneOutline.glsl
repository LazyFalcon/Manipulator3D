#ifdef COMPILING_VERTEX_SHADER

layout(location=0)in vec4 mVertex;

uniform mat4 uModel;

void main(){
	gl_Position = uModel*mVertex;
}

#endif

#ifdef COMPILING_FRAGMENT_SHADER

out vec4 outColor;

uniform vec4 uColor;

void main(void){
	outColor = uColor;
}

#endif

