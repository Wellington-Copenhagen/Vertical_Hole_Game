#pragma once
#include "Source/DirectX/DirectX.h"
#include "framework.h"
#include "Interfaces.h"
#include "Component.h"
#include "Source/DirectX/StringDraw.h"
#include "DebugAssist.h"
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
	std::bitset<TeamCount* TeamCount>* pHostilityTable;
	Hurtboxes() {

	}
	Hurtboxes(entt::registry* pRegistry, std::bitset<TeamCount* TeamCount>* pHostilityTable) {
		pMainRegistry = pRegistry;
		this->pHostilityTable = pHostilityTable;
		WallIsThere.reset();
		OccupyingUnitCount = std::vector<int>((WorldWidth / CellWidth) * (WorldHeight / CellWidth));
		OccupyingWalls = std::vector<entt::entity>(WorldWidth * WorldHeight);
		OccupyingUnits = std::vector<entt::entity>((WorldWidth / CellWidth) * (WorldHeight / CellWidth) * UnitCountPerCell);
		OccupiedMap.reset();
		DebugLogOutput("Collision system Initialization succeeded.");
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
		globalStringDraw.SimpleAppend("HB updated", 0, 0, 1, { (float)x,(float)y,0,1 }, 0.5, 1, StrDrawPos::AsCenter);
	}
	void DeleteUnit(int x, int y, entt::entity entity, Interface::HostilityTeam team);
	// �{�[������ �{�[���̃{�[�� �G�t�F�N�g���{�[�� �G�t�F�N�g���ǂ̌����Ń_���[�W���^������\��������
	bool CheckUnitCollid(entt::entity coreEntity);
	bool CheckCircleCollid(entt::entity subjectEntity);
	bool CheckCircleInterfare(entt::entity thisUnitEntity);
	bool IsAbleToSpawn(float left, float right, float top, float bottom, float posX, float posY);
	void SetOccupation(float left, float right, float top, float bottom, float posX, float posY);
	// �܂��G���A�͕K����(�����͂ǂ��ɂ��Ȃ邩�������)
	// �G��Ă��āA�Ȃ����G���A�̊Ԃ��ړ�����ꍇ�Ɍo�R����̈���w�肵�A���̒����烉���_���ɑI�񂾒n�_���o�R������
	void UpdatePassableMap();
	bool CheckSetPassable(int currentIndex,int currentSize);
	DirectX::XMVECTOR NextWayPoint(DirectX::XMVECTOR current, DirectX::XMVECTOR target);
	bool IsWayClear(DirectX::XMVECTOR* end1, DirectX::XMVECTOR* end2);
};