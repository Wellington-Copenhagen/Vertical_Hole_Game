#pragma once
#include "Source/DirectX/DirectX.h"
#include "framework.h"
// Get Setで変える
// 色
// 位置
// 直前のMotionInfo
// 
// 見た目や当たり判定や派生処理間でデータを共有できるオブジェクト
// 
class MotionInfo {
public:
	DirectX::XMMATRIX WorldMatrix;
	DirectX::XMMATRIX Velocity;
	float MoveLimit;
	int CollisionGroup;
	int TexIndex;
	bool Delete;
	// 回転部(A):移動部(B)*回転部(C):移動部(D) = 回転部(E):移動部(F)
	// E = A*C
	// F = B*C+(ABの44が1なら D)
	// 
	// 
	// 
	inline void UpdateWorldMatrix() {
		WorldMatrix = WorldMatrix + (Velocity * MoveLimit);
	}
	inline void ResetMoveLimit() {
		MoveLimit = 1.0;
	}
	static inline DirectX::XMMATRIX RotateOnly3x3(float heading, DirectX::XMMATRIX input) {
		DirectX::XMVECTOR pos = input.r[3];
		input = input * DirectX::XMMatrixRotationZ(heading);
		input.r[3] = pos;
	}
	// 基本的には親オブジェクトのMotionInfoを突っ込めば問題ないはず
	// 方向は速度も状態もcoefのworld基準となる
	// 
	static MotionInfo GetCorrected(MotionInfo* prototype,MotionInfo* coef) {
		MotionInfo output;
		output.CollisionGroup = coef->CollisionGroup;
		output.TexIndex = coef->TexIndex;
		output.Velocity = prototype->Velocity;//回転速度は原型依存 
		//方向は原形のものを修正のworldの回転角で修正し修正の速度を足す
		output.Velocity.r[3] = DirectX::XMVector4Transform(output.Velocity.r[3], coef->WorldMatrix);
		output.Velocity.r[3] = DirectX::XMVectorAdd(output.Velocity.r[3], coef->Velocity.r[3]);
		output.WorldMatrix = DirectX::XMMatrixMultiply(prototype->WorldMatrix, coef->WorldMatrix);
		return output;
	}
};
struct Damage {
	float physical;
	Damage() {
		physical = 1;
	}
	static Damage GetCorrectedDamage(Damage* Basic, Damage* Coef) {
		Damage output;
		output.physical = Basic->physical * Coef->physical;
		return output;
	}
};
//プロトタイプにないデータで指定したいものはすべてこれ経由
struct CombatUnitGenerateCoef {
	int PrototypeIndex;
	float HealthMultiply;
	Damage DamageMultiply;
	float SpeedMultiply;
	CombatUnitGenerateCoef(int index) {
		PrototypeIndex = index;
		HealthMultiply = 1;
		DamageMultiply = Damage();
		SpeedMultiply = 1;
	}
};
//プロトタイプにないデータで指定したいものはすべてこれ経由
struct BulletGenerateCoef {
	int PrototypeIndex;
	Damage DamageCoef;
	BulletGenerateCoef(int index) {
		PrototypeIndex = index;
		DamageCoef = Damage();
	}
};
struct TimeCounter {
	int CurrentTick;
	TimeCounter() {
		CurrentTick = 0;
	}
	void Reset() {
		CurrentTick = 0;
	}
	void CountUp() {
		CurrentTick++;
	}
};
class IAppearanceHandle {
public:
	virtual void Delete(){}
	virtual void SetTexIndex(int index){}
};
//描画バッファに入れる構造体
struct RectInstanceType{
public:
	DirectX::XMFLOAT4 World1;
	DirectX::XMFLOAT4 World2;
	DirectX::XMFLOAT4 World3;
	DirectX::XMFLOAT4 World4;
};
/*
struct ColorVarRectInstanceType {
public:
	DirectX::XMFLOAT4 World1;
	DirectX::XMFLOAT4 World2;
	DirectX::XMFLOAT4 World3;
	DirectX::XMFLOAT4 World4;
	DirectX::XMFLOAT4 Color1;
	DirectX::XMFLOAT4 Color2;
};
*/
struct RectDrawCallType{
public:
	DirectX::XMFLOAT2 UV;
	DirectX::XMFLOAT3 Pos;
};
/*
struct ColorVarRectDrawCallType {
public:
	DirectX::XMFLOAT2 UV;
	DirectX::XMFLOAT3 Pos;
};
*/
struct ConstantType
{
public:
	DirectX::XMFLOAT4X4 ViewProjection;
};
//描画バッファに入れる構造体
class IAllGraphicBuffer {
public:
	//取得するごとにインデックスを進めること！
	virtual RectDrawCallType* GetNextRectDCPointer();
	virtual RectInstanceType* GetNextRectIPointer();
};
class IEnvironment {
public:
	virtual int GetCurrentTick();
	virtual void AddCombatUnit(CombatUnitGenerateCoef* coef, MotionInfo* motionCorr);
	virtual void AddBullet(BulletGenerateCoef* coef, MotionInfo* motionCorr);
	virtual IAppearanceHandle SetNewAppearance(int index,MotionInfo* pParent);
};
class IAllAppearance {
	virtual void Load(std::string AppearanceFileName);
	virtual IAppearanceHandle AddNew(MotionInfo* pWorld, int drawCallIndex);
	virtual void Draw();
};
//必要なインターフェイス
//Bullet生成用
//CombatUnit生成用
//Appearance生成用
//Hitbox生成用

struct EntityPointer {
	int Archetype;
	UINT16 Index;
};
typedef int EntityIdentifyN;