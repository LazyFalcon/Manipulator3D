#version 330

layout(location=0)in vec2 position;

uniform vec4 under15[18];
uniform mat4 little_letter_scale[1];
// uniform vec2 over15[];, use texture

out vec2 texCoords;


void main(){
	vec2 tmp;
	vec4	tmp2;
		vec4 letter=under15[gl_InstanceID];
		gl_Position=vec4(position+letter.xy,1.0, 1.0);
		if(gl_VertexID==0)tmp=vec2(0.0 , 980.0/1024.0);
		if(gl_VertexID==1)tmp=vec2( 25.0/1024.0, 980.0/1024.0);
		if(gl_VertexID==2)tmp=vec2(0.0, 1.0);
		if(gl_VertexID==3)tmp=vec2(25.0/1024.0, 980.0/1024.0);
		if(gl_VertexID==4)tmp=vec2(0.0, 1.0);
		if(gl_VertexID==5)tmp=vec2(25.0/1024.0, 1.0);
		
		texCoords=tmp+letter.zw;

}


