#ifdef COMPILING_VERTEX_SHADER

layout(location=0)in vec4 mVertex;

out vec2 vUV;

void main(){
	gl_Position = (vec4(mVertex.xy,0,1));
	vUV = (mVertex.xy+vec2(1,1))/2;
}


#endif

/// TODO: dodaæ zanikanie wartoœci wraz z odleg³oœci¹, mo¿na wtedy podbiæ wartoœci

#ifdef COMPILING_FRAGMENT_SHADER
out vec4 outAO;

uniform sampler2D uDepthBuffer;
uniform sampler2D uSSAORandom;
uniform float uWidth;
uniform float uHeight;

in vec2 vUV;
const vec2 camerarange = vec2(0.10, 50.0);

float pw = 1.0/1400.0*0.5;
float ph = 1.0/720.0*0.5;
const vec2 screen = vec2(1400, 720);

float readDepth(in vec2 coord){
	// if (coord.x<0||coord.y<0) return 1.0;
	float nearZ = camerarange.x;
	float farZ = camerarange.y;
	float posZ = texture(uDepthBuffer, coord).x;
	return (2.0 * nearZ) / (nearZ + farZ - posZ * (farZ - nearZ));
}

float compareDepths(in float depth1, in float depth2){
	float gauss = 0.0;
	float diff = (depth1 - depth2)*100.0; //depth difference (0-100)
	float gdisplace = 0.5; //gauss bell center
	float garea = 0.50; //gauss bell width

	//reduce left bell width to avoid self-shadowing
	if (diff<gdisplace) garea = 0.2;

	gauss = pow(2.7182,-2*(diff-gdisplace)*(diff-gdisplace)/(garea*garea));

	return max(0.2, gauss);
}

float calAO(float depth, vec2 uv,  float dw, float dh, inout float ao){
	float temp = 0;
	float coordw = vUV.x + (uv.x + dw)/depth*0.5;
	float coordh = vUV.y + (uv.y + dh)/depth*0.5;

	vec2 coord = vec2(coordw , coordh);
	temp = compareDepths(depth, readDepth(coord));

	ao += temp*1.0;
	return temp;
}
const vec2 kernel[16] = vec2[]
(
	// vec2(  0.5244396,  0.6552907 ),
	// vec2(  0.7802591,  0.1212579 ),
	// vec2( -0.1709561,  0.9723814 ),
	// vec2(  0.08371004, 0.3698372 ),
	// vec2(  0.1981699, -0.0234299 ),
	// vec2( -0.2241783, -0.4910517 ),
	// vec2(  0.615613,  -0.6265193 ),
	// vec2( -0.5282364,  0.02899032 ),
	// vec2(  0.9069752, -0.2703182 ),
	// vec2( -0.3698249,  0.3978794 ),
	// vec2( -0.7183862, -0.6849706 ),
	// vec2( -0.7184948,  0.6608582 ),
	// vec2( -0.0925712, -0.9822884 ),
	// vec2(  0.20099,   -0.4438035 ),
	// vec2( -0.9637656,  0.1957591 ),
	// vec2( -0.8576232, -0.262913 )
	vec2( -0.94201624,  -0.39906216 ),
	vec2(  0.94558609,  -0.76890725 ),
	vec2( -0.094184101, -0.92938870 ),
	vec2(  0.34495938,   0.29387760 ),
	vec2( -0.91588581,   0.45771432 ),
	vec2( -0.81544232,  -0.87912464 ),
	vec2( -0.38277543,   0.27676845 ),
	vec2(  0.97484398,   0.75648379 ),
	vec2(  0.44323325,  -0.97511554 ),
	vec2(  0.53742981,  -0.47373420 ),
	vec2( -0.26496911,  -0.41893023 ),
	vec2(  0.79197514,   0.19090188 ),
	vec2( -0.24188840,   0.99706507 ),
	vec2( -0.81409955,   0.91437590 ),
	vec2(  0.19984126,   0.78641367 ),
	vec2(  0.14383161,  -0.14100790 )
);
void main(void){
 //randomization texture:
 // vec2 fres = vec2(20);
 // vec4 random = texture(uSSAORandom, vUV*fres.xy);
 // random = random*2.0-vec4(1.0);

 //initialize stuff:
 float depth = readDepth(vUV);
 float gi = 0;
 float ao = 0.0;

 	const int iterations = 16;
	for(int i=0; i<iterations; ++i){
	 // gi += calAO(depth,kernel[i]/screen, pw, ph, ao);
	 // gi += calAO(depth,kernel[i]/screen, pw, -ph, ao);
	 // gi += calAO(depth,kernel[i]/screen, -pw, ph, ao);
	 // gi += calAO(depth,kernel[i]/screen, -pw, -ph, ao);
	 gi += calAO(depth,vec2(0),  pw, ph,ao);
	 gi += calAO(depth,vec2(0),  pw, -ph,ao);
	 gi += calAO(depth,vec2(0),  -pw, ph,ao);
	 gi += calAO(depth,vec2(0),  -pw, -ph,ao);

	 // sample jittering:
	 // pw += random.x*0.0005;
	 // ph += random.y*0.0005;

	 //increase sampling area:
	 pw *= 1.4;
	 ph *= 1.4;

	}
	float finalAO = 1.0-(ao/32.0);
	float finalGI = (gi/32)*0.6;

	outAO = vec4(finalAO+finalGI);
}


#endif

