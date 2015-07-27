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

uniform sampler2D uNormalBuffer;
uniform sampler2D uDepthBuffer;
uniform sampler2D uSSAORandom;

uniform mat4 uInvPV;
uniform mat4 uView;

in vec2 vUV;

uniform vec2 camerarange = vec2(0.10, 50.0);

float pw = 1.0/1400.0*0.5;
float ph = 1.0/720.0*0.5;

float readDepth(in vec2 coord){
	if (coord.x<0||coord.y<0) return 1.0;
	float nearZ = camerarange.x;
	float farZ =camerarange.y;
	float posZ = texture(uDepthBuffer, coord).x;
	return (2.0 * nearZ) / (nearZ + farZ - posZ * (farZ - nearZ));
}

float compareDepths(in float depth1, in float depth2){
	float gauss = 0.0;
	float diff = (depth1 - depth2)*100.0; //depth difference (0-100)
	float gdisplace = 0.2; //gauss bell center
	float garea = 3.0; //gauss bell width

	//reduce left bell width to avoid self-shadowing
	if (diff<gdisplace) garea = 0.2;

	gauss = pow(2.7182,-2*(diff-gdisplace)*(diff-gdisplace)/(garea*garea));

	return max(0.2,gauss);
}

vec4 calAO(float depth, float dw, float dh, inout float ao){
	float temp = 0;
	vec4 bleed = vec4(1);
	float coordw = vUV.x + dw/depth*10;
	float coordh = vUV.y + dh/depth*10;

	if (coordw  < 1.0 && coordw  > 0.0 && coordh < 1.0 && coordh  > 0.0){

	vec2 coord = vec2(coordw , coordh);
	temp = compareDepths(depth, readDepth(coord));

	}
	ao += temp;
	return temp*bleed;
}

void main(void){
 //randomization texture:
 vec2 fres = vec2(20);
 vec4 random = texture(uSSAORandom, vUV.st*fres.xy);
 random = random*2.0-vec4(1.0);

 //initialize stuff:
 float depth = readDepth(vUV);
 vec4 gi = vec4(0);
 float ao = 0.0;

 for(int i=0; i<8; ++i){
	 //calculate color bleeding and ao:
	 gi += calAO(depth,  pw, ph,ao);
	 gi += calAO(depth,  pw, -ph,ao);
	 gi += calAO(depth,  -pw, ph,ao);
	 gi += calAO(depth,  -pw, -ph,ao);

	 //sample jittering:
	 pw += random.x*0.0005;
	 ph += random.y*0.0005;

	 //increase sampling area:
	 pw *= 1.4;
	 ph *= 1.4;
 }

 //final values, some adjusting:
 vec4 finalAO = vec4(1.0-(ao/32.0));
 vec4 finalGI = (gi/32)*0.6;

 outAO = vec4(finalAO+finalGI);
}


#endif

