
#include "framework.h"
#include "Interfaces.h"
#include "Source/DirectX/DirectX.h"
#include "Camera.h"

//回転の方向とはなにか
//ヨー回転の後にピッチ回転が来て
DirectX::XMMATRIX Camera::CameraMatrix;
float Camera::Aspect;//Width/Height
DirectX::XMVECTOR Camera::CenterCoord;
float Camera::ZoomRate;//How many block line virtically
float Camera::Theta;
void Camera::Update() {
	/*
	DirectX::XMMATRIX scale{
		2.0f / (ZoomRate * Aspect),0.0f,0.0f,0.0f,
		0.0f,2.0f / ZoomRate,0.0f,0.0f,
		0.0f,0.0f,-1.0f / (ZoomRate*(1+cosf(Theta))),0.0f,
		0.0f,0.0f,0.0f,1.0f,
	};
	DirectX::XMMATRIX rotate = DirectX::XMMatrixRotationX(Theta-PI/2);
	CameraMatrix = scale * rotate;
	CameraMatrix.r[3] = DirectX::XMVectorAdd(
		{
			0.0f,
			(1-cosf(Theta)) * - 1 * cosf(Theta),
			(1 - cosf(Theta))* sinf(Theta),
			0.0f
		},
		CenterCoord);
	*/

	CameraMatrix = DirectX::XMMATRIX{
		2.0f/(ZoomRate*Aspect),0.0f,0.0f,0.0f,
		0.0f,2.0f/ZoomRate,0.0f,0.0f,
		0.0f,2.0f*cosf(Theta)/(ZoomRate*sinf(Theta)),-1.0f / (ZoomRate * sinf(Theta)),0.0f,
		-1 * CenterCoord.m128_f32[0] * 2.0f / (ZoomRate * Aspect),-1 * CenterCoord.m128_f32[1] * 2.0f / (ZoomRate),1.0f,1.0f,
	};
	CameraMatrix = DirectX::XMMATRIX{
		2.0f / (ZoomRate * Aspect),0.0f,0.0f,0.0f,
		0.0f,2.0f / ZoomRate,0.0f,0.0f,
		0.0f,0.0f,-1.0f / ZoomRate,0.0f,
		-1 * CenterCoord.m128_f32[0] * 2.0f / (ZoomRate * Aspect),-1 * CenterCoord.m128_f32[1] * 2.0f / (ZoomRate),0.9f,1.0f,
	};
	//CameraMatrix.r[3] = DirectX::XMVectorSubtract(CameraMatrix.r[3], DirectX::XMVectorScale(CenterCoord,1/ZoomRate));
	CameraMatrix.r[3].m128_f32[3] = 1.0f;
	//[3][2]にある分は高いところにあるものが奥に行くようにするためのもの
	//CameraMatrix = DirectX::XMMatrixTranspose(CameraMatrix);
}
void Camera::MoveCamera(DirectX::XMVECTOR MoveTo) {
	CenterCoord= DirectX::XMVectorAdd(CenterCoord, MoveTo);
}
Camera::Camera() {
	Camera::Aspect = (float)WindowWidth / (float)WindowHeight;
	Camera::CenterCoord = { 0,0,0,1 };
	Camera::ZoomRate = 64;
	Camera::Theta = PI / 2;
}
DirectX::XMVECTOR Camera::MousePointingWorldPos(float mouseX, float mouseY) {
	DirectX::XMVECTOR output = 
		{
			((float)mouseX / (float)WindowWidth - 0.5f) * (Camera::ZoomRate * Camera::Aspect)
			 + CenterCoord.m128_f32[0],
			(0.5f - (float)mouseY / (float)WindowHeight) * Camera::ZoomRate
			+ CenterCoord.m128_f32[1],
			0.0f,
			1.0f
		};
	return output;
}