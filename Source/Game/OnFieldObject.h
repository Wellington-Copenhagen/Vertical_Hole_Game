#pragma once
#include "HitBox.h"
#include "Source/DirectX/DirectX.h"
#include "framework.h"
#include "Interfaces.h"

class OnFieldObject{
public:
	int GeneratedTick;
	HitboxArray Hitbox;
	int DrawCallIndex;
	IAppearanceHandle pAppearenceInstance;
	MotionInfo AboutMotion;
	IEnvironment* pEnv;
	OnFieldObject(MotionInfo* motionCorr, OnFieldObject* prototype,IEnvironment* penv){
		AboutMotion = MotionInfo::GetCorrected(&prototype->AboutMotion, motionCorr);
		DrawCallIndex = prototype->DrawCallIndex;
		Hitbox = HitboxArray(&AboutMotion, &prototype->Hitbox);
		pAppearenceInstance = penv->SetNewAppearance(prototype->DrawCallIndex,&AboutMotion);
		GeneratedTick = penv->GetCurrentTick();
		pEnv = penv;
	}
	OnFieldObject(Json::Value fromLoad);
	inline void ResetBeforeMoveLimiting() {
		AboutMotion.ResetMoveLimit();
		Hitbox.UpdateTentativeCenter();
	}


	virtual void EveryTickAction();
	virtual Damage WhenHit();
	virtual void SetDamage(Damage damage);
	virtual void ApplyMovement();

};