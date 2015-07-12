#ifdef COMPILING_VERTEX_SHADER

layout(location=0)in vec3 Vertex;
layout(location=1)in vec2 UV;
layout(location=2)in vec3 Normal;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;

void main(){
	gl_Position = u_projection*u_view*u_model*(vec4(Vertex,1));
}

#endif

#ifdef COMPILING_FRAGMENT_SHADER

uniform vec4 u_color;

out vec4 outColor;

void main(void){
	outColor = u_color;
}

#endif

