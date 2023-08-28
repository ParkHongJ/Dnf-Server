
#include "Client_Shader_Defines.hpp"

matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
textureCUBE	g_DiffuseTexture;

sampler DefaultSampler = sampler_state {

	filter = min_mag_mip_linear;
	AddressU = WRAP;
	AddressV = WRAP;
};

sampler PointSampler = sampler_state {

	filter = min_mag_mip_Point;
	AddressU = WRAP;
	AddressV = WRAP;
};

struct VS_IN
{
	float3		vPosition : POSITION;	
	float3		vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;	
	float3		vTexUV : TEXCOORD0;	
};


VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;
	
	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float3		vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	float4		vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	//Out.vColor = (vector)0.f;

	return Out;
}


BlendState DefaultBlendState
{
	BlendEnable[0] = true;
	SrcBlend = Src_Alpha;
	DestBlend = Inv_Src_Alpha;
	BlendOp = Add;	
};

technique11 DefaultTechnique
{
	pass SkyBox
	{
		SetRasterizerState(RS_Skybox);
		SetDepthStencilState(DSS_Skybox, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
	/*	SetBlendState(DefaultBlendState);
		SetRasterizerState();
		*/

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}	
}