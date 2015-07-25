#ifdef COMPILING_VERTEX_SHADER

layout(location=0)in vec4 mVertex;

out vec2 vUV;

void main(){
	gl_Position = (vec4(mVertex.xy,0,1));
	vUV = (mVertex.xy+vec2(1,1))/2;
}

#endif

#ifdef COMPILING_FRAGMENT_SHADER

out vec4 outColor;

in vec2 vUV;

uniform sampler2D uTexture;

void main(void){
	vec4 color = texture(uTexture, vUV);
	// outColor = color;
	outColor = vec4(1.5);
}

#endif

