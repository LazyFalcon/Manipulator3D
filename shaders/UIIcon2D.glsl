#ifdef COMPILING_VERTEX_SHADER

layout(location=0)in vec4 mVertex;

uniform float uWidth;
uniform float uHeight;
uniform vec4 uRect;
uniform vec4 uUVs;

out vec2 vUV;

void main(){
	gl_Position = vec4(
											(mVertex.xy*uRect.zw+uRect.xy)/vec2(uWidth/2, uHeight/2)-vec2(1)
											,0,1);
	vUV = uUVs.xy + mVertex.xy*uUVs.zw ;
}

#endif

#ifdef COMPILING_FRAGMENT_SHADER
out vec4 outColor;

uniform sampler2D uTexture;

uniform vec4 uColor;

in vec2 vUV;

void main(void){
	vec4 color = texture(uTexture, vUV);
	outColor = vec4(uColor.rgb, color.a*uColor.a);
	outColor = vec4(color * uColor);
}

#endif

