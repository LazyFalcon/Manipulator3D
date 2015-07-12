#ifdef COMPILING_VERTEX_SHADER

layout(location=0)in vec4 vertex;

uniform mat4 uProjection;
uniform vec4 uPattern;
uniform vec2 uInfo;
//info.x: rotacja

out vec2 vUV;

void main(){

	vec2 pos = vec2(
			vertex.x*cos(uInfo.x)*uPattern.z - vertex.y*sin(uInfo.x)*uPattern.w
			, vertex.y*cos(uInfo.x)*uPattern.w + vertex.x*sin(uInfo.x)*uPattern.z
		);
	gl_Position = uProjection*(vec4(pos.xy + uPattern.xy,0,1));
	vUV = (vertex.xy+vec2(0.5,0.5));
}

#endif

#ifdef COMPILING_FRAGMENT_SHADER

out vec4 outColor;

uniform sampler2D uTexture;

uniform vec4 uColor;

in vec2 vUV;

void main(void){
	float alpha = texture(uTexture, vUV).a;

	outColor = vec4(uColor.rgb, alpha*uColor.a);
}

#endif

