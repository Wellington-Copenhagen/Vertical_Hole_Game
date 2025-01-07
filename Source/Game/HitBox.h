#pragma once
#include "Source/DirectX/DirectX.h"
#include "framework.h"
#include "Interfaces.h"
class BaseCollidbox {
public:
	DirectX::XMVECTOR RelativeCenter;
	DirectX::XMVECTOR AbsoluteCenter;
	DirectX::XMVECTOR TentativeCenter;
	BaseCollidbox(DirectX::XMVECTOR relativeCenter) {
		RelativeCenter = relativeCenter;
	}
	void UpdateAbsoluteCenter(MotionInfo* Parent);
	void UpdateTentativeCenter(MotionInfo* Parent);
	inline virtual float GetWidth() = 0;
	inline virtual float GetHeight() = 0;
	inline virtual float GetRadius() = 0;
	inline virtual BaseCollidbox* Copy() = 0;
};
class RectCollidbox: public BaseCollidbox {
private:
	float HalfWidth;
	float HalfHeight;
public:
	RectCollidbox(Json::Value fromLoad);
	RectCollidbox(float Width,float Height,DirectX::XMVECTOR relativeCenter) : BaseCollidbox(relativeCenter) {
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
	inline BaseCollidbox* Copy() override {
		return new RectCollidbox(*this);
	}
};
class CircleCollidbox : public BaseCollidbox{
private:
	float Radius;
public:
	CircleCollidbox(Json::Value fromLoad);
	CircleCollidbox(float diameter, DirectX::XMVECTOR relativeCenter) : BaseCollidbox(relativeCenter) {
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
	inline BaseCollidbox* Copy() override {
		return new CircleCollidbox(*this);
	}
};
class CollidboxArray {//ì ëΩäpå`
public:
	MotionInfo* Parent;
	std::vector<CircleCollidbox> Collidboxes;
	CollidboxArray(Json::Value fromLoad);
	void UpdateAbsoluteCenter();
	void UpdateTentativeCenter();
	CollidboxArray() {
		Parent = nullptr;
		Collidboxes = std::vector<CircleCollidbox>();
	}
	CollidboxArray(MotionInfo* aboutMotion,CollidboxArray* prototype) {
		Parent = aboutMotion;
		Collidboxes = std::vector<CircleCollidbox>();
		int size = prototype->Collidboxes.size();
		for (int i = 0; i < size; i++) {
			Collidboxes.push_back(prototype->Collidboxes[i]);
		}
	}
};
static class Collision {
public:
	inline static bool IsColliding(BaseCollidbox* A, BaseCollidbox* B);
	static bool IsColliding(CollidboxArray* A, CollidboxArray* B,bool AlwaysCollidPair);
	inline static void PenetDepth(BaseCollidbox* ToMove, BaseCollidbox* B, MotionInfo* parent);
	static void PenetDepth(CollidboxArray* ToMove, CollidboxArray* B);
};