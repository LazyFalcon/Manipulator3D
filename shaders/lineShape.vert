#version 330

layout(location=0)in vec4 Vertex;
   
uniform mat4 projection;
uniform float size;
uniform vec2 start;
uniform vec2 end;

out float yPos;

void main(){
	
	float len = distance(start, end);
	
	vec4 newX = vec4(normalize(end - start), 0, 0);
	vec4 newY = newX.yxzw;
	newY.y *= -1;
	
	vec4 newVertex = Vertex.x*newX*len + Vertex.y*newY*size;
	
	yPos = Vertex.y;
	gl_Position = projection*vec4(newVertex + vec4(start,0,1));

}
