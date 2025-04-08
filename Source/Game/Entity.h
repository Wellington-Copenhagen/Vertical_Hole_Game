#pragma once
#include "framework.h"
#include "Interfaces.h"
#include "Routing.h"
#include "Component.h"
#include "Source/DirectX/StringDraw.h"
class Entities {
public:
	entt::registry Registry;
	entt::registry PrototypeRegistry;

	std::vector<int> UnitCountEachTeam;
	std::vector<std::vector<int>> UnitCountAreaTeam;
	std::vector <Interface::UnitInfo> UnitInfos;
	Routing mRouting;

	// その場所に壁があるか
	std::bitset<WorldWidth* WorldHeight> IsWallMap;
	// 影の状態を更新する必要がある場所
	std::bitset<WorldWidth* WorldHeight> ShouldUpdateShadow;

	std::tuple<int, int, int, int> PlayerCorpsSpawnArea;

	entt::entity PlayingUnit;
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
	void LoadPlayerCorps(std::string fileName);
	void LoadUnits(std::vector<std::string> fileNames);
	void LoadMission(std::string missionFileName);
	entt::entity EmplaceUnitWithInitData(int unitIndex, Interface::EntityInitData* pInitData, Interface::RelationOfCoord* pCorePos);
	int GetShadowIndex(int x, int y, bool searchFloor);
	void DeleteWall(int x, int y);
	std::pair<entt::entity,float> GetNearestHostilingUnit(DirectX::XMVECTOR pos, Interface::HostilityTeam team);
};