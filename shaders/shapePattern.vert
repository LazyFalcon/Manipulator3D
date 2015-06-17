#version 330

layout(location=0)in vec4 qP;

uniform mat4 projection;
uniform vec4 pattern;
uniform vec2 info;
//info.x: rotacja

out vec2 texCoords;

void main(){
	
	vec2 pos = vec2(
			qP.x*cos(info.x)*pattern.z - qP.y*sin(info.x)*pattern.w
			, qP.y*cos(info.x)*pattern.w + qP.x*sin(info.x)*pattern.z
		);
	gl_Position = projection*(vec4(pos.xy + pattern.xy,0,1));
	texCoords = (qP.xy+vec2(0.5,0.5));
}

