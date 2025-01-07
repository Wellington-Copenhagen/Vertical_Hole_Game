#pragma once
#include "framework.h"
#include "Interfaces.h"
#include "HitBox.h"
namespace Component {
	struct RectAppearance {
		IAppearanceHandle pAppearance;
		float TexIndex;
		RectAppearance(Json::Value fromLoad);
	};
	struct WorldPosition {
		DirectX::XMVECTOR World;
	};
	struct CircleHitbox {
		CollidboxArray Hitbox;
	};
	struct CircleHurtbox {
		CircleCollidbox Hurtbox;

	};
	struct RectHurtbox {
		RectCollidbox Hurtbox;
	};
	struct CharacterData {
		float HP;
		float Attack;
		float TilePerTick;
	};
	struct Damage {
		float physical;
	};
	struct HitEffect {
		EntityIdentifyN PrototypeEIN;
		float WidthRadian;
		UINT16 EjectCount;
	};
	struct Trajectory {
		EntityIdentifyN PrototypeEIN;
		UINT16 EjectStartTick;
		UINT16 EjectEndTick;
		UINT16 EjectInterval;
		float WidthRadian;
		UINT16 EjectCount;
	};
	struct LinearAcceralation {
		DirectX::XMMATRIX AccelMatrix;
	};
	struct Move {
		DirectX::XMVECTOR Velocity;
		float LimitationCoef;
	};
	struct Rotate {
		DirectX::XMMATRIX AngulerVelocity;
	};
};