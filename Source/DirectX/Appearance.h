#pragma once
#include "Source/DirectX/DirectX.h"
#include "Interfaces.h"
#include "Source/Game/Component.h"
extern int Tick;
//マスクは
// r->1枚目テクスチャ濃度
// g->2枚目テクスチャ濃度
// b->3枚目テクスチャ濃度(影でもいいかと思うが)
// 
//頂点データの追加、更新はここで行っている
//BuffersはInstancesのポインタを受け取るだけ

// : public Interface::IAppearances<DCType,IType>

template <class DCType, class IType,class CompType,int InstancePerEntity>
class EntityBindAppearances{
public:
	int InstanceMaxCount;
	//実際のインスタンスデータ
	std::vector<IType> Instances;
	//登録したエンティティ
	std::vector<entt::entity> InstanceEntities;
	//ドローコールの情報
	std::vector<DCType> DrawCall;
	//各分類ごとの登録数
	Interface::RectAppId InstanceCount;
	EntityBindAppearances(int instanceMaxCount) {
		InstanceMaxCount = instanceMaxCount;
		Instances = std::vector<IType>(InstanceMaxCount * InstancePerEntity);
		DrawCall = std::vector<DCType>(4 * InstancePerEntity);
		InstanceEntities = std::vector<entt::entity>(InstanceMaxCount);
		InstanceCount = 0;
	}
	EntityBindAppearances() {
	}
	inline IType* GetInstancePointer(int index) {
		return &Instances[index * InstanceMaxCount];
	}
	inline DCType* GetDrawCallPointer(int index) {
		return &DrawCall[index * 4];
	}

	//頭のインスタンスのポインタを返すということで
	inline Interface::RectAppId Add(entt::entity entity, IType* pInstanceData[InstancePerEntity]) {
		if (InstanceCount >= InstanceMaxCount - 1) {
			return -1;
		}
		for (int i = 0; i < InstancePerEntity; i++) {
			pInstanceData[i] = &Instances[i * InstanceMaxCount + InstanceCount];
		}
		InstanceEntities[InstanceCount] = entity;
		InstanceCount++;
		return InstanceCount - 1;
	}
	inline void Update(Interface::RectAppId Id, IType* pInstanceData[InstancePerEntity]) {
		if (Id >= InstanceCount) {
			//throw("");
		}
		if (Id != -1) {
			for (int i = 0; i < InstancePerEntity; i++) {
				pInstanceData[i] = &Instances[i * InstanceMaxCount + Id];
			}
		}
	}
	// 削除時に申告させる方式だと抜けが防げないので全チェックにする
	void EraseDeletedEntities(entt::registry* pRegistry) {
		for (int i = 0; i < InstanceCount; i++) {
			if (!pRegistry->valid(InstanceEntities[i])) {
				//頂点データの移動
				for (int j = 0; j < InstancePerEntity; j++) {
					Instances[i * InstancePerEntity + j] = Instances[(InstanceCount - 1) * InstancePerEntity + j];
					ZeroMemory(&Instances[(InstanceCount - 1) * InstancePerEntity + j], sizeof(IType));
				}
				//EntIdの移動
				InstanceEntities[i] = InstanceEntities[InstanceCount - 1];
				// 移動させたエンティティに新しい情報を与える
				CompType* replacedComp = pRegistry->try_get<CompType>(InstanceEntities[i]);
				if (replacedComp!=nullptr) {
					replacedComp->SetInstanceId(i);
				}
				//カウントを減らす
				InstanceCount--;
				i--;
			}
		}
	}
};


template <class DCType, class IType,int InstancePerEntity>
class TimeBindAppearances {
public:
	int InstanceMaxCount;
	//実際のインスタンスデータ
	std::vector<IType> Instances;
	// 削除するティック
	std::vector<std::tuple<int, bool>> TickToDelete;
	//ドローコールの情報
	std::vector<DCType> DrawCall;
	//各分類ごとの登録数
	Interface::RectAppId InstanceCount;
	TimeBindAppearances(int instanceMaxCount) {
		InstanceMaxCount = instanceMaxCount;
		Instances = std::vector<IType>(InstanceMaxCount * InstancePerEntity);
		DrawCall = std::vector<DCType>(4 * InstancePerEntity);
		TickToDelete = std::vector<std::tuple<int,bool>>(InstanceMaxCount);
		InstanceCount = 0;
	}
	TimeBindAppearances() {
	}
	inline IType* GetInstancePointer(int index) {
		return &Instances[index * InstanceMaxCount];
	}
	inline DCType* GetDrawCallPointer(int index) {
		return &DrawCall[index * 4];
	}
	inline void Add(int timeToDelete,bool isEternal, IType* pInstanceData[InstancePerEntity]) {
		for (int i = 0; i < InstancePerEntity; i++) {
			pInstanceData[i] = &Instances[i * InstanceMaxCount + InstanceCount];
		}
		TickToDelete[InstanceCount] = { Tick + timeToDelete ,isEternal};
		InstanceCount++;
	}
	void EraseExpired() {
		for (int i = 0; i < InstanceCount; i++) {
			if (std::get<0>(TickToDelete[i])<Tick && !std::get<1>(TickToDelete[i])) {
				//頂点データの移動
				for (int j = 0; j < InstancePerEntity; j++) {
					Instances[i * InstancePerEntity + j] = Instances[(InstanceCount - 1) * InstancePerEntity + j];
					ZeroMemory(&Instances[(InstanceCount - 1) * InstancePerEntity + j], sizeof(IType));
				}
				//EntIdの移動
				TickToDelete[i] = TickToDelete[InstanceCount - 1];
				//カウントを減らす
				InstanceCount--;
				i--;
			}
		}
	}
	void Clear() {
		InstanceCount = 0;
	}
};