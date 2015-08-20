#ifdef COMPILING_VERTEX_SHADER

layout(location=0)in vec3 mVertex;

uniform mat4 uPV;
uniform mat4 uView;
uniform mat4 uModel;

void main(){
	gl_Position = uPV*uView*uModel*(vec4(mVertex,1))*1.1;
}

#endif

#ifdef COMPILING_FRAGMENT_SHADER

out vec4 outColor;

uniform vec4 uColor;

void main(void){
	outColor = uColor;
}

#endif

