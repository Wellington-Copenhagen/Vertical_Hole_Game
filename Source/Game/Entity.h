#pragma once
#include "framework.h"
#include "Interfaces.h"
#include "Routing.h"
#include "Component.h"
#include "StringDraw.h"
class Entities {
public:
	entt::registry Registry;
	entt::registry PrototypeRegistry;

	std::vector<int> UnitCountEachTeam;
	std::vector<std::vector<int>> UnitCountAreaTeam;
	std::vector <Interface::UnitInfo> UnitInfos;
	Routing mRouting;

	// ���̏ꏊ�ɕǂ����邩
	std::bitset<WorldWidth* WorldHeight> IsWallMap;
	// �e�̏�Ԃ��X�V����K�v������ꏊ
	std::bitset<WorldWidth* WorldHeight> ShouldUpdateShadow;
	// ���̏ꏊ�����Ԃ̕����ɑ����邩
	std::vector<int> AreaIndexMap;

	Entities() {
		UnitCountEachTeam = std::vector<int>();
		for (int i = 0; i < TeamCount; i++) {
			UnitCountEachTeam.push_back(0);
		}
		UnitInfos = std::vector<Interface::UnitInfo>();
		mRouting = Routing();
	}

	template<typename typeComp,bool asPrototype>
	void EmplaceEntityIfNeeded(entt::entity entity, Json::Value onePrototypeEntityData);
	template<typename typeComp, bool asPrototype>
	void EmplaceEntityIfNeeded(entt::entity entity, Interface::EntityInitData* pInitData);
	template<typename typeComp, bool asPrototype>
	void EmplaceEntity(entt::entity entity, Interface::EntityInitData* pInitData);
	template<bool asPrototype>
	entt::entity EmplaceFromJson(Json::Value onePrototypeEntityData);
	template<bool asPrototype>
	entt::entity EmplaceFromPrototypeEntity(Interface::EntityInitData* pInitData);
	void LoadMap(std::string mapFileName);
	void LoadEntities(std::vector<std::string> fileNames);
	void LoadUnits(std::vector<std::string> fileNames);
	void LoadMission(std::string missionFileName);
	entt::entity EmplaceUnitWithInitData(int unitIndex, Interface::EntityInitData* pInitData, Interface::RelationOfCoord* pCorePos);
	int GetFloorShadow(int x, int y);
	int GetWallShadow(int x, int y);
	void DeleteWall(int x, int y);
	int GetAreaIndex(DirectX::XMVECTOR pos);
};