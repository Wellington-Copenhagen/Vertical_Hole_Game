#pragma once
#include "Source/DirectX/DirectX.h"
#include "framework.h"
#include "Interfaces.h"
#include "Entity.h"
#define WorldWidth 256
#define WorldHeight 256
static class Hurtboxes {
public:
	AllEntities* pAllEntities;
	std::bitset<WorldHeight* WorldWidth> WallIsThere;
	std::vector<Interface::EntId> OccupyingWalls;
	std::vector<int> OccupyingUnitCount;
	std::vector <Interface::EntId> OccupyingUnits;
	std::bitset<256 * 256> OccupiedMap;
	// 4分木でエリアを管理する
	// 256*256 128*128左下 128*128左上 128*128右下 128*128右上 64*64左上の左上 ...
	// しかし、1*1のエリアはOccupiedMapでも問題がないのでOccupiedMapを使う
	// trueがあるエリアは占有されていない
	// サイズは等比数列の総和の性質よりfloor(256*256*(4/3))-256*256=21845となる
	// 2.5kB
	std::bitset<21845> PassableMap;
	int Width;
	int Height;
	Hurtboxes() {

	}
	Hurtboxes(AllEntities* pallentities, int mapWidth, int mapHeight) {
		pAllEntities = pallentities;
		WallIsThere.reset();
		OccupyingUnitCount = std::vector<int>(mapWidth * mapHeight);
		OccupyingWalls = std::vector<Interface::EntId>(mapWidth * mapHeight);
		OccupyingUnits = std::vector<Interface::EntId>(mapWidth * mapHeight * 4);
		OccupiedMap.reset();
		Width = mapWidth;
		Height = mapHeight;
	}
	inline void SetWall(int x, int y, Interface::EntId id) {
		int pos = y * Width + x;
		WallIsThere[pos] = true;
		OccupiedMap[pos] = true;
		OccupyingWalls[pos] = id;
	}
	inline void DeleteWall(int x, int y) {
		int pos = y * Width + x;
		WallIsThere[pos] = false;
		OccupiedMap[pos] = false;
	}
	inline void SetUnit(int x, int y, Interface::EntId id) {
		int pos = y * Width + x;
		if (OccupyingUnitCount[pos] >= 4) {
			return;
		}
		OccupyingUnits[pos * 4 + OccupyingUnitCount[pos]] = id;
		OccupiedMap[pos] = true;
		OccupyingUnitCount[pos]++;
	}
	void DeleteUnit(int x, int y, Interface::EntId id);
	// ボール→壁 ボール⇔ボール エフェクト→ボール エフェクト→壁の向きでダメージが与えられる可能性がある
	bool CheckUnitCollid(Interface::EntId coreId);
	bool CheckCircleCollid(DirectX::XMVECTOR center, float radius, Interface::Damage* giveDamage,
		bool checkOnlyOnce, int team);
	bool CheckCircleInterfare(DirectX::XMVECTOR center, float radius, bool collidToWall, bool collidToBall);
	bool IsAbleToSpawn(float left, float right, float top, float bottom, float posX, float posY);
	void SetOccupation(float left, float right, float top, float bottom, float posX, float posY);
	// まずエリアは必ず凸(多少はどうにかなるかもしれんが)
	// 触れていて、なおかつエリアの間を移動する場合に経由する領域を指定し、その中からランダムに選んだ地点を経由させる
	void UpdatePassableMap();
	bool CheckSetPassable(int currentIndex,int currentSize);
	DirectX::XMVECTOR NextWayPoint(DirectX::XMVECTOR current, DirectX::XMVECTOR target);
};