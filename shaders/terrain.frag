#version 330
#extension GL_ARB_shading_language_420pack: enable
#extension GL_EXT_texture_array : enable


out vec4 out_Color;
in vec2 in_texCoords;
in vec4 Normal;
in vec4 Vertex;
in vec4 worldCoords;
in float depth;

layout(binding=1)uniform sampler2DArray grounds;
layout(binding=2)uniform sampler2D GrassPattern;

float changeRange(float val, float mn, float mx){
	val = (val- mn)/(mx-mn);
	return clamp(val, 0,1);
}

void main(void){
	
	vec4 V = Vertex;
	vec4 N = normalize(Normal);
	vec4 L = -normalize(vec4(0.64,-0.62,0.5,0));
	vec4 E = normalize( vec4(0,0,0,1) - V);
	vec4 R = normalize( reflect( -L, N));
	vec4 sun_color = vec4(0.99, 0.98, 0.91,1);
	vec2 uv = in_texCoords*170;

	
	// float val = texture(GrassPattern, in_texCoords*11)*5;
	float val = texture(GrassPattern, in_texCoords*16)*4;
	float layer = floor(val);
	float layer2 = ceil(val);
	
	vec4 color = texture2DArray(grounds, vec3(uv,layer));
	vec4 color2 = texture2DArray(grounds, vec3(uv,layer2));
	// color = mix(color, color2, changeRange(fract(val), 0.2, 0.8));
	color = mix(color, color2, changeRange(fract(val), 0.48, 0.52));
	color.a = 1; 
	// color = vec4(layer/4, layer/4.65, layer/4.58, 1);

	
	float ndotl = 0.2;
	float sunLight = clamp(dot(N, L), ndotl, 1);
	float spec = pow(max(0.05, dot(R, E)), 64.0)*3*0;
	// color = t1g;
	color = clamp(sunLight*color*sun_color*4.3 + spec*sun_color*dot(color,color), 0.0, 1.0);

	// out_Color = vec4(color.rgb*vec3(1+depth/1000,1+depth/1000,1+depth/700), 1.0);
	out_Color = vec4(color.rgb, 1.0);
}
