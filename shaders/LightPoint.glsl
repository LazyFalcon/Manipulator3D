#ifdef COMPILING_VERTEX_SHADER

layout(location=0)in vec3 Vertex;
layout(location=1)in vec2 UV;
layout(location=2)in vec3 Normal;

uniform mat4  u_projection;
uniform mat4  u_view;
uniform vec4  u_lightPos;
uniform float u_size;


void main(){
	gl_Position = u_projection*u_view*(vec4(Vertex*u_size, 0)+u_lightPos);
}


#endif

#ifdef COMPILING_FRAGMENT_SHADER

void main(void){
}


#endif

/// first pass of light - draw to stencil buffer
