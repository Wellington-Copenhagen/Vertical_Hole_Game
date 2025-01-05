#pragma once
#include "Source/DirectX/DirectX.h"
#include "framework.h"
#include "Interfaces.h"
class BaseHitbox {
public:
	DirectX::XMVECTOR RelativeCenter;
	DirectX::XMVECTOR AbsoluteCenter;
	DirectX::XMVECTOR TentativeCenter;
	BaseHitbox(DirectX::XMVECTOR relativeCenter) {
		RelativeCenter = relativeCenter;
	}
	void UpdateAbsoluteCenter(MotionInfo* Parent);
	void UpdateTentativeCenter(MotionInfo* Parent);
	inline virtual float GetWidth() = 0;
	inline virtual float GetHeight() = 0;
	inline virtual float GetRadius() = 0;
	inline virtual BaseHitbox* Copy() = 0;
};
class RectHitbox: public BaseHitbox {
private:
	float HalfWidth;
	float HalfHeight;
public:
	RectHitbox(Json::Value fromLoad);
	RectHitbox(float Width,float Height,DirectX::XMVECTOR relativeCenter) : BaseHitbox(relativeCenter) {
		HalfHeight = Height/2;
		HalfWidth = Width/2;
	}
	inline float GetWidth() override {
		return HalfWidth;
	}
	inline float GetHeight() override {
		return HalfHeight;
	}
	inline float GetRadius() override {
		return 0;
	}
	inline BaseHitbox* Copy() override {
		return new RectHitbox(*this);
	}
};
class CircleHitbox : public BaseHitbox{
private:
	float Radius;
public:
	CircleHitbox(Json::Value fromLoad);
	CircleHitbox(float diameter, DirectX::XMVECTOR relativeCenter) : BaseHitbox(relativeCenter) {
		Radius = diameter/2;
	}
	inline float GetWidth() override {
		return Radius;
	}
	inline float GetHeight() override {
		return Radius;
	}
	inline float GetRadius() override {
		return Radius;
	}
	inline BaseHitbox* Copy() override {
		return new CircleHitbox(*this);
	}
};
class HitboxArray {//ì ëΩäpå`
public:
	MotionInfo* Parent;
	std::vector<BaseHitbox*> Hitboxes;
	HitboxArray(Json::Value fromLoad);
	void UpdateAbsoluteCenter();
	void UpdateTentativeCenter();
	HitboxArray() {
		Parent = nullptr;
		Hitboxes = std::vector<BaseHitbox*>();
	}
	HitboxArray(MotionInfo* aboutMotion,HitboxArray* prototype) {
		Parent = aboutMotion;
		Hitboxes = std::vector<BaseHitbox*>();
		int size = prototype->Hitboxes.size();
		for (int i = 0; i < size; i++) {
			Hitboxes.push_back(prototype->Hitboxes[i]);
		}
	}
	~HitboxArray() {
		int size = Hitboxes.size();
		for (int i = 0; i < size; i++) {
			delete Hitboxes[i];
		}
	}
};
static class Collision {
public:
	inline static bool IsColliding(BaseHitbox* A, BaseHitbox* B);
	static bool IsColliding(HitboxArray* A, HitboxArray* B,bool AlwaysCollidPair);
	inline static void PenetDepth(BaseHitbox* ToMove, BaseHitbox* B, MotionInfo* parent);
	static void PenetDepth(HitboxArray* ToMove, HitboxArray* B);
};