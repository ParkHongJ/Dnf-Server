#include "Client_Shader_Defines.hpp"

matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D	g_DiffuseTexture;
vector		g_vCamPosition;

sampler DefaultSampler = sampler_state {

	filter = min_mag_mip_linear;
	/*minfilter = linear;
	magfilter = linear;
	mipfilter = linear;*/
};

sampler PointSampler = sampler_state {

	filter = min_mag_mip_Point;
	/*minfilter = linear;
	magfilter = linear;
	mipfilter = linear;*/
};

struct VS_IN
{
	float3		vPosition : POSITION;
	float2		vSize : PSIZE;
	

	float4		vRight : TEXCOORD1;
	float4		vUp : TEXCOORD2;
	float4		vLook : TEXCOORD3;
	float4		vTranslation : TEXCOORD4;
};

struct VS_OUT
{
	float4		vPosition : POSITION;
	float2		vSize : PSIZE;
};


VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	float4x4	TransformMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);

	/* 정점 여섯개를 내가 원하는 상태로 변형한다. In.LocalSpace */
	vector		vPosition = mul(float4(In.vPosition, 1.f), TransformMatrix);

	Out.vPosition = mul(vPosition, g_WorldMatrix);
	Out.vSize = In.vSize;
	
	return Out;
}

struct GS_IN
{
	float4		vPosition : POSITION;
	float2		vSize : PSIZE;
};

struct GS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
};

[maxvertexcount(6)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> GeometryStream)
{
	GS_OUT			Out[4];

	float3			vLook = (g_vCamPosition - In[0].vPosition).xyz;
	float3			vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook)) * In[0].vSize.x * 0.5f;
	float3			vUp = normalize(cross(vLook, vRight)) * In[0].vSize.y * 0.5f;

	matrix			matVP = mul(g_ViewMatrix, g_ProjMatrix);

	Out[0].vPosition = mul(vector(In[0].vPosition.xyz + vRight + vUp, 1.f), matVP);
	Out[0].vTexUV = float2(0.f, 0.f);

	Out[1].vPosition = mul(vector(In[0].vPosition.xyz - vRight + vUp, 1.f), matVP);
	Out[1].vTexUV = float2(1.f, 0.f);

	Out[2].vPosition = mul(vector(In[0].vPosition.xyz - vRight - vUp, 1.f), matVP);
	Out[2].vTexUV = float2(1.f, 1.f);

	Out[3].vPosition = mul(vector(In[0].vPosition.xyz + vRight - vUp, 1.f), matVP);
	Out[3].vTexUV = float2(0.f, 1.f);

	GeometryStream.Append(Out[0]);
	GeometryStream.Append(Out[1]);
	GeometryStream.Append(Out[2]);
	GeometryStream.RestartStrip();


	GeometryStream.Append(Out[0]);
	GeometryStream.Append(Out[2]);
	GeometryStream.Append(Out[3]);
	GeometryStream.RestartStrip();	
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	float4		vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(PointSampler, In.vTexUV);

	Out.vColor = float4(1.f, 0.f, 0.f, Out.vColor.a);

	if (0 == Out.vColor.a)
		discard;

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
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	
}