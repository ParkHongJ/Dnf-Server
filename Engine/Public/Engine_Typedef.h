#pragma once

namespace Engine
{
	enum DIMK { DIMK_LBUTTON, DIMK_RBUTTON, DIMK_WHEEL, DIMK_X, DIMK_END };
	enum DIMM { DIMM_X, DIMM_Y, DIMM_WHEEL, DIMM_END };

	typedef unsigned char				_uchar;
	typedef signed char					_char;

	typedef unsigned short				_ushort;
	typedef signed short				_short;

	typedef unsigned long				_ulong;
	typedef signed long					_long;

	typedef unsigned int				_uint;
	typedef signed int					_int;

	typedef float						_float;
	typedef double						_double;

	typedef bool						_bool;
	typedef wchar_t						_tchar;

	typedef XMFLOAT2					_float2;
	typedef XMFLOAT3					_float3;
	typedef XMFLOAT4					_float4;
	typedef XMVECTOR					_vector;
	typedef FXMVECTOR					_fvector;

	typedef XMFLOAT4X4					_float4x4;
	typedef XMMATRIX					_matrix;
	typedef FXMMATRIX					_fmatrix;

	/*typedef struct D3DXVECTOR3_DERIVED : public D3DXVECTOR3
	{
		float Get_Length();
		float Get_Distance(D3DXVECTOR3_DERIVED vTarget);

	}_float3;


	_float3		vTmp, vTmp1;

	vTmp.Get_Length();
	= vTmp.Get_Distance(vTmp1);*/

	
}