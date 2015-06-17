#version 330
#extension GL_ARB_shading_language_420pack: enable


// out vec4 [2] out_Color;
in vec2 in_texCoords;
in vec4 Normal;
in vec4 Vertex;




void main(void){
	vec4 N = normalize(Normal);
	gl_FragData[0] = Vertex;
	// out_Color[0] = vec4(Vertex.xyz,1);
	// out_Color[1] = N;
	gl_FragData[1] = N;
	// gl_FragData[1] = N;

}
