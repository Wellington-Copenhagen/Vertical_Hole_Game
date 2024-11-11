#pragma once
#include "framework.h"
#include "Source/DirectX/Buffers.h"
#include "Effect.h"
#include "Character.h"
#include "Source/DirectX/DrawPipe.h"
#include "Source/DirectX/Texture.h"
#include "Loader.h"
#include "Object.h"
//キャラクター オブジェクトは回らない　見た目はエフェクトのものなら回る　キャラクターの見た目は回らない上に中心固定
// キャラクターオブジェクト、キャラクター見た目なら無加工World
// キャラクターオブジェクト、キャラクター見た目ならWorldの4列目のみ加算したもの
//エフェクト オブジェクトは回る　見た目は回らない
// エフェクトオブジェクト、エフェクト見た目なら行列同士の積
// 
//Worldに回転成分が含まれてるなら掛け算をする必要がある
//Worldに回転成分が含まれてないなら4行目の足し算で良い
class AllGraphics {
	std::vector<std::list<RectGraphic>> Rects;
	std::vector<std::list<StripGraphic>> Strips;
	std::vector<RectCommonInfo> RectCI;
	std::vector<StripCommonInfo> StripCI;
	std::unordered_map<std::string, int> RectDictionary;
	std::unordered_map<std::string, int> StripDictionary;
	AllGraphics();
	void Draw();
	std::list<RectGraphic>::iterator AddRect(int index, std::list<Object>::iterator relatedTo);
	std::list<StripGraphic>::iterator AddStrip(int index, std::list<Object>::iterator relatedTo);
	void LoadRectDefaults(std::string filePath);
	void LoadStripDefaults(std::string filePath);
};
struct RectCommonInfo {
	Texture Tex;
	RectVertexType vDatas[4];
	bool OffsetZero;
	bool OffsetRotatable;
};
struct StripCommonInfo {
	Texture Tex;
};
class RectGraphic {
	friend AllGraphics;
private:
	std::list<Object>::iterator RelativeTo;
	bool IsWorldRollable;
	DirectX::XMMATRIX* World;
	DirectX::XMMATRIX Offset;
public:
	RectGraphic(std::list<Object>::iterator itr) {
		RelativeTo = itr;
		IsWorldRollable = itr->ShapeData.rectWorld.Rollable;
		World = &(itr->ShapeData.rectWorld.World);
	}
};
class StripGraphic {
	friend AllGraphics;
private:
	std::list<Object>::iterator RelativeTo;
	int* count;
	StripVertexType* vDatas;
public:
	StripGraphic(std::list<Object>::iterator itr) {
		RelativeTo = itr;
		vDatas = itr->ShapeData.stripVertexes.VDatas;
		count = &(itr->ShapeData.stripVertexes.VCount);
	}
};