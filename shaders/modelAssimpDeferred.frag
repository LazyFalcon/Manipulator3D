#version 330

uniform float specta;
uniform mat4 view;

layout(location = 0) out vec4 out_Color;
in vec2 texCoords;
in vec2 texCoordsTrue;
in vec3 Normal;
in vec4 Vertex;
in vec3 Eye;
in vec3 T;
in vec3 B;

uniform sampler2D mytexture;
uniform sampler2D overlayTex;

void main(void){
 	vec3 normal = normalize( Normal ); 
	vec3 eye = normalize( Eye );
	// vec3 light = normalize(vec4(0.5,1,2,0));
	// vec3 light = normalize(vec4(0.64,-0.62,0.5,0));
	// vec3 r = normalize( reflect( -light, normal ));

	vec4 color = texture(mytexture, texCoords);
	vec4 color2 = texture(overlayTex, texCoordsTrue);
	color = mix(color, color2, color2.a);
	
	gl_FragData[0] = color;
	gl_FragData[1] = vec4(normal*vec3(-1,-1,1), 0);
	// vec3 sun_color = vec3( 0.6,0.7,0.7 );
	// vec3 sun_color = vec3(0.99, 0.98, 0.91);

	// float ndotl = clamp(dot(normal, light), 0.3, 1.0);

	// float spec = pow(max(0.05, dot(r, eye)), 64.0)*3;
	
	// color = clamp(ndotl*color*sun_color*2.3 + spec*sun_color*dot(color,color), 0.0, 1.0);
	// out_Color = vec4(color, 1.0); 
 
}
