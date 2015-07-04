#version 330

out vec4 out_sobel;

uniform sampler2D normalTex;
uniform sampler2D depthTex;

uniform mat4 invPV;
uniform mat4 View;

in vec2 texCoords;

const vec2 sampleRadius = 1/vec2(1400, 720);


float getDepth(vec2 uv){
	return texture(depthTex, uv).r;
	// float depth = texture(depthTex, uv).r*2 -1;
	// float depth = texture(depthTex, uv).r;
	// depth = (0.1 * 50) / (depth * 49.9 - 50);
	// return depth/50;
}
vec4 getNormal(vec2 uv){
	return texture(normalTex, uv);
}

vec2 kernel[6] = vec2[]
(
	vec2(1,1),

	vec2(1,0),
	vec2(0,1),
	vec2(0,0),
	vec2(-1,0),
	vec2(0,-1)

);

void main(void){

	vec4 n0 = getNormal(texCoords + kernel[0]  * sampleRadius);
	vec4 n1 = getNormal(texCoords + kernel[1]  * sampleRadius);
	vec4 n2 = getNormal(texCoords + kernel[2]  * sampleRadius);
	vec4 n3 = getNormal(texCoords + kernel[3]  * sampleRadius);

	vec2 n_x = 3 * vec2((n3.x + n2.x) - (n1.x + n0.x), (n1.x + n3.x) - (n0.x + n2.x));
	vec2 n_y = 3 * vec2((n3.y + n2.y) - (n1.y + n0.y), (n1.y + n3.y) - (n0.y + n2.y));
	vec2 n_z = 3 * vec2((n3.z + n2.z) - (n1.z + n0.z), (n1.z + n3.z) - (n0.z + n2.z));

	float d1 = getDepth(texCoords + kernel[1]  * sampleRadius);
	float d2 = getDepth(texCoords + kernel[2]  * sampleRadius);
	float d3 = getDepth(texCoords + kernel[3]  * sampleRadius);
	float d4 = getDepth(texCoords + kernel[4]  * sampleRadius);
	float d5 = getDepth(texCoords + kernel[5]  * sampleRadius);

	// vec2 d_xy = 1000 * vec2((d3 + d2) - (d1 + d0), (d1 + d3) - (d0 + d2));
	vec2 d_xy = 400 * vec2((d3 - d1) - (d4 - d3), (d2 - d3) - (d3 - d5));

	float sobel = clamp((length(n_x) + length(n_y) + length(n_z))-1.0, 0, 1);
	sobel += clamp(length(d_xy)-0.4, 0, 1)*100;
	out_sobel = vec4(1- sobel);

}

