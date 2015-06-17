#version 330

uniform float specta;

out vec4 out_Color;
in vec2 texCoords;
in vec3 Normal;
in vec3 light_dir;
in vec3 eye_dir;
in vec3 lightVector;

uniform sampler2D mytexture;

void main(void){
	vec3 normal = normalize(Normal); 
	vec3 eye = normalize( eye_dir);
	vec3 light = normalize( light_dir);
	vec3 r =normalize(reflect( light, eye));
	const vec3 red =texture2D(mytexture, texCoords).xyz;
	const vec3 white = vec3( 1,0.5,0.5);
	float tmp=dot(normalize(lightVector), light);//efekt latarki
	// Compute a simple phong illumination with a directional light
	vec3 color=red;
	float ndotl;
	float spec=0.01;
	float dista=length(light_dir);
	if(tmp>0.9 && dista<50.0){
		ndotl=dot(normal, light);
		ndotl=max(min(max( ndotl, 0.0)+0.15, 1.5)*red[0]*pow(tmp, 10)*105/dista, 0);
		spec=pow(max(0.1, dot(r, light)), 64.0)*red[0]*specta*50;
	}
	else{
	ndotl=dot(normal, light);
	ndotl=max(min(max( ndotl, 0.0)+0.05, 1), 0)*red[1]/50;
	}
	color=clamp(ndotl*red+ spec*white, 0.0, 1.5);
	out_Color=vec4(color, 1.0); 
}
