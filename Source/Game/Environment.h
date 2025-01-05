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
	変化するか
	衝突するか
	移動を阻害するか
	ダメージを受けるのか

	モーションのような抽象性が高い部分は必ず分けるしかない
	*/
};