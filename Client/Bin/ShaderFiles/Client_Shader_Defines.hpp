
RasterizerState RS_Skybox
{
	FillMode = solid;

	/* �ո��� �ø��ϰڴ�. == �ĸ��� �����ְڴ�. */
	CullMode = Front;

	/* �ո��� �ð�������� ���ڴ�. */
	FrontCounterClockwise = false;
};


RasterizerState RS_Default
{
	FillMode = solid;
};

RasterizerState RS_Wireframe
{
	FillMode = wireframe;
};

DepthStencilState DSS_Default
{
	DepthEnable = true;
	DepthWriteMask = all;
	DepthFunc = less_equal;
};

DepthStencilState DSS_Skybox
{
	DepthEnable = false;
	DepthWriteMask = zero;	
};


BlendState BS_Default
{
	BlendEnable[0] = false;
};

BlendState BS_AlphaBlend
{
	BlendEnable[0] = true;

	SrcBlend = Src_Alpha;
	DestBlend = Inv_Src_Alpha;
	BlendOp = Add;
};




