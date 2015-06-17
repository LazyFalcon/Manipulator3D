 #version 330

out vec4 outColor;
in vec2 texCoords;

uniform sampler2D normalTexture;
// uniform sampler2D depthTexture;
uniform sampler2D ambientTexture;
uniform float ambient;
uniform float timeNormalized;
uniform vec4 lightVec;


void main(void){	
	vec4 normal = texture(normalTexture, texCoords);
	float lambert = clamp( dot(normalize(lightVec), normal ),0.2,1.0); 
	vec4 color = texture(ambientTexture, vec2(lambert,timeNormalized));
	// float depth = texture(depthTexture, texCoords).r;
	// depth *= 100;
	
	
	// outColor = lambert*2.3*vec4(0.99, 0.98, 0.85,1);
	outColor = lambert*2.3*color;
}
