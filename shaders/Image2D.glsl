#ifdef COMPILING_VERTEX_SHADER

layout(location=0)in vec4 mVertex;

uniform mat4 uPV;
uniform vec4 uBox;

out vec2 vUV;

void main(){
	vec4 newPlane = vec4(mVertex.xy*uBox.zw + uBox.xy, 0.5, 1);

	vUV = mVertex.xy;
	gl_Position = uPV*newPlane;
}

#endif

#ifdef COMPILING_FRAGMENT_SHADER

out vec4 outColor;
in vec2 vUV;

uniform sampler2D uTexture;

void main(void){
	vec4 color = texture(uTexture, vUV);
	// color = pow(color, vec4(18));
	outColor = color;
}

#endif

