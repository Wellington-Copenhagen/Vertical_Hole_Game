#pragma once
#include "framework.h"
#include "Interfaces.h"
#include "Routing.h"
#include "Component.h"
#include "Source/DirectX/StringDraw.h"
#include "DebugAssist.h"
#include "PathBindJsonValue.h"
class UnitInfo;
class BallInfo;
class Entities {
public:
	entt::registry Registry;
	entt::registry PrototypeRegistry;

	std::vector<int> UnitCountEachTeam;
	std::vector<std::vector<int>> UnitCountAreaTeam;
	std::vector <UnitInfo> UnitInfos;
	std::vector<BallInfo> BallInfos;
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
		UnitInfos = std::vector<UnitInfo>();
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
struct BallInfo {
	std::vector<entt::entity> Prototypes;
	std::vector<DirectX::XMMATRIX> PosFrom;
	std::vector<int> Parent;
	std::vector<int> Connected1;
	std::vector<int> Connected2;
	float Weight;
	float MovePower;
	float RotatePower;
	BallInfo() {}
	BallInfo(PathBindJsonValue jsonOfThisBall, std::map<std::string, entt::entity>* pEntNameHash) {
		Weight = jsonOfThisBall.get("weight").asFloat();
		MovePower = jsonOfThisBall.get("movePower").asFloat();
		RotatePower = jsonOfThisBall.get("rotatePower").asFloat();
		for (int i = 0; i < jsonOfThisBall.get("entities").size(); i++) {
			PathBindJsonValue jsonOfThisEntity = jsonOfThisBall.get("entities")[i];
			std::string entityName = jsonOfThisEntity.get("entityName").asString();
			if (pEntNameHash->contains(entityName)) {
				Prototypes.push_back((*pEntNameHash)[entityName]);
			}
			else {
				DebugLogOutput("Json Error:Entity name \"{}\" not found. {}", entityName, jsonOfThisEntity.get("entityName").Path);
				throw("");
			}
			float scale = jsonOfThisEntity.tryGetAsFloat("scale", 1);
			float rotation = jsonOfThisEntity.tryGetAsFloat("rotation", 0);
			DirectX::XMVECTOR parallel = jsonOfThisEntity.tryGetAsVector("parallel", { 0,0,0,1 });
			PosFrom.push_back(Interface::GetMatrix(parallel, rotation, scale));
			int parent = jsonOfThisEntity.tryGetAsInt("parent", -1);
			// parent==-1はcoreをparentとする
			// また、自身を指定している場合は移動自由なものとする
			int connected1 = jsonOfThisEntity.tryGetAsInt("connected1", 0);
			int connected2 = jsonOfThisEntity.tryGetAsInt("connected2", 0);
			Parent.push_back(parent);
			Connected1.push_back(connected1);
			Connected2.push_back(connected2);
		}
	}
};
struct UnitInfo {
	entt::entity CorePrototype;
	DirectX::XMMATRIX World;
	std::vector<int> Ballindices;
	int ThumbnailTextureIndex;
	float Weight;
	float MovePower;
	float RotatePower;
	UnitInfo() {

	}
	UnitInfo(PathBindJsonValue jsonOfThisUnit, std::map<std::string, entt::entity>* pEntNameHash, std::map<std::string, int>* pBallNameHash, std::vector<BallInfo>* pBallInfos) {
		std::string coreName = jsonOfThisUnit.get("coreName").asString();
		if (pEntNameHash->contains(coreName)) {
			CorePrototype = (*pEntNameHash)[coreName];
		}
		else {
			DebugLogOutput("Json Error:Entity name \"{}\" not found. {}", coreName, jsonOfThisUnit.get("coreName").Path);
			throw("");
		}
		float ratio = jsonOfThisUnit.tryGetAsFloat("ratio", 1);
		World = Interface::GetMatrix({ 0,0,0,1 }, 0, ratio);
		Weight = 0;
		MovePower = 0;
		RotatePower = 0;
		Ballindices = std::vector<int>();
		for (int i = 0; i < 7; i++) {
			std::string ballName = jsonOfThisUnit.get("ballNames")[i].asString();
			if (pBallNameHash->contains(ballName)) {
				int ballIndex = (*pBallNameHash)[ballName];
				Ballindices.push_back(ballIndex);
				Weight += (*pBallInfos)[ballIndex].Weight;
				RotatePower += (*pBallInfos)[ballIndex].RotatePower;
				MovePower += (*pBallInfos)[ballIndex].MovePower;
			}
			else {
				DebugLogOutput("Json Error:Ball name \"{}\" not found. {}", ballName, jsonOfThisUnit.get("ballNames")[i].Path);
				throw("");
			}
		}
		RotatePower = RotatePower / Weight;
		MovePower = MovePower / Weight;
	}
};