#ifdef COMPILING_VERTEX_SHADER

layout(location=0)in vec4 mVertex;

uniform mat4 uPV;
uniform mat4 uView;
uniform float uSize;

void main(){
	gl_PointSize = uSize;
	gl_Position = uPV*uView*mVertex;
}

#endif

#ifdef COMPILING_FRAGMENT_SHADER

uniform vec4 uColor;

out vec4 out_Color;

void main(void){
	vec2 tmp = gl_PointCoord* 2.0 - vec2(1.0);

	float r = dot(tmp, tmp);

	if(r > 1)
		discard;

	out_Color = vec4(uColor.rgb, 1-r);
}


#endif

