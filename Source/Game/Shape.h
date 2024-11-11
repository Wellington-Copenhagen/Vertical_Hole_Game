#pragma once
#include "Source/DirectX/DirectX.h"
#include "Source/DirectX/Buffers.h"

class Strip {//�U���̎c���○�ȂǗp
public:
	StripVertexType* VArray;
	DirectX::XMVECTOR* Wpos;
	DirectX::XMFLOAT2* UVs;
	int VerCount;
	float HighestX;
	float LowestX;
	float HighestY;
	float LowestY;
	Strip(DirectX::XMVECTOR wpos[],DirectX::XMFLOAT2 uvs[], int verCount);
	Strip() {};
};
class Rect {//�u���b�N�p
public:
	Rect(DirectX::XMVECTOR wpos[4]);
	DirectX::XMVECTOR Wpos[4];
};
class Circle {//���̂𔺂��U���ƃL�����N�^�[�p
public:
	Circle(DirectX::XMVECTOR relCenterPos,DirectX::XMMATRIX world, float radius);
	void SetCenter(DirectX::XMVECTOR relCenterPos);
	Circle() {};
	//Relative�͌����ڂ̒��ł̈ʒu
	//Center = Relative * World;
	DirectX::XMVECTOR Center;
	float Radius;
};
//�L�����N�^�[�ƍU��
bool IsColliding(Rect inputA, Circle inputB);
bool IsColliding(Rect inputA, Circle inputB,DirectX::XMVECTOR& MoveRange);
bool IsColliding(Circle inputA, Strip inputB);
bool IsColliding(Circle inputA, Circle inputB);
//�u���b�N�ƃL�����N�^�[�A�L�����N�^�[���m�̏Փ�
bool IsPointInTriangle(DirectX::XMVECTOR point, DirectX::XMVECTOR triangle[3],bool IsClockWise);
//���_�w��͍��������と�E�����E��
bool IsPointInRect(DirectX::XMVECTOR point, DirectX::XMVECTOR rect[4]);
float VectorDistance(DirectX::XMVECTOR inputA, DirectX::XMVECTOR inputB);
//line�����v���Ŏw�肵���ꍇ�A�O������distance����
void PerpendicularFoot(DirectX::XMVECTOR lineStart, DirectX::XMVECTOR lineEnd, DirectX::XMVECTOR point,float& distance,float& ratio);