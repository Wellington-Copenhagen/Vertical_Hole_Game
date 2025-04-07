#pragma once
#include "framework.h"
#include "Source/DirectX/DirectX.h"

static class Camera {
public:
	static DirectX::XMMATRIX CameraMatrix;
	static float Aspect;//Width/Height
	static DirectX::XMVECTOR CenterCoord;
	static float ZoomRate;//How many block line virtically
	static float Theta;
	Camera();
	static void Update();
	static void MoveCamera(DirectX::XMVECTOR MoveTo);
	static DirectX::XMVECTOR MousePointingWorldPos(float mouseX, float mouseY);
};