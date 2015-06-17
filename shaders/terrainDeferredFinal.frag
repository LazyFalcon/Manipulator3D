#version 330
#extension GL_ARB_shading_language_420pack: enable


in vec2 texCoords;
//normalne terenu
layout(binding=0)uniform sampler2D tPosition;//red
layout(binding=1)uniform sampler2D tNormals;//green
layout(binding=2)uniform sampler2D red;//blue
layout(binding=3)uniform sampler2D green;//alpha

out vec4 out_Color;

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

void main(void){
	vec4 V = texture(tPosition, texCoords);
	vec4 N = texture(tNormals, texCoords);
	
	vec4 L = normalize(vec4(0.5,1,3,0));
	vec4 E = normalize( vec4(0,0,0,1) - V);
	vec4 R = normalize( reflect( -L, N));
	vec4 sun_color = vec4( 1,1,1,1);
	
	float slope = N.z;
	vec4 white = vec4( 0.8, 0.8, 0.8, 0.0);
	// vec2 uv = fract(in_texCoords*250)*0.49+0.01;
	vec2 uv = fract(V.xy)*0.49+0.01;


	vec4 t1g = texture(red, uv);
	vec4 t2g = texture(red, uv+vec2(0.5,0));
	vec4 t3g = texture(red, uv+vec2(0,0.5));
	vec4 t4g = texture(red, uv+vec2(0.5,0.5));
	vec4 color;
	color = heightBlend(t1g, t2g, slopeBased(slope, 0.8, 1, 0.1), 0.1);
	color = heightBlend(color, t3g, slopeBased(slope, 0.7, 0.8, 0.1), 0.1);
	color = heightBlend(color, t4g, slopeBased(slope, 0.5, 0.75, 0.1), 0.1);
			
	if(slope < 0.4){
	// triplanar: //ska³a
	float total = 1;//dot(N,vec4(1,1,1,1));
	float tighten = 0.4679;
	
	float mxy = abs(N.z/total);
	float mxz = abs(N.y/total);
	float myz = abs(N.x/total);
	
	
	
	vec4 t4;
	vec4 worldCoords2 = fract(V*100)*0.45+vec4(1,1,1,0)*0.01;
	vec4 t4xy = texture(green, worldCoords2.xy);
	t4 = t4xy*mxy;
	vec4 t4xz = texture(green, worldCoords2.xz);
	t4 += t4xz*mxz;
	vec4 t4yz = texture(green, worldCoords2.yz);
	t4 += t4yz*myz;
	t4.a = (t4xy.a + t4xz.a + t4yz.a)/3;
	
	color = heightBlend(color, t4, slopeBased(slope, 0.0, 0.3, 0.1), 0.1);
	}
	float sunLight = dot(N, L);
	float spec = pow(max(0.05, dot(R, E)), 64.0)*3;
	
	float ndotl = 0.4;
	color = clamp((ndotl+sunLight)*color + spec*sun_color*dot(color,color), 0.0, 1.0); 
 
	// out_Color = vec4(position.rgb, 1.0); 
	// out_Color = normal;
	// out_Color = color;
	out_Color = fract(V);
	// out_Color = vec4(0.5, 0.2, 0.8, 1.0); 
}
