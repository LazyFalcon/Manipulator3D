#version 330
uniform sampler2D mytexture;
uniform mat4 projection;

out vec4 out_Color;

// out float gl_FragDepth; //[0, 1]
in float radius;
in float alpha_factor;
in vec4 pos;// position after transformations
in vec2 uv;
in vec2 uv2;
in vec2 mapping;

void main(void){
	vec4 color = texture2D(mytexture, uv2);
	// vec2 coords = uv*2*radius - vec2(1,1)*radius;
	// float r = dot(coords,coords);
	// float unitR = dot(mapping, mapping)*4;
	// float d = sqrt(radius*radius - r);
	// vec4 f = vec4(pos.xy,pos.z + d, 1);
		
	// f = projection * f;
	
	// gl_FragDepth = f.z/f.w * 0.5 + 0.5;
	color.a *= 0.9;
	out_Color = color;
	
}
