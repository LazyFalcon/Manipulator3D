 #version 330

out vec4 outColor;

uniform sampler2D normalTex;
uniform sampler2D depthTex;

uniform vec4 color;
uniform vec3 size;
uniform mat4 invPV;
uniform vec4 position;
uniform vec4 spotDir;
uniform float alpha;

in vec4 persPos;

void main(void){	
	
	vec3 viewSpace = persPos.xyz / persPos.w;
	vec2 uv = viewSpace.xy*0.5+0.5;
	
	vec4 normal = texture(normalTex, uv);
	viewSpace.z = texture(depthTex, uv).r*2 - 1;
	
	vec4 worldPos = invPV*vec4(viewSpace, 1);
	worldPos.xyz /= worldPos.w;
	worldPos.w = 1;
	
	vec4 dir = worldPos-position;
	float angle = dot( normalize(spotDir), normalize(dir));
	float l = length( dir );
	float h = l*angle;
	float dist = 1.0-pow((clamp (h ,0.0,size.y)/size.y),2.5);
	// dist =  1-clamp(1-angle, 0,1-size.x);
	dist *=  1-pow(clamp(1-angle, 0,1-size.x)/(1-size.x), 5);
	float lambert = clamp( dot(normalize(dir), -normal ),0.1,1.0);
	
	
	outColor = color*lambert*dist*angle;
	// outColor = angle;


}
