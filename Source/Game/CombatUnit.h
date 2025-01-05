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
// UŒ‚E”z’u“]Š·EUŒ‚‚Ì‚½‚ß‚ÌˆÚ“®
// UŒ‚‚Í‹–‰Â§
// UŒ‚‚Ì‚½‚ß‚ÌˆÚ“®‚Í‹——£‚ğ’è‚ß‚Ä‚»‚ê‚ğ‰z‚¦‚éˆÚ“®‚Í‚Å‚«‚È‚¢
// ”z’u“]Š·(”CˆÓ‚ÌêŠ‚Ö‚ÌˆÚ“®)
// ÀÛ‚É‚Æ‚éƒ‚[ƒVƒ‡ƒ“
// S‘©ŠÔ
// —Dæ‡ˆÊ
// S‘©‰ğœ‚²‚Æ‚ÉÄ“x”»’è‚ğs‚¤
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
	//ˆÚ“®–Ú•W
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