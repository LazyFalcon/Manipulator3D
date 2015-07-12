#ifdef COMPILING_VERTEX_SHADER

layout(location=0)in vec4 mVertex;

out vec2 vUV;

void main(){
	vUV = (mVertex.xy+vec2(1,1))/2;
	gl_Position = (vec4(mVertex.xy,0,1));
}

#endif

#ifdef COMPILING_FRAGMENT_SHADER

out float outColor;

uniform sampler2D uDeptBuffer;

in vec2 vUV;

void main(void){
	float color = texture(uDeptBuffer, vUV).r;

	outColor = color;
}

#endif

