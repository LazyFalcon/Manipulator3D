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


vec4 heightBlend(vec4 tex1, vec4 tex2, float t, float depth){

	// float ma = max(tex1.a + (1-t), tex2.a + t) - depth;

	// float b1 = max(tex1.a + (1-t) - ma, 0);
	// float b2 = max(tex2.a + t - ma, 0);

	// return vec4((tex1.rgb*b1 + tex2.rgb*b2) / (b1+b2), max(tex1.a, tex2.a) );

	// return tex1.a+(1-t)+depth > tex2.a+t -depth ? tex1 : tex2;
	return tex1*(1-t) +  tex2*t;
	// return tex1.a+(1-t) > tex2.a+t  ? tex1 : tex2;
}

float slopeBased(float slope, float min, float max, float offset){
	if(slope < min+offset/2)
		return clamp(  (slope-min+offset/2)/offset, 0,1);
	else if(slope > max-offset/2)
		return clamp((1-(slope+1-(max+offset/2) ))/offset,0,1);
	else 
		return 1.0;
}

float changeRange(float val, float mn, float mx){
	val = (val- mn)/(mx-mn);
	return clamp(val, 0,1);
}

void main(void){
	
	vec4 V = Vertex;
	vec4 N = normalize(Normal);
	
	vec2 uv = in_texCoords*170;

	float val = texture(GrassPattern, uv/15).r*5;
	float layer = floor(val);
	float layer2 = ceil(val);
	
	vec4 color = texture2DArray(grounds, vec3(uv,layer));
	vec4 color2 = texture2DArray(grounds, vec3(uv,layer2));
	color = mix(color, color2, changeRange(fract(val), 0.25, 0.75));
	color.a = 1; 
	// gl_FragData[0] = color*vec4(1+depth/1000,1+depth/1000,1+depth/700,1);
	gl_FragData[0] = color;
	gl_FragData[1] = vec4(N.xyz, 0);

}
