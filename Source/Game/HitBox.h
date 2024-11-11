#pragma once
#include "framework.h"
#include "Shape.h"
#include "Object.h"
class Hitbox {
	union ShapeType
	{
		Circle circle;
		Rect rect;
		Strip strip;
	};

	std::list<Object>::iterator ConnectedObject;
	void CalcuratePosition();
};