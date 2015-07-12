#ifdef COMPILING_VERTEX_SHADER

layout(location=0)in vec3 mVertex;
layout(location=1)in vec2 mUV;
layout(location=2)in vec3 mNormal;

uniform mat4 uPV;
uniform mat4 uModel;

void main(){
	gl_Position = uPV*uModel*(vec4(mVertex,1));
}

#endif

#ifdef COMPILING_FRAGMENT_SHADER

out vec4 outColor;

void main(void){
	outColor = vec4(0.2, 0.1, 0.3, 0);
}

#endif

