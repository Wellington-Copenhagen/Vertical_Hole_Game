#pragma once
#include "framework.h"
#include "Direct3D.h"
#include "Interfaces.h"
#include "Appearance.h"
#include "Texture.h"
#include "Buffers.h"
#include "DrawPipe.h"
extern int Tick;
struct FourPoints {
	FourPoints(DirectX::XMVECTOR pos1, DirectX::XMVECTOR pos2, DirectX::XMVECTOR pos3, DirectX::XMVECTOR pos4) {
		Pos1 = pos1;
		Pos2 = pos2;
		Pos3 = pos3;
		Pos4 = pos4;
	}
	FourPoints() {
		Pos1 = { 0,0,0,0 };
		Pos2 = { 0,0,0,0 };
		Pos3 = { 0,0,0,0 };
		Pos4 = { 0,0,0,0 };
	}
	DirectX::XMVECTOR Pos1;
	DirectX::XMVECTOR Pos2;
	DirectX::XMVECTOR Pos3;
	DirectX::XMVECTOR Pos4;
};
inline FourPoints GetFourVFromTwoV(DirectX::XMVECTOR vertex1, DirectX::XMVECTOR vertex2) {
	FourPoints fourV;
	float z = (vertex1.m128_f32[2] + vertex2.m128_f32[2]) / 2.0f;
	fourV.Pos1 = { min(vertex1.m128_f32[0],vertex1.m128_f32[0]),min(vertex1.m128_f32[1],vertex1.m128_f32[1]),z,1 };
	fourV.Pos2 = { min(vertex1.m128_f32[0],vertex1.m128_f32[0]),max(vertex1.m128_f32[1],vertex1.m128_f32[1]),z,1 };
	fourV.Pos3 = { max(vertex1.m128_f32[0],vertex1.m128_f32[0]),min(vertex1.m128_f32[1],vertex1.m128_f32[1]),z,1 };
	fourV.Pos4 = { max(vertex1.m128_f32[0],vertex1.m128_f32[0]),max(vertex1.m128_f32[1],vertex1.m128_f32[1]),z,1 };
	return fourV;
}
class LineDraw {
public:
	// ドローコールの情報
	TimeBindAppearances<Interface::LineDrawCallType, Interface::LineInstanceType, 1> mAppearances;
	VertexBuffer<Interface::LineDrawCallType> mDrawCallBuffer;
	VertexBuffer<Interface::LineInstanceType> mInstanceBuffer;

	// 行の高さを1とする
	// テクスチャ全般の情報
	SameFormatTextureArray mTextureArray;

	LineDraw() {
	}
	LineDraw(int maxInstanceCount,int maxTextureCount) {
		mAppearances = TimeBindAppearances<Interface::LineDrawCallType, Interface::LineInstanceType, 1>(maxInstanceCount);
		Interface::InitDrawCallUV(mAppearances.DrawCall, 0);
		Interface::InitDrawCallPos(mAppearances.DrawCall, 0);
		mDrawCallBuffer = VertexBuffer < Interface::LineDrawCallType>(mAppearances.GetDrawCallPointer(0), 4, 0);
		mInstanceBuffer = VertexBuffer<Interface::LineInstanceType>(mAppearances.GetInstancePointer(0), maxInstanceCount, 1);
		mTextureArray = SameFormatTextureArray(maxTextureCount, true, 64);
	}
	void Update() {
		mAppearances.EraseExpired();
	}
	void Draw() {
		// 描画処理
		mTextureArray.SetToGraphicPipeLine();
		GraphicProcessSetter::SetAsLine();
		mInstanceBuffer.UpdateAndSet(mAppearances.GetInstancePointer(0), 0, mAppearances.InstanceCount);
		mDrawCallBuffer.UpdateAndSet(mAppearances.GetDrawCallPointer(0), 0, 4);
		D3D.m_deviceContext->DrawInstanced(4, mAppearances.InstanceCount, 0, 0);
	}
	void Append(float red,float green,float blue,DirectX::XMVECTOR left,DirectX::XMVECTOR right,float width,int tickToDelete,bool isEternal=false) {
		Interface::LineInstanceType* newInstance;
		mAppearances.Add(Tick + tickToDelete, isEternal, &newInstance);
		newInstance->Set(left, right, width, mTextureArray.AppendOneColorTexture(red, green, blue, 1));
	}
	void Append(float red, float green, float blue, float alpha, DirectX::XMVECTOR left, DirectX::XMVECTOR right, float width, int tickToDelete, bool isEternal = false) {
		Interface::LineInstanceType* newInstance;
		mAppearances.Add(Tick + tickToDelete, isEternal, &newInstance);
		newInstance->Set(left, right, width, mTextureArray.AppendOneColorTexture(red, green, blue, alpha));
	}
	void Append(std::string texPath, DirectX::XMVECTOR left, DirectX::XMVECTOR right, float width, int tickToDelete, bool isEternal = false) {
		Interface::LineInstanceType* newInstance;
		mAppearances.Add(Tick + tickToDelete, isEternal, &newInstance);
		newInstance->Set(left, right, width, mTextureArray.AppendFromFileName(texPath));
	}
};
class FreeShapeDraw {
public:
	// ドローコールの情報
	TimeBindAppearances<Interface::FreeShapeDrawCallType, Interface::FreeShapeInstanceType, 1> mAppearances;
	VertexBuffer<Interface::FreeShapeDrawCallType> mDrawCallBuffer;
	VertexBuffer<Interface::FreeShapeInstanceType> mInstanceBuffer;

	// 行の高さを1とする
	// テクスチャ全般の情報
	SameFormatTextureArray mTextureArray;

	FreeShapeDraw() {
	}
	FreeShapeDraw(int maxInstanceCount, int maxTextureCount) {
		mAppearances = TimeBindAppearances<Interface::FreeShapeDrawCallType, Interface::FreeShapeInstanceType, 1>(maxInstanceCount);
		Interface::InitDrawCallUV(mAppearances.DrawCall, 0);
		mDrawCallBuffer = VertexBuffer < Interface::FreeShapeDrawCallType>(mAppearances.GetDrawCallPointer(0), 4, 0);
		mInstanceBuffer = VertexBuffer<Interface::FreeShapeInstanceType>(mAppearances.GetInstancePointer(0), maxInstanceCount, 1);
		mTextureArray = SameFormatTextureArray(maxTextureCount, true, 64);
	}
	void Update() {
		mAppearances.EraseExpired();
	}
	void Draw() {
		// 描画処理
		mTextureArray.SetToGraphicPipeLine();
		GraphicProcessSetter::SetAsFreeShape();
		mInstanceBuffer.UpdateAndSet(mAppearances.GetInstancePointer(0), 0, mAppearances.InstanceCount);
		mDrawCallBuffer.UpdateAndSet(mAppearances.GetDrawCallPointer(0), 0, 4);
		D3D.m_deviceContext->DrawInstanced(4, mAppearances.InstanceCount, 0, 0);
	}
	void Append(float red, float green, float blue, FourPoints shape, int tickToDelete, float alpha = 1, bool isEternal = false) {
		Interface::FreeShapeInstanceType* newInstance;
		mAppearances.Add(Tick + tickToDelete, isEternal, &newInstance);
		newInstance->Set(&shape.Pos1, &shape.Pos2, &shape.Pos3, &shape.Pos4, mTextureArray.AppendOneColorTexture(red, green, blue, alpha));
	}
	void Append(std::string texPath, FourPoints shape, int tickToDelete, bool isEternal = false) {
		Interface::FreeShapeInstanceType* newInstance;
		mAppearances.Add(Tick + tickToDelete, isEternal, &newInstance);
		newInstance->Set(&shape.Pos1, &shape.Pos2, &shape.Pos3, &shape.Pos4, mTextureArray.AppendFromFileName(texPath));
	}
	void AppendRectEdge(float red, float green, float blue, FourPoints shape, float width, int tickToDelete, float alpha = 1, bool isEternal = false) {
		Interface::FreeShapeInstanceType* newInstance;
		DirectX::XMVECTOR horizontalGap = DirectX::XMVECTOR{ width, 0, 0, 0 };
		DirectX::XMVECTOR virticalGap = DirectX::XMVECTOR{ 0, width, 0, 0 };
		// 左
		{
			DirectX::XMVECTOR pos1 = shape.Pos1;
			DirectX::XMVECTOR pos2 = shape.Pos2;
			DirectX::XMVECTOR pos3 = DirectX::XMVectorAdd(shape.Pos1, horizontalGap);
			DirectX::XMVECTOR pos4 = DirectX::XMVectorAdd(shape.Pos2, horizontalGap);
			mAppearances.Add(Tick + tickToDelete, isEternal, &newInstance);
			newInstance->Set(&pos1, &pos2, &pos3, &pos4, mTextureArray.AppendOneColorTexture(red, green, blue, 1));
		}
		// 右
		{
			DirectX::XMVECTOR pos1 = DirectX::XMVectorSubtract(shape.Pos3, horizontalGap);
			DirectX::XMVECTOR pos2 = DirectX::XMVectorSubtract(shape.Pos4, horizontalGap);
			DirectX::XMVECTOR pos3 = shape.Pos3;
			DirectX::XMVECTOR pos4 = shape.Pos4;
			mAppearances.Add(Tick + tickToDelete, isEternal, &newInstance);
			newInstance->Set(&pos1, &pos2, &pos3, &pos4, mTextureArray.AppendOneColorTexture(red, green, blue, 1));
		}
		// 上
		{
			DirectX::XMVECTOR pos1 = DirectX::XMVectorSubtract(shape.Pos2, virticalGap);
			DirectX::XMVECTOR pos2 = shape.Pos2;
			DirectX::XMVECTOR pos3 = DirectX::XMVectorSubtract(shape.Pos4, virticalGap);
			DirectX::XMVECTOR pos4 = shape.Pos4;
			mAppearances.Add(Tick + tickToDelete, isEternal, &newInstance);
			newInstance->Set(&pos1, &pos2, &pos3, &pos4, mTextureArray.AppendOneColorTexture(red, green, blue, 1));
		}
		// 下
		{
			DirectX::XMVECTOR pos1 = shape.Pos1;
			DirectX::XMVECTOR pos2 = DirectX::XMVectorAdd(shape.Pos1, virticalGap);
			DirectX::XMVECTOR pos3 = shape.Pos3;
			DirectX::XMVECTOR pos4 = DirectX::XMVectorAdd(shape.Pos4, virticalGap);
			mAppearances.Add(Tick + tickToDelete, isEternal, &newInstance);
			newInstance->Set(&pos1, &pos2, &pos3, &pos4, mTextureArray.AppendOneColorTexture(red, green, blue, 1));
		}
	}
};