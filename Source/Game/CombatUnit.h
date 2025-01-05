#pragma once
#include "Source/DirectX/DirectX.h"
#include "Environment.h"
#include "Interfaces.h"
//0:HP
//1:Attack
//2:Defence
class CombatUnit : public OnFieldObject{
public:
	CombatUnit(Json::Value fromLoad);
	CombatUnit(MotionInfo* motionCorr, CombatUnit* prototype, IEnvironment* penv,CombatUnitGenerateCoef* coef)
		: OnFieldObject(motionCorr,prototype,penv){
		HP = prototype->HP * coef->HealthMultiply;
		DamageCoef = Damage::GetCorrectedDamage(&prototype->DamageCoef,&coef->DamageMultiply);
		TilePerSec = prototype->TilePerSec * coef->SpeedMultiply;
	}
	float HP;
	Damage DamageCoef;
	float TilePerSec;
	int NextActionSelectTick;
	void EveryTickAction() override;
	Damage WhenHit() override;
	void SetDamage(Damage damage) override;
	void ApplyMovement() override {
		AboutMotion.UpdateWorldMatrix();
		Hitbox.UpdateAbsoluteCenter();
	}
};
class Order {
	DirectX::XMVECTOR MovingTarget;
	DirectX::XMVECTOR FixedPoint;
	bool AbleToAttack;

};
// �U���E�z�u�]���E�U���̂��߂̈ړ�
// �U���͋���
// �U���̂��߂̈ړ��͋������߂Ă�����z����ړ��͂ł��Ȃ�
// �z�u�]��(�C�ӂ̏ꏊ�ւ̈ړ�)
// ���ۂɂƂ郂�[�V����
// �S������
// �D�揇��
// �S���������Ƃɍēx������s��
class CombatUnitArray{
public:
	IEnvironment* pEnv;
	std::vector<CombatUnit> Prototypes;
	std::vector<std::list<CombatUnit*>> Instances;
	CombatUnitArray(std::string fileName);
	void CreateCombatUnitInstance(CombatUnitGenerateCoef* coef,MotionInfo* initialMotion);
	inline void ExecuteEveryTickAction();
	inline void ResetBeforeMoveLimiting();
	inline void ApplyMovement();
};
class BaseAction {
public:
	CombatUnit* parent;
	virtual bool Act();
	BaseAction(CombatUnit* parent);
};
class MoveToArea : public BaseAction{
	//�ړ��ڕW
	bool Act() override;
};
class FreeToMove : public BaseAction {
	bool Act() override;
};
class CeaseFire : public BaseAction {
	bool Act() override;
};
class FireAtWill : public BaseAction {
	bool Act() override;
};