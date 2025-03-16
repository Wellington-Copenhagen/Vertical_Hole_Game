#pragma once
#include "Source/DirectX/DirectX.h"
#include "framework.h"
#include "Interfaces.h"
#include "Component.h"
#define UnitCountPerCell 4
#define UnitSearchDivision 4
static class Hurtboxes {
public:
	// 1�G���A�ɓo�^�ł��鐔�ŏ������ׂƐ��x�̃o�����X���ς�����
	// �J�E���g�����͗L���ɂ���Έʒu�̒������\
	// �ł��߂��G��T�����@���l����K�v������
	// 4x4�l���̔��Ɋe�`�[��1���j�b�g���o�^����
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
	// �{�[������ �{�[���̃{�[�� �G�t�F�N�g���{�[�� �G�t�F�N�g���ǂ̌����Ń_���[�W���^������\��������
	bool CheckUnitCollid(entt::entity coreEntity);
	bool CheckCircleCollid(DirectX::XMVECTOR center, float radius, Interface::Damage* giveDamage,
		bool checkOnlyOnce, int team);
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