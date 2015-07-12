#ifdef COMPILING_VERTEX_SHADER

layout(location=0)in vec3 mVertex;

uniform mat4  uPV;
uniform mat4  uView;
uniform vec4  uLightPos;
uniform float uSize;


void main(){
	gl_Position = uPV*uView*(vec4(mVertex*uSize, 0)+uLightPos);
}

#endif

#ifdef COMPILING_FRAGMENT_SHADER

void main(void){

}

#endif

/// first pass of light - draw to stencil buffer
