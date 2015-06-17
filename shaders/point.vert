#version 330

layout(location=0)in vec4 vec;

uniform mat4 projection;
uniform vec4 info;



void main(){
	gl_PointSize = info.z;
	gl_Position = projection*vec4(vec.xy+info.xy, 1, 1);
}
