#include "Effect.h"
#include "Source/DirectX/DrawPipe.h"
#include "framework.h"
#include "Loader.h"


AllEffect::AllEffect() {
	NameToRectIndex = std::unordered_map<std::string, int>();
	NameToStripIndex = std::unordered_map<std::string, int>();
	RectEffects = std::vector<RectEffect>();
	StripEffects = std::vector<StripEffect>();
}
void AllEffect::Load(std::string rectEffectFileName, std::string stripEffectFileName) {
	LoadRectEffect(rectEffectFileName);
	LoadStripEffect(stripEffectFileName);
}
//エフェクトの名前,テクスチャのアドレス,WhenSpawn,WhenHitTocharacter,WhenHitToWall,EveryTick,width,height,シュリンクするか,当たり判定半径,当たり判定の相対位置(x,y)
void AllEffect::LoadRectEffect(std::string rectEffectFileName) {
	OneLayer::LoadFromFile(rectEffectFileName);
	RectEffects.resize(OneLayer::Root->leafNames.size());
	for (int i = 0; i < RectEffects.size(); i++) {
		OneLayer* thisLayer = OneLayer::Root->Get(OneLayer::Root->leafNames[i]);
		NameToRectIndex[OneLayer::Root->leafNames[i]] = i;
		RectEffects[i] = RectEffect("Data/Effect/" + thisLayer->Get("texPath")->GetAsString(),
			thisLayer->Get("shrink")->GetAsBool(), thisLayer->Get("width")->GetAsFloat(), thisLayer->Get("height")->GetAsFloat(),
			thisLayer->Get("hitboxRadius")->GetAsFloat(), thisLayer->Get("hitboxCenter")->GetAsVector());
		RectEffects[i].WhenSpawn = thisLayer->Get("WhenSpawn")->GetAsAttack();
		RectEffects[i].WhenHitToCharacter = thisLayer->Get("WhenHitToCharacter")->GetAsAttack();
		RectEffects[i].WhenHitToWall = thisLayer->Get("WhenHitToWall")->GetAsAttack();
		RectEffects[i].EveryTick = thisLayer->Get("EveryTick")->GetAsAttack();

	}
}
void AllEffect::LoadStripEffect(std::string stripEffectFileName) {
	OneLayer::LoadFromFile(stripEffectFileName);
	StripEffects.resize(OneLayer::Root->leafNames.size());
	for (int i = 0; i < StripEffects.size(); i++) {
		OneLayer* thisLayer = OneLayer::Root->Get(OneLayer::Root->leafNames[i]);
		NameToRectIndex[OneLayer::Root->leafNames[i]] = i;
		StripEffects[i] = StripEffect("Data/Effect/" + thisLayer->Get("texPath")->GetAsString());
		StripEffects[i].WhenSpawn = thisLayer->Get("WhenSpawn")->GetAsAttack();
		StripEffects[i].WhenHitToCharacter = thisLayer->Get("WhenHitToCharacter")->GetAsAttack();
		StripEffects[i].WhenHitToWall = thisLayer->Get("WhenHitToWall")->GetAsAttack();
		StripEffects[i].EveryTick = thisLayer->Get("EveryTick")->GetAsAttack();

	}
}
void AllEffect::EveryTickProcess() {
	for (int i = 0; i < RectEffects.size(); i++) {
		for (auto itr = RectEffects[i].Effects.begin(); itr != RectEffects[i].Effects.end(); itr++) {
			RectEffects[i].EveryTick.Run(*itr);
		}
	}
}
void AllEffect::AddRectElement(int index, DirectX::XMMATRIX verocity, float vars[4], float angle, DirectX::XMVECTOR pos, StatusEffect damage) {
	RectEachEffect toAdd = RectEachEffect(verocity, vars, angle, pos, RectEffects[index].HitboxRadius, RectEffects[index].RelHitboxPos, damage);
	RectEffects[index].Effects.push_front(toAdd);
}
void AllEffect::AddStripElement(int index, DirectX::XMMATRIX verocity, float vars[4], DirectX::XMVECTOR* wpos, DirectX::XMFLOAT2* uvs, int stripLength, StatusEffect damage) {
	StripEachEffect toAdd = StripEachEffect(verocity, vars, wpos, uvs, stripLength, damage);
	StripEffects[index].Effects.push_front(toAdd);
}
void AllEffect::Draw() {
	for (auto itr = RectEffects.begin(); itr != RectEffects.end(); itr++) {
		itr->Draw();
	}
	for (auto itr = StripEffects.begin(); itr != StripEffects.end(); itr++) {
		itr->Draw();
	}
}





















////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//これより下はEffect
RectEffect::RectEffect(std::string texFileName, bool shrinkUVarea, float width, float height, float hitboxRadius, DirectX::XMVECTOR relHitboxPos) : Effect() {
	Effects = std::list<RectEachEffect>();
	Tex.Load(texFileName);
	DirectX::XMFLOAT3 normal = DirectX::XMFLOAT3{ 0,0,-1 };
	RelHitboxPos = relHitboxPos;
	HitboxRadius = hitboxRadius;
	VDataList[0].Normal = normal;
	VDataList[1].Normal = normal;
	VDataList[2].Normal = normal;
	VDataList[3].Normal = normal;
	if (shrinkUVarea) {
		if (width > height) {
			float UVheight = height / (width * 2.0);
			VDataList[0].UV = { 0,0.5 + UVheight };
			VDataList[1].UV = { 0,0.5 - UVheight };
			VDataList[2].UV = { 1,0.5 + UVheight };
			VDataList[3].UV = { 1,0.5 - UVheight };
		}
		else {
			float UVwidth = width / (height * 2.0);
			VDataList[0].UV = { 0.5 - UVwidth,1 };
			VDataList[1].UV = { 0.5 - UVwidth,0 };
			VDataList[2].UV = { 0.5 + UVwidth,1 };
			VDataList[3].UV = { 0.5 + UVwidth,0 };
		}
	}
	else {
		VDataList[0].UV = { 0,1 };
		VDataList[1].UV = { 0,0 };
		VDataList[2].UV = { 1,1 };
		VDataList[3].UV = { 1,0 };
	}
	VDataList[0].Pos = { width / -2.0,height / -2.0 ,0 };
	VDataList[1].Pos = { width / -2.0,height / 2.0 ,0 };
	VDataList[2].Pos = { width / 2.0,height / -2.0 ,0 };
	VDataList[3].Pos = { width / 2.0,height / 2.0 ,0 };
}
StripEffect::StripEffect(std::string texFileName) : Effect() {
	Effects = std::list<StripEachEffect>();
	Tex.Load(texFileName);
}

void RectEffect::Draw() {
	DP.SetTexture(Tex);
	DP.SetRectVertexes(VDataList);
	rBuffer.UsedCount++;
	for (auto itr = Effects.begin(); itr != Effects.end(); itr++) {
		DP.AddRectInstance(itr->World);
	}
	DP.DrawAsRectTransparent();
}
void StripEffect::Draw() {
	DP.SetTexture(Tex);
	for (auto itr = Effects.begin(); itr != Effects.end(); itr++) {
		DP.AddStrip(itr->VisibleHit.Wpos, itr->VisibleHit.UVs, itr->VisibleHit.VerCount);
	}
	DP.DrawAsStripTransparent();
}
void RectEffect::DeleteElement(std::list<RectEachEffect>::iterator itr) {
	Effects.erase(itr);
}
void StripEffect::DeleteElement(std::list<StripEachEffect>::iterator itr) {
	Effects.erase(itr);
}
void RectEffect::ApplyWorldToHitBox() {
	for (auto itr = Effects.begin(); itr != Effects.end(); itr++) {
		itr->HitBox.Center = DirectX::XMVector4Transform(RelHitboxPos, itr->World);
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//これより下はEachEffect

EachEffect::EachEffect() {
	Verocity = {
		{1,0,0,0},
		{0,1,0,0},
		{0,0,1,0},
		{0,0,0,1}
	};
	Vars[0] = 0;
	Vars[1] = 0;
	Vars[2] = 0;
	Vars[3] = 0;
}
EachEffect::EachEffect(DirectX::XMMATRIX verocity, float vars[4], StatusEffect damage) {
	Verocity = verocity;
	Vars[0] = vars[0];
	Vars[1] = vars[1];
	Vars[2] = vars[2];
	Vars[3] = vars[3];
	Damage = damage;
}
RectEachEffect::RectEachEffect() {
	World = {
		{ 1,0,0,0},
		{ 0,1,0,0},
		{ 0,0,1,0},
		{ 0,0,0,1}
	};
	HitBox = Circle({ 0,0,0,0 }, World, 1);

}
RectEachEffect::RectEachEffect(DirectX::XMMATRIX verocity, float vars[4], float angle, DirectX::XMVECTOR pos, float radius, DirectX::XMVECTOR hitBoxRPos, StatusEffect damage) : EachEffect(verocity, vars, damage) {
	World = DirectX::XMMatrixRotationZ(angle);
	World.r[3] = pos;
	HitBox = Circle(hitBoxRPos, World, radius);
}
StripEachEffect::StripEachEffect() {
	VisibleHit = Strip(nullptr, nullptr, 0);
}
StripEachEffect::StripEachEffect(DirectX::XMMATRIX verocity, float vars[4], DirectX::XMVECTOR* wpos, DirectX::XMFLOAT2* uvs, int stripLength, StatusEffect damage) : EachEffect(verocity, vars, damage) {
	VisibleHit = Strip(wpos, uvs, stripLength);
}
RectEachEffect::~RectEachEffect() {

}