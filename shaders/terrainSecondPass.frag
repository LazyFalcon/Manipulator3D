#version 330
#extension GL_ARB_shading_language_420pack: enable


out vec4 out_Color;
in vec2 in_texCoords;
in vec4 Normal;
in vec4 Vertex;
in vec4 worldCoords;

layout(binding=1)uniform sampler2DArray grounds;


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
	
	vec4 V = Vertex;
	vec4 N = normalize(Normal);
	// vec4 L = normalize(vec4(0,0,2000,1) - V);
	// vec4 L = normalize(vec4(0.5,1,3,0));
	vec4 L = -normalize(vec4(0.64,-0.62,0.5,0));
	vec4 E = normalize( vec4(0,0,0,1) - V);
	vec4 R = normalize( reflect( -L, N));
	// vec4 sun_color = vec4( 1,1,1,1);
	vec4 sun_color = vec4(0.99, 0.98, 0.91,1);
	
	float slope = N.z;
	vec4 white = vec4( 0.8, 0.8, 0.8, 0.0);
	// vec2 uv = fract(in_texCoords*50);//*0.49+0.01;
	vec2 uv = in_texCoords*70;//*0.49+0.01;
	vec4 color;
			
	if(slope < 0.4){

	float total = 1;//dot(N,vec4(1,1,1,1));
	float tighten = 0.4679;
	
	float mxy = abs(N.z/total);
	float mxz = abs(N.y/total);
	float myz = abs(N.x/total);
	
	
	
	vec4 t4;
	vec4 worldCoords2 = fract(worldCoords*200);
	vec4 t4xy = texture(grounds, vec3(worldCoords2.xy,4));
	t4 = t4xy*mxy;
	vec4 t4xz = texture(grounds, vec3(worldCoords2.xz,4));
	t4 += t4xz*mxz;
	vec4 t4yz = texture(grounds, vec3(worldCoords2.yz,4));
	t4 += t4yz*myz;
	t4.a = (t4xy.a + t4xz.a + t4yz.a)/3;
	
	color = t4; 
	}
	else
		discard;
	float ndotl = 0.3;
	float sunLight = clamp(dot(N, L), ndotl, 1);
	float spec = pow(max(0.05, dot(R, E)), 64.0)*3*0;
	// color = t1g;
	color = clamp(sunLight*color*sun_color*2.3 + spec*sun_color*dot(color,color), 0.0, 1.0);

	out_Color = vec4(color.rgb, 1.0); 
}
