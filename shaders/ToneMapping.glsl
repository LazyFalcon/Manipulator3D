#ifdef COMPILING_VERTEX_SHADER

layout(location=0)in vec4 mVertex;

out vec2 vUV;

void main(){
	gl_Position = (vec4(mVertex.xy,0,1));
	vUV = (mVertex.xy+vec2(1,1))/2;
}

#endif

#ifdef COMPILING_FRAGMENT_SHADER

out vec4 outColor;

in vec2 vUV;

uniform sampler2D uTexture;


float luminance(vec4 color) {
    return dot(color.rgb, vec3(0.2126, 0.7152, 0.0722));
}

float a = 8.0;
float lmax = 16.0;
const float epsilon = 0.0001;

float lumCompressed(float lavg, float l) {
    float ls = (l * a) / (lavg + epsilon);
    return (ls * (1 + (ls / (lmax*lmax)))) / (1 + ls);
}


void main(void){
	vec4 color = texture(uTexture, vUV);
	float l = luminance(color);
	// float l = length(color);
	// float lavg = texture(sampler, vec2(0.5, 0.5)).r;
	float lavg = 1;

	// outColor = vec4(lumCompressed(lavg, l) * color.rbg, 1.0);
	outColor = color/5;
}

#endif

