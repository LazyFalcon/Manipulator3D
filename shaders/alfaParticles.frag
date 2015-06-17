#version 330
uniform sampler2D texture;
uniform mat4 projection;
uniform vec3 colorMin;
uniform vec3 colorMax;


in vec2 quadPos;// position in quad
in vec4 globalPos;// position in world
in vec2 uv;
in vec2 mapping;

out vec4 out_Color;

void main(void){
	vec4 color = texture2D(texture, uv);
	float r = dot(quadPos, quadPos);
	if(color.r < 0.5)
		discard;
	out_Color = vec4(mix(colorMin, colorMax, r), color.r);
	// out_Color = vec4(color.xyz, color.r);
}
