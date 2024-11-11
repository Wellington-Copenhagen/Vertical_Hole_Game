#pragma once
#include "framework.h"
#include "Source/DirectX/DirectX.h"

class Camera {
public:
	DirectX::XMMATRIX Rotation;
	DirectX::XMVECTOR ParallelMovement;
	float nearZ;
	float farZ;
	float aspect;
	float FOV;
	float rotationXaxis;
	float rotationYaxis;
	float rotationZaxis;
	//•½sˆÚ“®¨‰ñ“]¨Z‰“‹ß¨FOV‚Ì‡‚Åˆ—‚·‚é
	DirectX::XMFLOAT4X4 GetViewProjectionMatrix();
	DirectX::XMMATRIX GetViewProjectionMatrix2();
	DirectX::XMMATRIX GetRotationMatrix();
	DirectX::XMFLOAT2 GetEyePos();
	static Camera* pInstance;
	static Camera& GetInstance() {
		if (pInstance == nullptr) {
			pInstance = new Camera();
		}
		return *pInstance;
	}
private:
	Camera();
};
#define CAMERA Camera::GetInstance()