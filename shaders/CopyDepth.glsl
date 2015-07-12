#ifdef COMPILING_VERTEX_SHADER

layout(location=0)in vec4 quadPosition;

out vec2 p_uv;

void main(){
	p_uv = (quadPosition.xy+vec2(1,1))/2;
	gl_Position = (vec4(quadPosition.xy,0,1));
}

#endif

#ifdef COMPILING_FRAGMENT_SHADER

out float outColor;

uniform sampler2D u_deptBuffer;

in vec2 p_uv;

void main(void){
	float color = texture(u_deptBuffer, p_uv).r;

	outColor = color;
}

#endif

