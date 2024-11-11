#pragma once
#include "Source/DirectX/DirectX.h"
#include "Source/DirectX/Texture.h"
#include "Source/DirectX/Buffers.h"
#include "Shape.h"
class AllPlate {
private:
	Plate* plates;
	// 移動開始時点で移動先プレートを占有する
	// 
	// 半分を超えた時点で移動元プレートの占有を取り消す
	// 移動中に移動方向を変更した場合は変更先が占有済みなら変化させない
	// 移動先が占有済みでないなら元の移動予定プレートの占有を取り消し、新しい移動先プレートを占有する
	// 移動方向は新しい移動先プレートの中心になるように補正する
	bool* IsOccupied;// 
	int Width;
	int Height;
public:
	AllPlate(std::string fileName);
	void CheckCollision();
	void Draw();
};
//1種類のテクスチャあたり
class Plate {
public:
	Texture Texture;
	RectVertexType NormalUV;
	Rect* PosList;
	float Passability;
	int UsedCount;
	Plate(int usedCount);
	void SetToBufferData();
};