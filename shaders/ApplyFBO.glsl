#ifdef COMPILING_VERTEX_SHADER

layout(location=0)in vec4 quadPosition;

out vec2 p_uv;
out vec4 p_persPos;

void main(){
	gl_Position = (vec4(quadPosition.xy,0,1));
	p_persPos = gl_Position;
	p_uv = (quadPosition.xy+vec2(1,1))/2;
}

#endif

#ifdef COMPILING_FRAGMENT_SHADER

out vec4 outColor;

in vec2 p_uv;

uniform sampler2D u_texture;

void main(void){
	vec4 color = texture(u_texture, p_uv);
	float y = p_uv.y*250*3.14;
	color.rgb = pow(color.rgb, vec3(1,1,1)*clamp((1+p_uv.x + p_uv.y), 0.9,1.3+ sin(2*y)*0.09)  );

	outColor = color;
}

#endif

