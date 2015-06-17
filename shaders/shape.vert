#version 330


layout(location=0)in vec4 vec;

uniform mat4 projection;
uniform vec4 info;

out vec2 dimensions;
out vec2 position;
out vec2 texCoords;

void main(){
	dimensions = info.zw;
	texCoords = vec.xy;
	position = vec2(vec.x*info.z, vec.y*info.w);
	gl_Position = projection*vec4(position+floor(info.xy), 0, 1);

}
