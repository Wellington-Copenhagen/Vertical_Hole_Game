#pragma once
#include "framework.h"
#include "Source/DirectX/Direct3D.h"
#include "Interfaces.h"
#include "Source/DirectX/Texture.h"
#include "Source/DirectX/Buffers.h"
#include "Source/DirectX/DrawPipe.h"
#include "Source/DirectX/StringDraw.h"
enum IconTexIndex {

};
class UserInterface {
public:
	// テクスチャの名前とインデックスを対応させる方法はenum
	SameFormatTextureArray<256> mTextureArray;
	Interface::IconInstanceType mIconInstances[256];
	Interface::IconDrawCallType mIconDrawCalls[256];
	VertexBuffer<Interface::IconDrawCallType, 256, 0> mDrawCallBuffer;
	VertexBuffer<Interface::IconInstanceType, 256, 1> mInstanceBuffer;
	GraphicalStringDraw<1024, 2048, 32> mStringDraw;
	void UpdateAsPlaying();
	void UpdateAsMenu();
	UserInterface() {

	}
};