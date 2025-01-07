#pragma once
#include "framework.h"
#include "Component.h"
#include "Interfaces.h"
class AllEntities {
	//bitsetの引数はComponentの数だけ
	std::vector<std::bitset<32>> Archetypes;
	std::vector<SameArchetype> EntityArraies;
	void Load(std::string filePath);
	EntityPointer INtoIndex[UINT16_MAX * 4];
	inline void PushToDeleteFromEIN(EntityIdentifyN instanceEIN);
	inline void AddFromEIN(EntityIdentifyN prototypeEIN);
};
//同じArchetypeを持つEntityの集合
class SameArchetype {
public:
	//新しいエンティティを作るときの付加情報はこの関数の引数にポインタを指定するくらいしか案はない(ないならnullptr)
	inline int Add(int prototypeIndex);
	//削除を一気にやらないと整合性の問題になりそう
	inline void PushToDelete(int entityIndex);
	inline void Delete();

	SameArchetype() {
		GlobalIdentityNumber = std::vector<int>();
		CompTextureData = std::vector<Component::RectAppearance>();
	}
	//これは読み込み時用(Prototypesの中でしか呼ばない)
	void AddFromFile();

	SameArchetype Prototypes;
	//全オブジェクトに固有で割り当てる
	//オブジェクト間の識別用
	std::vector<EntityIdentifyN> GlobalIdentityNumber;
	//これより下が各Componentのデータ
	std::vector<Component::RectAppearance> CompTextureData;
};