#ifdef COMPILING_VERTEX_SHADER

layout(location=0)in vec3 Vertex;
layout(location=1)in vec2 UV;
layout(location=2)in vec3 Normal;

uniform mat4 u_PV;
uniform mat4 u_model;

void main(){
	gl_Position = u_PV*u_model*(vec4(Vertex,1));
}

#endif

#ifdef COMPILING_FRAGMENT_SHADER

out vec4 color;

void main(void){
	color = vec4(0.2, 0.1, 0.3, 0);
}

#endif

