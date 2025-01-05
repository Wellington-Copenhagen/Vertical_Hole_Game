#pragma once
#include "OnFieldObject.h"
#include "Interfaces.h"
#include "CombatUnit.h"
#include "Bullet.h"
#include "Block.h"

class Environment : IEnvironment{
public:
	Environment();
	void LoadNewMission(std::string fileName);
	int GetCurrentTick() override;
	void AddCombatUnit(CombatUnitGenerateCoef* coef,MotionInfo* motionCorr) override;
	void AddBullet(BulletGenerateCoef* coef,MotionInfo* motionCorr) override;
	IAppearanceHandle SetNewAppearance(int index, MotionInfo* pParent) override;
	void ExecuteEveryTickAction();
	void CollidAndDamage();
	void MoveLimitAndMove();
	CombatUnitArray* CombatUnits;
	BulletArray* Bullets;
	BulletArray* Effects;
	Map* Walls;
	Map* Floors;
	/*
	�ω����邩
	�Փ˂��邩
	�ړ���j�Q���邩
	�_���[�W���󂯂�̂�

	���[�V�����̂悤�Ȓ��ې������������͕K�������邵���Ȃ�
	*/
};