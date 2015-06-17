/******************************************************************************
simple normal map shader (HLSL version)
by Ben Cloward
bsmji@hotmail.com
http://www.monitorstudios.com/bcloward

This shader is based on several shaders written by Kevin Bjorke of Nvidia and 
included with the Cg plugin for 3DS Max.  I also borrowed a few ideas from the
DiffuseBump shader that was originally included with 3DS Max 6.

This shader uses a normal map for per-pixel lighting - to give the illusion that
the model surface contains more detail than is really there.  

It accepts the following inputs:

Diffuse Texture
Normal Map Texture
Ambient Color
Point Light Position
Point Light Color

It requires hardware support for DirectX 8.

I created it for use with the tutorial I wrote on creating normal maps:
http://www.monitorstudios.com/bcloward/tutorials_normal_maps1.html

This shader is intended to be used by the DirectX 9 shader of 3DS Max
but it could also be used in other applications that support the FX format.

******************************************************************************/

/************* TWEAKABLES **************/

float4x4 world : World < string UIWidget = "None"; >;
float4x4 WorldIMatrix : WorldInverse < string UIWidget = "None"; >;
float4x4 wvp : WorldViewProjection < string UIWidget = "None"; >;

float4 lightPos : POSITION
<
	string UIName = "Light Position";
	string Object = "PointLight";
	string Space = "World";
	int refID = 0;
> = {100.0f, 100.0f, 100.0f, 0.0f};

float4 lightColor : LIGHTCOLOR
<
	int LightRef = 0;
> = { 1.0f, 1.0f, 1.0f, 0.0f };

float4 ambiColor : Ambient
<
string UIName = "Ambient Color";
> = {0.1f, 0.1f, 0.1f, 1.0f};

texture diffuseTexture : DiffuseMap
<
string name = "seafloor.dds";
string UIName = "Diffuse Texture";
>;

texture normalMap : NormalMap
<
string name = "NMP_Ripples2_512.dds";
string UIName = "Normal Texture";
>;

sampler2D cmap = sampler_state
{
	Texture = <diffuseTexture>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
};

sampler2D normalSampler = sampler_state 
{
	Texture = <normalMap>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
};
/*************/


/************* STRUCTS **************/

// vertex input
struct VertexIn {
	float4 Position	: POSITION; //in object space
	float2 UV		: TEXCOORD0; //in object space
	float4 Normal	: NORMAL; //in object space
	float3 T 		: TANGENT; //in object space
	float3 B 		: BINORMAL; //in object space
};

//vertex output
struct VertexOut {
    float4 HPosition	: POSITION;
	float2 texCoord0	: TEXCOORD0;
	float2 texCoord1 	: TEXCOORD1;
	float3 LightVector	: TEXCOORD2;
};

//pixel output
struct PixelOut {
  float4 col : COLOR;
};
/*************/


//vertex shader*************************************
VertexOut DiffuseBumpVS(VertexIn IN, 
	uniform float4x4 WorldViewProj, 
	uniform float4x4 WorldIMatrix, 
	uniform float4x4 World, 
	uniform float4 LightPos)
{
	//create the vertex out struct
	VertexOut OUT;
	
	OUT.texCoord0 = IN.UV; //pass coords for diffuse map
	OUT.texCoord1 = IN.UV; //pass coords for normal map
#if 1
	// compute the 3x3 tranform from tangent space to object space
	float3x3 objToTangentSpace;
	objToTangentSpace[0] = IN.B;
	objToTangentSpace[1] = -IN.T;
	objToTangentSpace[2] = IN.Normal;
	
	//put the vert position in world space
	float4 worldSpacePos = mul(IN.Position, World);
	
	//cast a ray to the light
	float4 normLightVec = LightPos - worldSpacePos;

	//transform the light vector to object space
    float3 objnormLightVec = mul(normLightVec, WorldIMatrix).xyz;
	objnormLightVec = normalize(objnormLightVec);
    
	// transform light vector from object space to tangent space and pass it as a color 
	OUT.LightVector = 0.5 * mul(objToTangentSpace, objnormLightVec) + 0.5;

	// transform position to projection space
	OUT.HPosition = mul(IN.Position, WorldViewProj);
#else
	OUT.LightVector = 0;
	// transform position to projection space
	OUT.HPosition = mul(IN.Position, WorldViewProj);
#endif
	return OUT;
}


//pixel shader*************************************
PixelOut myps(VertexOut IN,
	uniform sampler2D colorMap,
	uniform sampler2D NormalMap,
	uniform float4 LightColor,
	uniform float4 AmbientColor) 
{
    PixelOut OUT; 
    
    //get the color from the diffuse texture
    float3 texColor = tex2D(colorMap, IN.texCoord0);
    
    //get the color from the normal map and convert to normal
	float3 bumpNormal = (2 * (tex2D(NormalMap, IN.texCoord1)-0.5));
	
	//unpack the light vector to [-1,1]
	float3 lightVector = 2* (IN.LightVector - 0.5);
    
	//compute the angle to the light and clamp at zero
	float bump = max(dot(bumpNormal,lightVector),0);
	
	//compute final color (diffuse + ambient)
	float3 diffuse = texColor * bump * LightColor;
	float3 ambient = texColor * AmbientColor*0.5;
	OUT.col.rgb = diffuse + ambient;
	OUT.col.a = 1.0;
    
    return OUT;
}


//MAIN*************************************
technique diffuseBump
{
	pass p0 
	{	
		ZEnable = true;
		ZWriteEnable = true;
		CullMode = CW;	
		VertexShader = compile vs_1_1 DiffuseBumpVS(wvp,WorldIMatrix,world,lightPos);

		PixelShader = compile ps_1_1 myps(cmap,normalSampler,lightColor,ambiColor);
	}
}

/***************************** eof ***/
