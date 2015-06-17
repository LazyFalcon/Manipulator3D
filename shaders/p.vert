#version 330

// 24576-number of vertices of terrain


layout(location=0)in vec2 position;
layout(location=1)in vec2 uv;

uniform mat4 matrices[3];

out vec2 texCoords;


void main(){
	if(gl_VertexID<24576)gl_Position=matrices[0]*vec4(position, 1.0, 1.0);
	if(gl_VertexID>=24576 && gl_VertexID<24576+6 )gl_Position=vec4(position, 1.0, 1.0);
	if(gl_VertexID>=24576+6 && gl_VertexID<24576+12 ) gl_Position=matrices[1]*vec4(position, 1.0, 1.0);
	if(gl_VertexID>=24576+12 && gl_VertexID<24576+18 ) gl_Position=matrices[2]*vec4(position, 1.0, 1.0);
	texCoords=uv;
}



/*
zrobiæ dwa vbo, jedno statyczne, zawierajace teren i to co siê nie zmienia
drugie dynamiczne na to co siê zmienia, to drugie niech zawiera particle(napisy, pociski)

DO ZROBIENIA!!!!!!!!!!!!!!
	wyswietlanie napisów
	animacje
	cz¹steczki
	poruszanie wieloma obiektami usprawniæ
	
*/


	// if(ID==-1)gl_Position=vec4(position, 1.0, 1.0);
	// if(ID==0) gl_Position=matrices[0]*vec4(position, 1.0, 1.0);
	// if(ID==1) gl_Position=matrices[1]*vec4(position, 1.0, 1.0);
	// texCoords=uv;
// }

/*28.06.2012
napisy przekazaæ jako uniform;
wymiary ekranu te¿
w VBO(statycznym) trzymaæ same -1ki
korzystaj¹c z gl_VertexID rozsuwaæ wierzcho³ki
*/