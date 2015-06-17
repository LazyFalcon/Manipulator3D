#version 330
uniform vec3 v3LightDir;
uniform float scatteringSymmetry;
uniform float scatteringSymmetry2;

in vec3 cameraDirection;
in vec4 rayleighColor;
in vec4 mieColor;

out vec4 fragColor;
const float g = -0.999;
const float g2 = g*g;


// Mie phase function
float getMiePhase(float lightAngle, float lightAngle2, float scatteringSymmetry, float scatteringSymmetry2){
return 1.5*((1.0-scatteringSymmetry2)/(2.0+scatteringSymmetry2))*(1.0+lightAngle2)/pow(1.0+scatteringSymmetry2 - 2.0*scatteringSymmetry*lightAngle, 1.5);
}
// Rayleigh phase function
float getRayleighPhase(float lightAngle2){
return 0.75 * (2.0 + 0.5 * lightAngle2);
}

void main(){
	// vec3 lightDir = normalize(vec3(10,2,2));
	vec3 lightDir = v3LightDir;
	fragColor = vec4(0.0,0.6,0.0,1.0);
	float lightAngle = dot(lightDir, normalize(cameraDirection));
	float lightAngle2 = lightAngle * lightAngle;
	// float miePhase = getMiePhase(lightAngle,lightAngle2,scatteringSymmetry,scatteringSymmetry2);
	float miePhase = 1.1 * ((1.0 - g2) / (2.0 + g2)) * (1.0 + lightAngle2) / pow(1.0 + g2 - 2.0*g*lightAngle, 1.5);
	// float rayleighPhase = getRayleighPhase(lightAngle2);
	float rayleighPhase = 0.75 * (1.0 + lightAngle2);
	fragColor = rayleighPhase*rayleighColor + miePhase* mieColor; 
	fragColor = 1.0-exp(fragColor*-2.0);
}
