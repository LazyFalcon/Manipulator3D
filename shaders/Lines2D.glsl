#ifdef COMPILING_VERTEX_SHADER

layout(location=0)in vec2 mVertex;

uniform mat4 uPV;

void main(){
	gl_Position = uPV*vec4(mVertex,0,1);
}

#endif

#ifdef COMPILING_FRAGMENT_SHADER
out vec4 outColor;

uniform vec4 uColor;

void main(void){
	outColor = uColor;
}

#endif

