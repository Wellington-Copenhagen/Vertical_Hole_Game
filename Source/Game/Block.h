#pragma once
#include "Source/DirectX/DirectX.h"
#include "Environment.h"
class Block : public OnFieldObject {
	Block(Json::Value fromLoad);
	void EveryTickAction() override{ }
	Damage WhenHit() override{ }
	void SetDamage(Damage damage) override{ }
	void ApplyMovement() override {	}
};
//“–‚½‚è”»’è‚ª‚ ‚é
class Map{
public:
	IEnvironment* pEnv;
	std::vector<Block> Prototypes;
	std::vector<std::list<Block*>> Instances;
	Map(std::string fileName);
};