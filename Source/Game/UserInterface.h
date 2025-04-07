#pragma once
#include "framework.h"
#include "Interfaces.h"
#include "Source/DirectX/DirectX.h" 
#include "Source/DirectX/StringDraw.h"
#include "Source/DirectX/LineDraw.h"
#include "Entity.h"
#include "Source/DirectX/Buffers.h"
#include "Input.h"
class LeaderIcon {
	FreeShapeDraw* pFreeShapeDraw;
	GraphicalStringDraw* pStringDraw;
	LineDraw* pLineDraw;
	Entities* pEntities;
	// 右上方向が正で左下の角が原点となる座標を使う
	// 長さの単位はピクセル
	LeaderIcon(FreeShapeDraw* pFreeShapeDraw, GraphicalStringDraw* pStringDraw, LineDraw* pLineDraw, Entities* pEntities) {
		this->pFreeShapeDraw = pFreeShapeDraw;
		this->pStringDraw = pStringDraw;
		this->pLineDraw = pLineDraw;
		this->pEntities = pEntities;
	}
	void DrawLeaderIcon(float left, float bottom, entt::entity leader) {
		float width = 30;
		float height = 40;
		FourPoints outline = FourPoints(
			{ left,bottom,0.5,1 },
			{ left,bottom + height,0.5,1 },
			{ left + width,bottom,0.5,1 },
			{ left + width,bottom + height,0.5,1 }
		);
		DirectX::XMVECTOR center = { left + width / 2.0f, bottom + height / 2.0f, 0.5f, 1 };
		Component::CorpsData* pCorpsData = pEntities->Registry.try_get<Component::CorpsData>(leader);
		if (pCorpsData == nullptr) {
			pFreeShapeDraw->Append(0.6, 0.6, 0.6, outline, 1);
			pFreeShapeDraw->AppendRectEdge(0.3, 0.3, 0.3, outline, 1, 1);
			pStringDraw->SimpleAppend("X", 0.8, 0, 0, center, height * 0.8, 1, StrDrawPos::AsCenter);
		}
		else {
			pFreeShapeDraw->Append(0.6, 0.6, 0.6, outline, 1);
			pFreeShapeDraw->AppendRectEdge(0.3, 0.3, 0.3, outline, 1, 1);

			pStringDraw->SimpleAppend(std::to_string(pCorpsData->AllMemberUnit.size()), 0.8, 0.8, 0.8, center, 20, 1, StrDrawPos::AsCenter);
		}
	}
};
class UserInterface {
public:
	FreeShapeDraw mFreeShapeDraw;
	GraphicalStringDraw mStringDraw;
	LineDraw mLineDraw;
	Entities* pEntities;
	ConstantBuffer* pCBuffer;
	UserInterface() {}
	DirectX::XMVECTOR GetFourVFromTwoV() {
		DirectX::XMVECTOR pos1;
		DirectX::XMVECTOR pos2;
		DirectX::XMVECTOR pos3;
		DirectX::XMVECTOR pos4;
	}
	UserInterface(Entities* pentities,ConstantBuffer* pcBuffer) {
		pEntities = pentities;
		pCBuffer = pcBuffer;
		mFreeShapeDraw = FreeShapeDraw(1024, 256);
		mLineDraw = LineDraw(1024, 256);
		HFONT hFont = CreateFontW(28,//文字の高さ
			0,//文字の幅、0だと高さに合わせて調整される？
			0,//文の傾き？
			0,//文字の傾き？
			1000,//太さ　大きいほど太い
			FALSE,// 斜体
			FALSE,// 下線
			FALSE,// 取り消し線
			SHIFTJIS_CHARSET,//文字セット SHIFT_JISもある
			OUT_TT_ONLY_PRECIS,//精度　よくわからん
			CLIP_DEFAULT_PRECIS,
			ANTIALIASED_QUALITY,//品質 とりあえずアンチエイリアスは使う
			DEFAULT_PITCH || FF_MODERN,//見た目のグループ？
			L"BIZ UDゴシック"//フォントの名前
		);
		mStringDraw = GraphicalStringDraw(hFont, 65536, 2048, 32);
	}
	void UpdateWhileMission() {
		float width = D3D.Width;
		float height = D3D.Height;
		DirectX::XMMATRIX viewProjection = DirectX::XMMATRIX{
			2.0f/width, 0, 0, 0,
			0, 2.0f / height, 0, 0,
			0, 0, 1, 0,
			-1, -1, 0, 1
		};
		DirectX::XMStoreFloat4x4(&pCBuffer->Data.ViewProjection,viewProjection);
		pCBuffer->UpdateAndSet();
		mFreeShapeDraw.mAppearances.Clear();
		mStringDraw.mAppearances.Clear();
		mLineDraw.mAppearances.Clear();
		// 淵の暗いやつ
		mFreeShapeDraw.Append("Data/Textures/texLensMask.tif",FourPoints(
			{ 0, 0, 0.5, 1 },
			{ 0, height, 0.5, 1 },
			{ width, 0, 0.5, 1 },
			{ width, height, 0.5, 1 }),
			1);
		// HP表示　右上
		Component::UnitData& playerUnitData = pEntities->Registry.get<Component::UnitData>(Interface::PlayingUnit);
		Component::DamagePool& playerDamagePool = pEntities->Registry.get<Component::DamagePool>(Interface::PlayingUnit);
		float HPvar = 20 + 380 * (1 - playerDamagePool.Damage.physical / playerUnitData.MaxHP);
		mLineDraw.Append(0, 0, 0,
			{ 19, height - 30, 0.5, 1 },
			{ 401, height - 30, 0.5, 1 }, 8, 1);
		mLineDraw.Append(0.2, 1.0, 0.2,
			{ 20, height - 30, 0.5, 1 },
			{ HPvar, height - 30, 0.5, 1 }, 6, 1);
		int leftHP = (int)roundf(playerUnitData.MaxHP - playerDamagePool.Damage.physical);
		mStringDraw.SimpleAppend(std::to_string(leftHP), 1, 1, 1,
			{ 22, height - 45, 0.5, 1 }, 20, 1, StrDrawPos::AsTopLeftCorner);
		mFreeShapeDraw.Append(0.2f, 0.2f, 0.6f,FourPoints(
			{ 20, height - 67, 0.5, 1 },
			{ 20, height - 43, 0.5, 1 },
			{ 160, height - 67, 0.5, 1 },
			{ 160, height - 43, 0.5, 1 }),
			1);



		mFreeShapeDraw.Draw();
		mLineDraw.Draw();
		mStringDraw.Draw();
	}
};