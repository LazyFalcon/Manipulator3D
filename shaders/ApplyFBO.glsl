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
	float y = vUV.y*250*3.14;
	color.rgb = pow(color.rgb, vec3(1.0/2.2)*clamp((1+vUV.x + vUV.y), 0.9,1.3+ sin(2*y)*0.09)  );

	outColor = color;
}

#endif

