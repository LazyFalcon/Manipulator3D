#ifdef COMPILING_VERTEX_SHADER

layout(location=0)in vec3 Vertex;
layout(location=1)in vec2 UV;
layout(location=2)in vec3 Normal;

uniform mat4  u_projection;
uniform mat4  u_view;
uniform vec4  u_lightPos;
uniform float u_size;

out vec4 p_screenPosition;

void main(){
	gl_Position = u_projection*u_view*(vec4(Vertex*u_size, 0)+u_lightPos);
	p_screenPosition = gl_Position;
}


#endif

#ifdef COMPILING_FRAGMENT_SHADER

out vec4 outColor;

uniform sampler2D u_normalTex;
uniform sampler2D u_depthTex;

uniform vec4 u_color;
uniform float u_size;
uniform mat4 u_invPV;
uniform vec4 u_lightPos;
uniform vec4 u_eyePos;
uniform float u_energy;

in vec4 p_screenPosition;
const float numShades = 7.0;

void main(void){

	vec3 viewSpace = p_screenPosition.xyz / p_screenPosition.w;
	vec2 uv = viewSpace.xy*0.5+0.5;

	vec4 normal = texture(u_normalTex, uv);

	float metal = (normal.w+0.2);
	normal.w = 0;

	viewSpace.z = texture(u_depthTex, uv).r*2 - 1;

	vec4 worldPos = u_invPV*vec4(viewSpace, 1);
	worldPos.xyz /= worldPos.w;
	worldPos.w = 1;

	vec4 lightVec = worldPos-u_lightPos;
	float l = length( lightVec );
	float attenuation = clamp(1.0 - l/(u_size), 0.0, 1.0);
	attenuation *= attenuation;

	float NdotL = clamp( dot(normalize(lightVec), -normal ),0.0,1.0);

	float d = length(lightVec);
	// float shadeIntensity = floor(energy*attenuation*NdotL*numShades)/numShades;
	float shadeIntensity = u_energy*attenuation*NdotL*0.5;
	outColor = u_color*shadeIntensity*0.5;

}


#endif

/// first pass of light - draw to stencil buffer
