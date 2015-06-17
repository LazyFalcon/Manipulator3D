 #version 330

out vec4 outColor;
in vec2 texCoords;

uniform sampler2D colorTexture;
uniform sampler2D depthTexture;
uniform sampler2D ambientTexture;

uniform mat4 invPV;

uniform vec3 v3CameraPos;
uniform vec3 v3LightDir;
uniform vec3 waveLength;
uniform float rayleighConstant;
uniform float mieConstant;
uniform	float rayleighConstantSun;
uniform	float mieConstantSun;
uniform float skyRadius;
uniform float groundRadius;
uniform float fScaleDepth;
const int samples = 5;



in vec4 persPos;
float scaleFactor(float angle){
	float x = 1.0 - angle;
	return fScaleDepth * exp(-0.00287 + x*(0.459 + x*(3.83 + x*(-6.80 + x*5.25))));
}



void main(void){	
	vec3 viewSpace = persPos.xyz / persPos.w;
	
	vec4 colorFromTex = texture(colorTexture,texCoords);
	viewSpace.z = texture(depthTexture, texCoords).r;
	// depth *= 100;
	
	vec4 worldPos = invPV*vec4(viewSpace, 1);
	worldPos.xyz /= worldPos.w;
	worldPos.w = 1;
	vec4 rayleighColor = vec4(1);
	vec4 mieColor = vec4(1);
	
		float scale = 1.0 / (skyRadius*7500 - groundRadius*7500);
		float scaleOverScaleDepth = scale/fScaleDepth;
		vec3 cameraPosition = vec3(v3CameraPos.xy,v3CameraPos.z+groundRadius*7500);
		
		// Get the ray from the camera to the vertex, and its length (which is the far point of the ray passing through the atmosphere)
		vec3 pos = worldPos.xyz; 
		pos.z += groundRadius*7500;
		vec3 ray = pos - cameraPosition;
		float far = length(ray);
		ray /= far;
		
		// Calculate the ray's starting position, then calculate its scattering offset
		vec3 start = cameraPosition;
		float height = length(start);
		//fajne kolory jak height *= 0.5;
		float depth = exp(scaleOverScaleDepth * (groundRadius*7500 - cameraPosition.z));
		float startAngle = dot(ray, start) / height;
		float startOffset = depth*scaleFactor(startAngle);
		// Initialize the scattering loop variables
		float sampleLength = far / samples;
		float scaledLength = sampleLength * scale*0.5;
		vec3 sampleRay = ray * sampleLength;
		vec3 samplePoint = start + (sampleRay * 0.5);
		
		// Now loop through the sample rays
		vec3 color = vec3(0); 
		for(int i = 0; i < samples; i++)	{
			float height = length(samplePoint) ;
			float depth = exp(scaleOverScaleDepth * (groundRadius*7500 - height));
			float lightAngle = dot(v3LightDir, samplePoint) / height;
			float cameraAngle = dot(ray, samplePoint) / height;
			// float scatter = 0*(startOffset + depth*(scaleFactor(lightAngle) - scaleFactor(cameraAngle)));
			float scatter = 0;
			vec3 attenuate = exp(-scatter * (waveLength * rayleighConstant + mieConstant));
			color += attenuate * (depth * scaledLength);
			samplePoint += sampleRay;
		}
		// Finally, scale the Rayleigh and Mie colors and set up the varying variables for the pixel shader
		// rayleighColor.xyz = color * waveLength * rayleighConstantSun;
		rayleighColor.xyz = color * waveLength * rayleighConstantSun;
		mieColor.xyz = color * mieConstantSun;
	 
	
	// outColor = rayleighColor + colorFromTex*mieColor;
	outColor = rayleighColor + colorFromTex;
	// outColor =  rayleighColor;
	// outColor =  vec4(0);
	// outColor = mieColor;
}
