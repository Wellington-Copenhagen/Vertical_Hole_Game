#pragma once
#include "framework.h"
#include "Source/DirectX/DirectX.h"

class Camera {
public:
	DirectX::XMMATRIX CameraMatrix;
	float Aspect;//Width/Height
	DirectX::XMVECTOR CenterCoord;
	float ZoomRate;//How many block line virtically
	Camera(float WindowHeight,float WindowWidth) {
		Aspect = WindowWidth / WindowHeight;
		CenterCoord = {0,0,0,1};
		ZoomRate = 10;
	}
	void Update();
};