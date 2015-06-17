#version 330

layout(location=0)in vec3 vertex;
layout(location=1)in vec2 uv;
layout(location=2)in vec3 normals;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform vec3 v3LightDir;
uniform vec3 waveLength;
uniform float rayleighConstant;
uniform float mieConstant;
uniform	float rayleighConstantSun;
uniform	float mieConstantSun;
uniform float skyRadius;
uniform float groundRadius;
uniform float fScaleDepth;
const int samples = 10;


out vec3 cameraDirection;
out vec4 rayleighColor;
out vec4 mieColor;

// Scale function
float scaleFactor(float angle){
	float x = 1.0 - angle;
	return fScaleDepth * exp(-0.00287 + x*(0.459 + x*(3.83 + x*(-6.80 + x*5.25))));
}

void main(){
	// vec3 lightDir = normalize(vec3(10,2,2));
	vec3 lightDir = v3LightDir;
	// Calculate scale variables
	float scale = 1.0 / (skyRadius - groundRadius);
	float scaleOverScaleDepth = scale/fScaleDepth;
	vec3 cameraPosition = vec3(0.0f,0.0f,groundRadius+0.1);
	
	// Get the ray from the camera to the vertex, and its length (which is the far point of the ray passing through the atmosphere)
	vec3 pos = vertex.xyz * groundRadius; 
	pos.z += 10.0;
	vec3 ray = pos - cameraPosition;
	float far = length(ray);
	ray /= far;
	
	// Calculate the ray's starting position, then calculate its scattering offset
	vec3 start = cameraPosition;
	float height = length(start);
	//fajne kolory jak height *= 0.5;
	float depth = exp(scaleOverScaleDepth * (groundRadius - cameraPosition.z));
	float startAngle = dot(ray, start) / height;
	float startOffset = depth*scaleFactor(startAngle);
	// Initialize the scattering loop variables
	float sampleLength = far / samples;
	float scaledLength = sampleLength * scale;
	vec3 sampleRay = ray * sampleLength;
	vec3 samplePoint = start + (sampleRay * 0.5);
	
	// Now loop through the sample rays
	vec3 color = vec3(0.0f, 0.0f, 0.0f); 
	for(int i = 0; i < samples; i++)	{
		float height = length(samplePoint) ;
		float depth = exp(scaleOverScaleDepth * (groundRadius - height));
		float lightAngle = dot(lightDir, samplePoint) / height;
		float cameraAngle = dot(ray, samplePoint) / height;
		float scatter = (startOffset + depth*(scaleFactor(lightAngle) - scaleFactor(cameraAngle)));
		vec3 attenuate = exp(-scatter * (waveLength * rayleighConstant + mieConstant));
		color += attenuate * (depth * scaledLength);
		samplePoint += sampleRay;
	}
	// Finally, scale the Rayleigh and Mie colors and set up the varying variables for the pixel shader
	rayleighColor.xyz = color * waveLength * rayleighConstantSun;
	mieColor.xyz = color * mieConstantSun;
	cameraDirection = cameraPosition - pos;

	mat4 tmp = projection*view*model;
	vec4 position = tmp*vec4(vertex+vec3(0,0,0.07), 1.0);
	gl_Position = position.xyww;
}

