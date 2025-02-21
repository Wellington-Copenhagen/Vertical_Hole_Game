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
	std::vector<int> OccupyingCharacterCount;
	std::vector < Interface::EntId> OccupyingCharacters;
	int Width;
	int Height;
	Hurtboxes() {

	}
	Hurtboxes(AllEntities* pallentities, int mapWidth, int mapHeight) {
		pAllEntities = pallentities;
		WallIsThere.reset();
		OccupyingCharacterCount = std::vector<int>(mapWidth * mapHeight);
		OccupyingWalls = std::vector<Interface::EntId>(mapWidth * mapHeight);
		OccupyingCharacters = std::vector<Interface::EntId>(mapWidth * mapHeight * 4);
		Width = mapWidth;
		Height = mapHeight;
	}
	inline void SetWall(int x, int y, Interface::EntId id) {
		int pos = y * Width + x;
		WallIsThere[pos] = true;
		Interface::OccupiedMap[pos] = true;
		OccupyingWalls[pos] = id;
	}
	inline void DeleteWall(int x, int y) {
		int pos = y * Width + x;
		WallIsThere[pos] = false;
		Interface::OccupiedMap[pos] = false;
	}
	inline void SetCharacter(int x, int y, Interface::EntId id) {
		int pos = y * Width + x;
		if (OccupyingCharacterCount[pos] >= 4) {
			return;
		}
		OccupyingCharacters[pos * 4 + OccupyingCharacterCount[pos]] = id;
		Interface::OccupiedMap[pos] = true;
		OccupyingCharacterCount[pos]++;
	}
	void DeleteCharacter(int x, int y, Interface::EntId id);
	bool CheckCircleCollid(DirectX::XMVECTOR center, float radius, Interface::Damage damage, Interface::Damage* gotDamage,
		bool checkOnlyOnce, int team, bool collidToWall,
		bool collidToCharacter, bool collidToAllTeamCharacter, int CoreId);
	bool LimitateInOneBlock(DirectX::XMVECTOR* center, DirectX::XMVECTOR* moveTo, DirectX::XMVECTOR* velocity, float* limitation,
		float radius, std::vector<Interface::EntId>* pCheckedCharacter, int x, int y);
	void Limitate(DirectX::XMVECTOR* center, float radius, DirectX::XMVECTOR* velocity, Interface::EntId id);
};