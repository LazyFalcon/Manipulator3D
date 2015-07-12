#ifdef COMPILING_VERTEX_SHADER

layout(location=0)in vec4 quadPosition;

out vec2 p_uv;

void main(){
	gl_Position = (vec4(quadPosition.xy,0,1));
	p_uv = (quadPosition.xy+vec2(1,1))/2;
}


#endif

#ifdef COMPILING_FRAGMENT_SHADER
out vec4 out_sobel;

uniform sampler2D u_normalTex;
uniform sampler2D u_depthTex;

uniform mat4 u_invPV;
uniform mat4 u_view;

in vec2 p_uv;

const vec2 sampleRadius = 1/vec2(1400, 720);


float getDepth(vec2 uv){
	return texture(u_depthTex, uv).r*1.7;
	// float depth = texture(depthTex, uv).r*2 -1;
	// depth = (0.1 * 50) / (depth * 49.9 - 50);
	// return depth/50;
}
vec4 getNormal(vec2 uv){
	return texture(u_normalTex, uv);
}

vec2 kernel[10] = vec2[]
(
	vec2(1,1),

	vec2(1,0),
	vec2(0,1),
	vec2(0,0),
	vec2(-1,0),

	vec2(-1,-1),
	vec2(1,1),
	vec2(-1,1),
	vec2(1,-1),
	vec2(0,0)

);

void main(void){

	vec4 n0 = getNormal(p_uv + kernel[0]  * sampleRadius);
	vec4 n1 = getNormal(p_uv + kernel[1]  * sampleRadius);
	vec4 n2 = getNormal(p_uv + kernel[2]  * sampleRadius);
	vec4 n3 = getNormal(p_uv + kernel[3]  * sampleRadius);

	vec2 n_x = 3 * vec2((n3.x + n2.x) - (n1.x + n0.x), (n1.x + n3.x) - (n0.x + n2.x));
	vec2 n_y = 3 * vec2((n3.y + n2.y) - (n1.y + n0.y), (n1.y + n3.y) - (n0.y + n2.y));
	vec2 n_z = 3 * vec2((n3.z + n2.z) - (n1.z + n0.z), (n1.z + n3.z) - (n0.z + n2.z));

	float d1 = getDepth(p_uv + kernel[1]  * sampleRadius*0.5);
	float d2 = getDepth(p_uv + kernel[2]  * sampleRadius*0.5);
	float d3 = getDepth(p_uv + kernel[3]  * sampleRadius*0.5);
	float d4 = getDepth(p_uv + kernel[4]  * sampleRadius*0.5);
	float d5 = getDepth(p_uv + kernel[5]  * sampleRadius*0.5);

	// vec2 d_xy = 1000 * vec2((d3 + d2) - (d1 + d0), (d1 + d3) - (d0 + d2));
	vec2 d_xy = 400 * vec2((d3 - d1) - (d4 - d3), (d2 - d3) - (d3 - d5));
	// vec2 d_xy = 400 * vec2((d1 - d5) - (d5 - d2), (d3 - d5) - (d5 - d4));

	float sobel = clamp((length(n_x) + length(n_y) + length(n_z))-1.0, 0, 1);
	sobel += clamp(length(d_xy)-0.15, 0, 1)*100;
	out_sobel = vec4(1- sobel);

}

#endif

