#pragma once
#include "Source/DirectX/DirectX.h"
#include "framework.h"
#include "Interfaces.h"
#include "Component.h"
#define UnitCountPerCell 4
#define UnitSearchDivision 4
static class Hurtboxes {
public:
	// 1エリアに登録できる数で処理負荷と制度のバランスが変えられる
	// カウントだけは有効にすれば位置の調整も可能
	// 最も近い敵を探す方法を考える必要がある
	// 4x4四方の箱に各チーム1ユニットずつ登録する
	// (256/4)*(256/4)*8*4=128kB
	std::vector<entt::entity> UnitMapForSearch;

	entt::registry* pMainRegistry;
	std::bitset<WorldHeight* WorldWidth> WallIsThere;
	std::vector<entt::entity> OccupyingWalls;
	std::vector<int> OccupyingUnitCount;
	std::vector<entt::entity> OccupyingUnits;
	std::bitset<256 * 256> OccupiedMap;
	Hurtboxes() {

	}
	Hurtboxes(entt::registry* pmainregistry) {
		pMainRegistry = pmainregistry;
		WallIsThere.reset();
		OccupyingUnitCount = std::vector<int>(WorldWidth * WorldHeight);
		OccupyingWalls = std::vector<entt::entity>(WorldWidth * WorldHeight);
		OccupyingUnits = std::vector<entt::entity>(WorldWidth * WorldHeight * UnitCountPerCell);
		OccupiedMap.reset();
		UnitMapForSearch = std::vector<entt::entity>(WorldWidth * WorldHeight * TeamCount / UnitSearchDivision * UnitSearchDivision, entt::entity(0xFFFFFFFF));

		IndexLengthFromCenter = std::vector<std::tuple<int, int, float>>();
		for (int x = 0; x < WorldWidth / UnitSearchDivision; x++) {
			for (int y = UnitSearchDivision / UnitSearchDivision; y < WorldWidth; y++) {
				IndexLengthFromCenter.push_back(std::tuple<int, int, float>(x, y, std::sqrtf(x * x + y * y)));
			}
		}
		std::sort(IndexLengthFromCenter.begin(), IndexLengthFromCenter.end(), [](std::tuple<int, int, float> lhs, std::tuple<int, int, float> rhs) {
			return std::get<2>(lhs) > std::get<2>(rhs);
			});
	}
	inline void SetWall(int x, int y, entt::entity entity) {
		int pos = y * WorldWidth + x;
		WallIsThere[pos] = true;
		OccupiedMap[pos] = true;
		OccupyingWalls[pos] = entity;
	}
	inline void DeleteWall(int x, int y) {
		int pos = y * WorldWidth + x;
		WallIsThere[pos] = false;
		OccupiedMap[pos] = false;
	}
	inline void SetUnit(int x, int y, entt::entity entity,Interface::HostilityTeam team) {
		int pos = y * WorldWidth + x;
		if (OccupyingUnitCount[pos] >= 4) {
			return;
		}
		OccupyingUnits[pos * UnitCountPerCell + OccupyingUnitCount[pos]] = entity;
		OccupiedMap[pos] = true;
		OccupyingUnitCount[pos]++;

		pos = (y / UnitSearchDivision) * (WorldWidth / UnitSearchDivision) + (x / UnitSearchDivision);
		UnitMapForSearch[pos * TeamCount + team] = entity;
	}
	void DeleteUnit(int x, int y, entt::entity entity, Interface::HostilityTeam team);
	std::vector<std::tuple<int, int, float>> IndexLengthFromCenter;
	entt::entity GetNearestUnit(int x, int y, Interface::HostilityTeam unitTeam, float maximumLength);
	// ボール→壁 ボール⇔ボール エフェクト→ボール エフェクト→壁の向きでダメージが与えられる可能性がある
	bool CheckUnitCollid(entt::entity coreEntity);
	bool CheckCircleCollid(DirectX::XMVECTOR center, float radius, Interface::Damage* giveDamage,
		bool checkOnlyOnce, int team);
	bool CheckCircleInterfare(entt::entity thisUnitEntity);
	bool IsAbleToSpawn(float left, float right, float top, float bottom, float posX, float posY);
	void SetOccupation(float left, float right, float top, float bottom, float posX, float posY);
	// まずエリアは必ず凸(多少はどうにかなるかもしれんが)
	// 触れていて、なおかつエリアの間を移動する場合に経由する領域を指定し、その中からランダムに選んだ地点を経由させる
	void UpdatePassableMap();
	bool CheckSetPassable(int currentIndex,int currentSize);
	DirectX::XMVECTOR NextWayPoint(DirectX::XMVECTOR current, DirectX::XMVECTOR target);
	bool IsWayClear(DirectX::XMVECTOR* end1, DirectX::XMVECTOR* end2);
};