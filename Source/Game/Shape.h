#pragma once
#include "Source/DirectX/DirectX.h"
#include "Source/DirectX/Buffers.h"

class Strip {//攻撃の残像や雷など用
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
class Rect {//ブロック用
public:
	Rect(DirectX::XMVECTOR wpos[4]);
	DirectX::XMVECTOR Wpos[4];
};
class Circle {//実体を伴う攻撃とキャラクター用
public:
	Circle(DirectX::XMVECTOR relCenterPos,DirectX::XMMATRIX world, float radius);
	void SetCenter(DirectX::XMVECTOR relCenterPos);
	Circle() {};
	//Relativeは見た目の中での位置
	//Center = Relative * World;
	DirectX::XMVECTOR Center;
	float Radius;
};
//キャラクターと攻撃
bool IsColliding(Rect inputA, Circle inputB);
bool IsColliding(Rect inputA, Circle inputB,DirectX::XMVECTOR& MoveRange);
bool IsColliding(Circle inputA, Strip inputB);
bool IsColliding(Circle inputA, Circle inputB);
//ブロックとキャラクター、キャラクター同士の衝突
bool IsPointInTriangle(DirectX::XMVECTOR point, DirectX::XMVECTOR triangle[3],bool IsClockWise);
//頂点指定は左下→左上→右下→右上
bool IsPointInRect(DirectX::XMVECTOR point, DirectX::XMVECTOR rect[4]);
float VectorDistance(DirectX::XMVECTOR inputA, DirectX::XMVECTOR inputB);
//lineを時計回りで指定した場合、外側だとdistanceが正
void PerpendicularFoot(DirectX::XMVECTOR lineStart, DirectX::XMVECTOR lineEnd, DirectX::XMVECTOR point,float& distance,float& ratio);