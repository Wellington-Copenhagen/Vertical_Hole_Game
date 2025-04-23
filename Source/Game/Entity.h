#pragma once
#include "framework.h"
#include "Interfaces.h"
#include "Routing.h"
#include "Component.h"
#include "Source/DirectX/StringDraw.h"
#include "DebugAssist.h"
#include "PathBindJsonValue.h"
class Entities {
public:
	entt::registry Registry;
	entt::registry PrototypeRegistry;

	std::vector<int> UnitCountEachTeam;
	std::vector<std::vector<int>> UnitCountAreaTeam;
	std::vector <Interface::UnitInfo> UnitInfos;
	std::vector<Interface::BallInfo> BallInfos;
	Routing mRouting;

	// その場所に壁があるか
	std::bitset<WorldWidth* WorldHeight> IsWallMap;
	// 影の状態を更新する必要がある場所
	std::bitset<WorldWidth* WorldHeight> ShouldUpdateShadow;

	std::tuple<int, int, int, int> PlayerCorpsSpawnArea;

	entt::entity PlayingUnit;
	std::vector<entt::entity> PlayerLeaders;
	std::vector<entt::entity> SpawnableUnits;
	std::bitset<TeamCount* TeamCount> HostilityTable;//チーム数*チーム数のテーブルでtrueの場所は敵対する
	std::map<std::string, Interface::UnitIndex> UnitNameHash;
	std::map<std::string, entt::entity> EntNameHash;
	std::map<std::string, int> BallNameHash;

	Entities() {
		UnitCountEachTeam = std::vector<int>();
		for (int i = 0; i < TeamCount; i++) {
			UnitCountEachTeam.push_back(0);
		}
		UnitInfos = std::vector<Interface::UnitInfo>();
		mRouting = Routing();
		UnitNameHash = std::map<std::string, Interface::UnitIndex>();
		EntNameHash = std::map<std::string, entt::entity>();
	}

	template<typename typeComp,bool asPrototype>
	void EmplaceEntityIfNeeded(entt::entity entity, PathBindJsonValue onePrototypeEntityData);
	template<typename typeComp, bool asPrototype>
	void EmplaceEntityIfNeeded(entt::entity entity, Interface::EntityInitData* pInitData);
	template<typename typeComp, bool asPrototype>
	void EmplaceEntity(entt::entity entity, Interface::EntityInitData* pInitData);
	template<bool asPrototype>
	entt::entity EmplaceFromJson(PathBindJsonValue onePrototypeEntityData);
	template<bool asPrototype>
	entt::entity EmplaceFromPrototypeEntity(Interface::EntityInitData* pInitData);
	void LoadMap(std::string mapFileName);
	void LoadEntities(std::vector<std::string> fileNames);
	void LoadPlayerCorps(std::string fileName);
	void LoadUnits(std::vector<std::string> fileNames);
	void LoadBalls(std::vector<std::string> fileNames);
	void LoadMission(std::string missionFileName);
	entt::entity EmplaceUnitWithInitData(int unitIndex, Interface::EntityInitData* pInitData, DirectX::XMMATRIX* pCorePos);
	int GetShadowIndex(int x, int y, bool searchFloor);
	void DeleteWall(int x, int y);
	std::pair<entt::entity,float> GetNearestHostilingUnit(DirectX::XMVECTOR pos, Interface::HostilityTeam team);
};