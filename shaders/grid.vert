#version 330



layout(location=0)in vec4 position;

uniform mat4 viewProjection;


void main(){
	gl_Position = viewProjection*position;
	// gl_Position = vec4(position.xy*0.1, 1,1);
}

