#include "Client_Shader_Defines.hpp"
matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D	g_DiffuseTexture;
texture2D	g_DiffuseTexture1;
texture2D	g_FilterTexture;

texture2D	g_BrushTexture;
vector		g_vBrushPos = vector(10.f, 0.f, 10.f, 1.f);
float		g_fBrushRange = 5.f;

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
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
	float4		vProjPos : TEXCOORD2;
};


VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	Out.vNormal = mul(float4(In.vNormal, 0.f), g_WorldMatrix);
	Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);

	Out.vProjPos = Out.vPosition;
		
	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
	float4		vProjPos : TEXCOORD2;
};

struct PS_OUT
{
	float4		vDiffuse : SV_TARGET0;
	float4		vNormal : SV_TARGET1;
	float4		vDepth : SV_TARGET2;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector		vSourDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV * 30.0f);
	vector		vDestDiffuse = g_DiffuseTexture1.Sample(DefaultSampler, In.vTexUV * 30.0f);
	vector		vFilter = g_FilterTexture.Sample(PointSampler, In.vTexUV);

	vector		vMtrlDiffuse = vSourDiffuse * vFilter + vDestDiffuse * (1.f - vFilter);

	vector		vBrush = (vector)0.0f; 	

	if (g_vBrushPos.x - g_fBrushRange < In.vWorldPos.x && In.vWorldPos.x <= g_vBrushPos.x + g_fBrushRange &&
		g_vBrushPos.z - g_fBrushRange < In.vWorldPos.z && In.vWorldPos.z <= g_vBrushPos.z + g_fBrushRange)
	{
		float2	vTexUV;

		vTexUV.x = (In.vWorldPos.x - (g_vBrushPos.x - g_fBrushRange)) / (2.f * g_fBrushRange);
		vTexUV.y = ((g_vBrushPos.z + g_fBrushRange) - In.vWorldPos.z) / (2.f * g_fBrushRange);;

		vBrush = g_BrushTexture.Sample(DefaultSampler, vTexUV);
	}

	Out.vDiffuse = vMtrlDiffuse + vBrush;
	Out.vDiffuse.a = 1.f;

	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);

	Out.vDepth = vector( In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 300.0f, 0.0f, 0.0f);

	return Out;	
}

technique11 DefaultTechnique
{
	pass DefaultPass
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

}