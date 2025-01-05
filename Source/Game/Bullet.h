#pragma once
#include "Source/DirectX/DirectX.h"
#include "Environment.h"
#include "Interfaces.h"
//•Ô‚è’l‚Í’eŠÛ‚ÌÁ‹ŽŽwŽ¦
class Bullet : public OnFieldObject {
public:
	Damage BaseDamage;
	Damage DamageMultiply;
	BaseMovement* Movement;
	BaseHitAction* HitAction;
	BaseWhileMove* WhileMove;
	bool IsPenerativeToWall;
	bool IsPenerativeToUnit;
	int DieTick;
	Bullet(Json::Value fromLoad);
	Bullet(MotionInfo* motionCorr, Bullet* prototype, IEnvironment* penv, BulletGenerateCoef* coef)
		: OnFieldObject(motionCorr, prototype, penv) {
		Movement = prototype->Movement;
		HitAction = prototype->HitAction;
		WhileMove = prototype->WhileMove;
		IsPenerativeToWall = prototype->IsPenerativeToWall;
		IsPenerativeToUnit = prototype->IsPenerativeToUnit;
		BaseDamage = prototype->BaseDamage;
		DamageMultiply = coef->DamageCoef;
	}
	void EveryTickAction() override;
	Damage WhenHit() override;
	void SetDamage(Damage damage) override;
	void ApplyMovement() override {
		AboutMotion.UpdateWorldMatrix();
		Hitbox.UpdateAbsoluteCenter();
	}
};
class BulletArray{
public:
	IEnvironment* pEnv;
	std::vector<Bullet> Prototypes;
	std::vector<std::list<Bullet*>> Instances;
	BulletArray(std::string fileName);
	void CreateBulletInstance(BulletGenerateCoef* coef,MotionInfo* initialMotion);
	inline void ExecuteEveryTickAction();
	inline void ResetBeforeMoveLimiting();
	inline void ApplyMovement();
};
class BaseMovement {
public:
	IEnvironment pEnv;
	virtual void Change(Bullet* parent);
	BaseMovement(IEnvironment penv) {
		pEnv = penv;
	}
};
class BaseChange {
public:
	IEnvironment pEnv;
	virtual void Change(Bullet* parent);
	BaseChange(IEnvironment penv) {
		pEnv = penv;
	}
};
class BaseHitAction {
public:
	virtual void Change(Bullet* parent);
	BaseChange* Execution;
	BaseHitAction(Json::Value fromLoad, IEnvironment penv) {

	}
};
class BaseWhileMove {
public:
	virtual void Change(Bullet* parent);
	BaseChange* Execution;
	BaseWhileMove(Json::Value fromLoad, IEnvironment penv) {

	}
	int Start;
	int End;
	int Step;
};
class BallisticDecVel : public BaseMovement {
public:
	DirectX::XMMATRIX M;
	void Change(Bullet* parent) override;
	BallisticDecVel(Json::Value fromLoad, IEnvironment penv):BaseMovement(penv) {

	}
};
class BulletEmission : public BaseChange {
public:
	BulletGenerateCoef EmitBulletData;
	int HowManyGenerate;
	float Width;
	void Change(Bullet* parent) override;
	BulletEmission(Json::Value fromLoad, IEnvironment penv): BaseMovement(penv) {

	}
};
class NonPenetrativeHit : public BaseChange {
public:
	void Change(Bullet* parent) override;
	NonPenetrativeHit(Json::Value fromLoad, IEnvironment penv) : BaseMovement(penv) {

	}
};