#pragma once
#include "Source/DirectX/DirectX.h"
#include "framework.h"
#include "Interfaces.h"
#include "Component.h"
#include "Source/DirectX/StringDraw.h"
extern GraphicalStringDraw globalStringDraw;
#define UnitCountPerCell 1
#define CellWidth 1
class Hurtboxes {
public:

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
		OccupyingUnitCount = std::vector<int>((WorldWidth / CellWidth) * (WorldHeight / CellWidth));
		OccupyingWalls = std::vector<entt::entity>(WorldWidth * WorldHeight);
		OccupyingUnits = std::vector<entt::entity>((WorldWidth / CellWidth) * (WorldHeight / CellWidth) * UnitCountPerCell);
		OccupiedMap.reset();

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
		int pos = (y / CellWidth) * (WorldWidth / CellWidth) + x / CellWidth;
		if (OccupyingUnitCount[pos] >= 4) {
			return;
		}
		OccupyingUnits[pos * UnitCountPerCell + OccupyingUnitCount[pos]] = entity;
		OccupiedMap[pos] = true;
		OccupyingUnitCount[pos]++;

	}
	void DeleteUnit(int x, int y, entt::entity entity, Interface::HostilityTeam team);
	// ボール→壁 ボール⇔ボール エフェクト→ボール エフェクト→壁の向きでダメージが与えられる可能性がある
	bool CheckUnitCollid(entt::entity coreEntity);
	bool CheckCircleCollid(entt::entity subjectEntity);
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