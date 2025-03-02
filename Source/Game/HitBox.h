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
	// 4���؂ŃG���A���Ǘ�����
	// 256*256 128*128���� 128*128���� 128*128�E�� 128*128�E�� 64*64����̍��� ...
	// �������A1*1�̃G���A��OccupiedMap�ł���肪�Ȃ��̂�OccupiedMap���g��
	// true������G���A�͐�L����Ă��Ȃ�
	// �T�C�Y�͓��䐔��̑��a�̐������floor(256*256*(4/3))-256*256=21845�ƂȂ�
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
	// �{�[������ �{�[���̃{�[�� �G�t�F�N�g���{�[�� �G�t�F�N�g���ǂ̌����Ń_���[�W���^������\��������
	bool CheckUnitCollid(Interface::EntId coreId);
	bool CheckCircleCollid(DirectX::XMVECTOR center, float radius, Interface::Damage* giveDamage,
		bool checkOnlyOnce, int team);
	bool CheckCircleInterfare(DirectX::XMVECTOR center, float radius, bool collidToWall, bool collidToBall);
	bool IsAbleToSpawn(float left, float right, float top, float bottom, float posX, float posY);
	void SetOccupation(float left, float right, float top, float bottom, float posX, float posY);
	// �܂��G���A�͕K����(�����͂ǂ��ɂ��Ȃ邩�������)
	// �G��Ă��āA�Ȃ����G���A�̊Ԃ��ړ�����ꍇ�Ɍo�R����̈���w�肵�A���̒����烉���_���ɑI�񂾒n�_���o�R������
	void UpdatePassableMap();
	bool CheckSetPassable(int currentIndex,int currentSize);
	DirectX::XMVECTOR NextWayPoint(DirectX::XMVECTOR current, DirectX::XMVECTOR target);
};