#pragma once
#include "Source/DirectX/DirectX.h"
#include "Interfaces.h"

//マスクは
// r->1枚目テクスチャ濃度
// g->2枚目テクスチャ濃度
// b->3枚目テクスチャ濃度(影でもいいかと思うが)
// 
//頂点データの追加、更新はここで行っている
//BuffersはInstancesのポインタを受け取るだけ

// : public Interface::IAppearances<DCType,IType>

template <class DCType, class IType, int InstancePerEntity, int InstanceMaxCount>
class Appearances{
public:
	std::vector<Interface::EntityPointer>* pINtoIndex;
	//実際のインスタンスデータ
	IType* Instances;
	//登録したエンティティ
	Interface::EntId* InstanceIds;
	//ドローコールの情報
	DCType DrawCall[4];
	//各分類ごとの登録数
	Interface::RectAppId InstanceCount;
	Appearances(std::vector<Interface::EntityPointer>* pintoindex, int separation, float bottomXscale=1, float topXscale=1, float Yscale=1, float Yoffset=0) {
		pINtoIndex = pintoindex;
		Instances = new IType[InstanceMaxCount * InstancePerEntity];
		InstanceIds = new Interface::EntId[InstanceMaxCount];
		ZeroMemory(Instances, sizeof(Instances));
		ZeroMemory(InstanceIds, sizeof(InstanceIds));
		ZeroMemory(DrawCall, sizeof(DrawCall));
		InstanceCount = 0;
		float width = 4.0f / 6.0f;
		DrawCall[0].Pos = { -1 * bottomXscale * width,-1 * Yscale * width + Yoffset,0.0f,1.0f };
		DrawCall[1].Pos = { -1 * topXscale * width,Yscale * width+Yoffset,0.0f,1.0f };
		DrawCall[2].Pos = { bottomXscale * width,-1 * Yscale * width + Yoffset,0.0f,1.0f };
		DrawCall[3].Pos = { topXscale * width,Yscale * width + Yoffset,0.0f,1.0f };
		DrawCall[0].UV = { 0.0f,1.0f / separation,(float)separation };
		DrawCall[1].UV = { 0.0f,0.0f,(float)separation };
		DrawCall[2].UV = { 1.0f / separation,1.0f / separation,(float)separation };
		DrawCall[3].UV = { 1.0f / separation,0.0f,(float)separation };
	}
	Appearances() {
		Instances = nullptr;
		InstanceIds = nullptr;
	}
	//頭のインスタンスのポインタを返すということで
	inline Interface::RectAppId Add(Interface::EntId Id, IType** pInstanceData) {
		if (InstanceCount >= InstanceMaxCount - 1) {
			return -1;
		}
		*pInstanceData = &Instances[InstanceCount*InstancePerEntity];
		InstanceIds[InstanceCount] = Id;
		InstanceCount++;
		return InstanceCount - 1;
	}
	inline void Update(Interface::RectAppId Id, IType** pInstanceData) {
		if (Id != -1) {
			*pInstanceData = &Instances[Id*InstancePerEntity];
		}
	}
	//
	inline Interface::SameArchIndex Delete(Interface::RectAppId toDelete) {
		if (toDelete != -1) {
			//頂点データの移動
			for (int i = 0; i < InstancePerEntity;i++) {
				Instances[toDelete * InstancePerEntity + i] = Instances[InstanceCount * InstancePerEntity + i];
			}
			//EntIdの移動
			InstanceIds[toDelete] = InstanceIds[InstanceCount];
			//カウントを減らす
			InstanceCount--;
			//移動させたもののSameArchIndexを出力
			return pINtoIndex->at(InstanceIds[toDelete]).Index;
		}
		else {
			int a = pINtoIndex->size();
			return -1;
		}
	}
};