#version 330
#extension GL_ARB_shading_language_420pack: enable

uniform mat4 view;

out vec4 out_Color;
in vec2 texCoords;
in vec3 Normal;
in vec3 Vertex;
in vec3 Eye;
in vec3 pos;
in vec3 T;
in vec3 B;

layout(binding=0)uniform sampler2D colorMap;
layout(binding=1)uniform sampler2D normalMap;
layout(binding=2)uniform sampler2D shadows;

float rand(vec2 co){
	return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main(void){
 	vec3 normal = normalize( Normal ); 
	vec3 eye = normalize( Eye );
	vec3 light = normalize( (view*vec4(0.0,0.0,-1.0,1.0)).xyz - Vertex );
	normal = normalize(texture(normalMap, texCoords).rgb)*2.0 -1.0; 

	vec4 color = texture(colorMap, texCoords);
	vec4 shadow = texture(shadows, texCoords);
	// T = normalize(T);
	// B = normalize(B);
	vec3 N = normalize(Normal);
	
	if(color.a < 0.5)
		discard;
	// gl_FragData[0] = color;
	// gl_FragData[1] = vec4(-N.xyz, 0);
 	// mat3 TBN = transpose(mat3(normalize(B), normalize(T), N));
	
	// light = TBN*light;
	// eye = TBN*eye;
	
	vec3 r = normalize( reflect( -light, normal ));
	vec3 sun_color = vec3( 1,1,1 );
	float ndotl = clamp(dot(normal, light), 0.2, 1.0);
	// float spec = pow(max(0.05, dot(r, eye)), 16.0)*8;
	// color.rgb = clamp(ndotl*color.rgb, 0.0, 1.0);
	// color.rgb = clamp(color.rgb, 0.0, 1.0);
	 
	out_Color = vec4(color);  
 
}
