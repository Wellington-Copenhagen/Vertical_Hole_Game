
#include "framework.h"
#include "Source/DirectX/DirectX.h"
#include "Camera.h"

//‰ñ“]‚Ì•ûŒü‚Æ‚Í‚È‚É‚©
//ƒˆ[‰ñ“]‚ÌŒã‚Éƒsƒbƒ`‰ñ“]‚ª—ˆ‚Ä
void Camera::Update() {
	CameraMatrix = DirectX::XMMATRIX{
		{1.0f/(ZoomRate*Aspect),0.0f,0.0f,0.0f},
		{0.0f,1.0f/(ZoomRate),0.0f,0.0f},
		{0.0f,4.0f/ZoomRate,-0.2f,1.0f},
		CenterCoord
	};
	CameraMatrix = DirectX::XMMatrixTranspose(CameraMatrix);
}