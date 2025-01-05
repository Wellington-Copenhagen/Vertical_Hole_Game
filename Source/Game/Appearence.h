#pragma once
#include "Source/DirectX/DirectX.h"
#include "Interfaces.h"
#include "Source/DirectX/Texture.h"

class BaseAppearence {
public:
	MotionInfo* pWorld;
 	virtual void SetToBufferData(IAllGraphicBuffer* pBuffer) = 0;
	virtual void EveryTickAction() = 0;
	BaseAppearence(MotionInfo* pworld) {
		pWorld = pworld;
	}
};
class RectAppearence  : public BaseAppearence{
public:
	void SetToBufferData(IAllGraphicBuffer* pBuffer) override;
	void EveryTickAction() override {};
	RectAppearence(MotionInfo* pworld) :BaseAppearence(pworld) {

	}
};
class AbsoluteRectAppearence : public BaseAppearence {
public:
	void SetToBufferData(IAllGraphicBuffer* pBuffer) override;
	void EveryTickAction() override {};
	AbsoluteRectAppearence(MotionInfo* pworld) :BaseAppearence(pworld) {

	}
};
/*
class ColorVarRectAppearence : public BaseAppearence {
	DirectX::XMFLOAT4 Color1;
	DirectX::XMFLOAT4 Color2;
public:
	void SetToBufferData(IGraphicBuffer* pBuffer) override;
	void EveryTickAction() override;
	ColorVarRectAppearence(MotionInfo* pworld,DirectX::XMFLOAT4 color1, DirectX::XMFLOAT4 color2) :BaseAppearence(pworld) {
		Color1 = color1;
		Color2 = color2;
	}
};
*/


class BaseDrawCallData {
protected:
	TextureArray Texture;
	std::list<BaseAppearence*> BelongingInstances;
public:
	virtual void SetToBufferData(IAllGraphicBuffer* pBuffer) = 0;
	virtual IAppearanceHandle AddNew(MotionInfo* pWorld);
	BaseDrawCallData(Json::Value fromLoad);
};
class RectDrawCallData : BaseDrawCallData{
public:
	void SetToBufferData(IAllGraphicBuffer* pBuffer) override;
	DirectX::XMFLOAT3 Poss[4];
	DirectX::XMFLOAT2 UVs[4];
	RectDrawCallData(Json::Value fromLoad);
};
class AbsoluteRectDrawCallData : BaseDrawCallData {
public:
	void SetToBufferData(IAllGraphicBuffer* pBuffer) override;
	DirectX::XMFLOAT3 Poss[4];
	DirectX::XMFLOAT2 UVs[4];
	AbsoluteRectDrawCallData(Json::Value fromLoad);
};
/*
class ColorVarRectDrawCallData : BaseDrawCallData {
public:
	void SetToBufferData(IGraphicBuffer* pBuffer) override;
	DirectX::XMFLOAT3 Poss[4];
	DirectX::XMFLOAT2 UVs[4];
	DirectX::XMFLOAT3 Colors;
	ColorVarRectDrawCallData(Json::Value fromLoad);
};
*/
class AllAppearance : IAllAppearance{
public:
	IAllGraphicBuffer* pBuffer;
	std::vector<BaseDrawCallData> DrawCalls;
	AllAppearance(IAllGraphicBuffer* pbuffer) {
		pBuffer = pbuffer;
	}
	void Load(std::string AppearanceFileName) override;
	IAppearanceHandle AddNew(MotionInfo* pWorld, int drawCallIndex) override;
	void Draw() override;
};
class BaseAppearanceHandle : public IAppearanceHandle {
protected:
	std::list<BaseAppearence*>::iterator Appearance;
	std::list<BaseAppearence*>* pDrawCall;
public:
	BaseAppearanceHandle(std::list<BaseAppearence*>* pdrawcall, std::list<BaseAppearence*>::iterator appearance) {
		pDrawCall = pdrawcall;
		Appearance = appearance;
	}
	void Delete() override {
		pDrawCall->erase(Appearance);
	}
	void SetTexIndex(int index) override {
		(*Appearance)->pWorld->TexIndex = index;
	}
};