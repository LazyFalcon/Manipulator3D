#version 330

uniform float specta;

out vec4 out_Color;
in vec2 texCoords;
// in vec3 Normal;
// in vec3 sunDir;
// in vec3 eyeDir;
// in vec3 spotDir;
// in float sunLight;
// in vec3 spotVector;

uniform sampler2D mytexture;

void main(void){
	// vec3 eye = 		normalize( eyeDir);
	// vec3 spot_dir = 	normalize( spotDir);
	// vec3 reflect =	normalize(reflect( spot_dir, eye));
	vec4 color =	texture2D(mytexture, texCoords);
	// red.a = 0.1;
	if(color.a < 0.5)
		discard;
	// vec3 normal = normalize( vec3(Normal.x+red.x, Normal.y+red.y, Normal.z+red.z)); 
	// const vec3 white = vec3( 1,1,1);
	// float tmp = dot(normalize(spotVector), spot_dir);//efekt latarki
	// Compute a simple phong illumination with a directional light
	// vec3 color = red.xyz;
	// float ndotl = 0.4;
	// float spec = clamp(pow(sunLight, 20),0.0,0.1);
	// float dista = length(spotVector);
	// if(tmp>0.95 && dista<50.0){
		// ndotl=dot(normal, spot_dir);
		// ndotl=max(min(max( ndotl, 0.0)+0.15, 1.5)*red[0]*pow(tmp,8)*5, 0);
		// spec=pow(max(0.1, dot(reflect, spot_dir)), 20.0)*red[0]*specta*5;
	// }
	
	// color = clamp((ndotl)*red, 0.0, 1.0);
	// color = vec3(1,0.25, 0.75);
	out_Color = vec4(color*0.5); 
}
