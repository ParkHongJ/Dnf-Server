
#pragma once

#pragma warning (disable : 4005)
#pragma warning (disable : 4251)

#include <d3d11.h>
#include <d3dcompiler.h>
#include <FX11/d3dx11effect.h>
#include <DirectXTK/DDSTextureLoader.h>
#include <DirectXTK/WICTextureLoader.h>
#include <DirectXTK/ScreenGrab.h>

#include <DirectXTK/PrimitiveBatch.h>
#include <DirectXTK/Effects.h>
#include <DirectXTK/VertexTypes.h>
#include <DirectXTK/SpriteBatch.h>
#include <DirectXTK/SpriteFont.h>

#include <DirectXCollision.h>


#include <Assimp/scene.h>
#include <Assimp/Importer.hpp>
#include <Assimp/postprocess.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include <DirectXMath.h>
using namespace DirectX;

#include <vector>
#include <list>
#include <map>
#include <algorithm>

#include <process.h>

#include "Engine_Macro.h"
#include "Engine_Struct.h"
#include "Engine_Typedef.h"
#include "Engine_Functor.h"
#include "Engine_Function.h"

#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif

#endif // _DEBUG

using namespace std;
using namespace Engine;


