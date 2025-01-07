#pragma once
#include "Source/DirectX/DirectX.h"
#include "framework.h"
// Get Set�ŕς���
// �F
// �ʒu
// ���O��MotionInfo
// 
// �����ڂⓖ���蔻���h�������ԂŃf�[�^�����L�ł���I�u�W�F�N�g
// 
class MotionInfo {
public:
	DirectX::XMMATRIX WorldMatrix;
	DirectX::XMMATRIX Velocity;
	float MoveLimit;
	int CollisionGroup;
	int TexIndex;
	bool Delete;
	// ��]��(A):�ړ���(B)*��]��(C):�ړ���(D) = ��]��(E):�ړ���(F)
	// E = A*C
	// F = B*C+(AB��44��1�Ȃ� D)
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
	// ��{�I�ɂ͐e�I�u�W�F�N�g��MotionInfo��˂����߂Ζ��Ȃ��͂�
	// �����͑��x����Ԃ�coef��world��ƂȂ�
	// 
	static MotionInfo GetCorrected(MotionInfo* prototype,MotionInfo* coef) {
		MotionInfo output;
		output.CollisionGroup = coef->CollisionGroup;
		output.TexIndex = coef->TexIndex;
		output.Velocity = prototype->Velocity;//��]���x�͌��^�ˑ� 
		//�����͌��`�̂��̂��C����world�̉�]�p�ŏC�����C���̑��x�𑫂�
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
//�v���g�^�C�v�ɂȂ��f�[�^�Ŏw�肵�������̂͂��ׂĂ���o�R
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
//�v���g�^�C�v�ɂȂ��f�[�^�Ŏw�肵�������̂͂��ׂĂ���o�R
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
//�`��o�b�t�@�ɓ����\����
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
//�`��o�b�t�@�ɓ����\����
class IAllGraphicBuffer {
public:
	//�擾���邲�ƂɃC���f�b�N�X��i�߂邱�ƁI
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
//�K�v�ȃC���^�[�t�F�C�X
//Bullet�����p
//CombatUnit�����p
//Appearance�����p
//Hitbox�����p

struct EntityPointer {
	int Archetype;
	UINT16 Index;
};
typedef int EntityIdentifyN;