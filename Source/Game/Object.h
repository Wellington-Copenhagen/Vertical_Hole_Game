#pragma once
#include "framework.h"
#include "Source/DirectX/Buffers.h"
#include "Shape.h"
#include "Source/DirectX/Buffers.h"
struct Damage {
	int Direct;
	int PoisonStrength;
	int PoisonTime;
	float DamageCut;
	int ShieldCount;
	int RegenerationStrength;
	int RegenerationTime;
	float DamageUp;
	float MoveSpeedCoef;
};
struct BuffDebuff {
	int HPdamage;
	int PoisonStrength;
	int PoisonTime;
	float DamageCut;
	int ShieldCount;
	int RegenerationStrength;
	int RegenerationTime;
	float DamageUp;
	float MoveSpeedCoef;
	void ApplyDamage(Damage damage);
};
struct Attack {
	typedef void (*pAttack)(Object&, int&, int&);
	static pAttack Attacks[];
	static void BowShot(Object&, int&, int&);
	static void Slash(Object&, int&, int&);
	static void Stab(Object&, int&, int&);
	static void Guard(Object&, int&, int&);
};
struct HitAction {
	typedef void (*pHitAction)(Object&, std::list<Object>::iterator, int&);
	static pHitAction pHitActions[];
	static void Penetrate(Object&, std::list<Object>::iterator, int&);
	static void Stop(Object&, std::list<Object>::iterator, int&);
	static void Explode(Object&, std::list<Object>::iterator, int&);
};
struct EveryAction {
	typedef void (*pEveryAction)(Object&, int&, int&);
	static pEveryAction pEveryActions[];
	static void Thrust(Object&, int&, int&);
	static void Uniform(Object&, int&, int&);
	static void Wind(Object&, int&, int&);
};
union ShapeDataType{
public:
	RectWorld rectWorld;
	StripVertexes stripVertexes;
};
struct RectWorld{
	DirectX::XMMATRIX World;
	bool Rollable;
};
struct StripVertexes {
	StripVertexType* VDatas;
	int VCount;
};
union ObjectState {
	CharacterState cState;
	EffectState eState;
};
struct CharacterState {
	BuffDebuff buffDebuff;
};
struct EffectState {
	Damage damage;
};
struct CharacterCommonInfo {
	int DamageLevel;
	int HealthLevel;
	int SpeedLevel;
	Attack::pAttack Attack1;
	Attack::pAttack Attack2;
	Attack::pAttack Attack3;
	Attack::pAttack Attack4;
};
struct EffectCommonInfo {

};
class Object {
public:
	ShapeDataType ShapeData;

};
class AllObjects {
	std::vector<std::list<Object>> Characters;
};

//1つのオブジェクトに当たり判定は1つだが見た目が1つとは限らない
//見た目はバッファ形式で保存しておきたい
//衝突後の処理は当たり判定の形以外の要素が関わる
//今の方式から描画部分だけ分離させる