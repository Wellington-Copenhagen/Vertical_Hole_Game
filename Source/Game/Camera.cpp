
#include "framework.h"
#include "Source/DirectX/DirectX.h"
#include "Camera.h"

Camera* Camera::pInstance = nullptr;
//âÒì]ÇÃï˚å¸Ç∆ÇÕÇ»Ç…Ç©
//ÉàÅ[âÒì]ÇÃå„Ç…ÉsÉbÉ`âÒì]Ç™óàÇƒ
Camera::Camera() {
	Rotation = DirectX::XMMATRIX
	{
		{1,0,0,0},
		{0,1,0,0},
		{0,0,1,0},
		{0,0,0,1}
	};
	ParallelMovement = DirectX::XMVECTOR
	{
		{0,0,0,0}
	};
	nearZ = 10;
	farZ = 30000;
	aspect = 16.0/9.0;
	FOV = 3;
	rotationXaxis = 0;
	rotationYaxis = 0;
	rotationZaxis = 0;
}
DirectX::XMFLOAT4X4 Camera::GetViewProjectionMatrix(){
	DirectX::XMMATRIX viewM{
		{1,0,0,0},
		{0,1,0,0 },
		{0,0,1,0},
		{ParallelMovement.m128_f32[0]*-1,ParallelMovement.m128_f32[1] * -1,
		ParallelMovement.m128_f32[2] * -1,1}
	};
	viewM = DirectX::XMMatrixMultiply(viewM, DirectX::XMMatrixRotationY(rotationYaxis * -1));
	viewM = DirectX::XMMatrixMultiply(viewM, DirectX::XMMatrixRotationX(rotationXaxis * -1));
	viewM = DirectX::XMMatrixMultiply(viewM, DirectX::XMMatrixRotationZ(rotationZaxis * -1));
	DirectX::XMMATRIX projectionM = DirectX::XMMATRIX
	{
		nearZ / (FOV * aspect),0,0,0,
		0,nearZ / FOV,0,0,
		0,0,farZ / (farZ - nearZ),1.0f,
		0,0,-1.0f * farZ * nearZ / (farZ - nearZ),0
	};
	DirectX::XMFLOAT4X4 output;
	DirectX::XMStoreFloat4x4(&output, DirectX::XMMatrixMultiply(viewM,projectionM));
	return output;
}
DirectX::XMFLOAT2 Camera::GetEyePos() {
	DirectX::XMFLOAT2 output;
	DirectX::XMStoreFloat2(&output, ParallelMovement);
	return output;
}
DirectX::XMMATRIX Camera::GetViewProjectionMatrix2() {
	DirectX::XMMATRIX viewM = DirectX::XMMatrixLookToLH({ 0,0,0,0 }, { 0,0,1,0 }, { 0,1,0,0 });
	DirectX::XMMATRIX projectionM = DirectX::XMMATRIX
	{
		nearZ / (FOV * aspect),0,0,0,
		0,nearZ / FOV,0,0,
		0,0,farZ / (farZ - nearZ),1,
		0,0,-1 * farZ * nearZ / (farZ - nearZ),0
	};
	return DirectX::XMMatrixMultiply(viewM, projectionM);
}
DirectX::XMMATRIX Camera::GetRotationMatrix() {
	DirectX::XMMATRIX output = DirectX::XMMatrixRotationY(rotationYaxis);
	output = DirectX::XMMatrixMultiply(output, DirectX::XMMatrixRotationX(rotationXaxis));
	return output;
}