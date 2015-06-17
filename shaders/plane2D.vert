#version 330

layout(location=0)in vec4 vertex;

uniform mat4 projection;
uniform vec4 plane;

out vec2 texCoords;

void main(){
	vec4 newPlane = vec4(vertex.xy*plane.zw + plane.xy, 0.5, 1);

	texCoords = vertex.xy;
	gl_Position = projection*newPlane;
}
