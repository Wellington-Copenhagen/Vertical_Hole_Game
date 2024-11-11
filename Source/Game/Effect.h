#pragma once
#include "Source/DirectX/DirectX.h"
#include "Source/DirectX/Texture.h"
#include "Source/DirectX/Buffers.h"
#include "Shape.h"
#include "Character.h"
#include "framework.h"
#define CountOfAction 4
//�U���A�L�����N�^�[�A�n��
//�U���A�L�����N�^�[�̐������w���I�u�W�F�N�g�@
//������(�I�u�W�F�N�g�A�������͐�΍��W�ɂȂ���)�@�`�揈��
//�����蔻��(�ǂ̃I�u�W�F�N�g�ɂȂ���̂�)�@�ڐG���肵�ăI�u�W�F�N�g�̊֐��|�C���^���Ă�
// �ڐG��������ւ̃C�e���[�^�[���������Ă�����Damage�\���̂�n��
//�����𕪗����ĊǗ�����
//list�ŊǗ����ăC�e���[�^�[���݂��Ɏ�������

struct EffectAddress {
	bool IsRect;
	int AllAddress;
	union itr {
		std::list<RectEachEffect>::iterator rect;
		std::list<StripEachEffect>::iterator rect;
	};
};

//1�̃G�t�F�N�g
class EachEffect {
	friend RectEffect;
	friend StripEffect;
protected:
	DirectX::XMVECTOR Verocity;
	DirectX::XMMATRIX World;
	bool Fixed;
	std::list<EachCharacter>::iterator FIxedTo;
	float Vars[4];
	StatusEffect Damage;
public:
	EachEffect();
	EachEffect(DirectX::XMMATRIX verocity,float vars[4],StatusEffect damage);
	DirectX::XMMATRIX* GetWorld();
	void SetVerocity(DirectX::XMMATRIX input);
};
struct RectEachEffectDesc {

};
class RectEachEffect : public EachEffect {
	friend RectEffect;
private:
	Circle HitBox;
public:
	RectEachEffect();
	RectEachEffect(DirectX::XMMATRIX verocity, float vars[4],float angle,DirectX::XMVECTOR pos,float radius,DirectX::XMVECTOR hitBoxRPos, StatusEffect damage);
};
class StripEachEffect : public EachEffect {
	friend StripEffect;
private:
	Strip VisibleHit;
public:
	StripEachEffect();
	StripEachEffect(DirectX::XMMATRIX verocity, float vars[4],DirectX::XMVECTOR* wpos, DirectX::XMFLOAT2* uvs,int stripLength, StatusEffect damage);
	StripVertexType* GetVDatas();
	int* GetVCount();
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//1�̎�ނ̑S�ẴG�t�F�N�g
class Effect {
	friend AllEffect;
protected:
	Texture Tex;//Load�Ŏw��
	bool DefaultFixed;//������false
public:
	Effect() {};
	AttackFuncAndArg WhenSpawn;
	AttackFuncAndArg WhenHitToCharacter;
	AttackFuncAndArg WhenHitToWall;
	AttackFuncAndArg EveryTick;
};
class RectEffect : public Effect {
	friend AllEffect;
private:
	RectVertexType VDataList[4];//Load
	float HitboxRadius;//Load
	DirectX::XMVECTOR RelHitboxPos;//Load
	std::list<RectEachEffect> Effects;
public:
	RectEffect() {};
	RectEffect(std::string texFileName,bool shrinkUVarea, float width, float height,float hitboxRadius,DirectX::XMVECTOR relHitboxPos);
	void Draw();
	void DeleteElement(std::list<RectEachEffect>::iterator itr);
	void ApplyWorldToHitBox();
};
class StripEffect : public Effect {
	friend AllEffect;
private:
	std::list<StripEachEffect> Effects;
public:
	StripEffect(std::string texFileName);
	void Draw();
	void DeleteElement(std::list<StripEachEffect>::iterator itr);
};
//////////////////////////////////////////////////////////////////////////////
class AllEffect {
private:
	std::unordered_map<std::string, int> NameToRectIndex;//�t�@�C������ǂݍ��ނƂ��ɖ��O���烊�X�g��̃C���f�b�N�X�ɕϊ����鎫�������
	std::unordered_map<std::string, int> NameToStripIndex;
	std::vector<RectEffect> RectEffects;
	std::vector<StripEffect> StripEffects;
public:
	AllEffect();
	void Load(std::string rectEffectFileName, std::string stripEffectFileName);
	//�G�t�F�N�g�̖��O,�e�N�X�`���̃A�h���X,WhenSpawn,WhenHitTocharacter,WhenHitToWall,EveryTick,width,height,�V�������N���邩,�����蔻�蔼�a,�����蔻��̑��Έʒu(x,y)
	void LoadRectEffect(std::string rectEffectFileName);
	//�G�t�F�N�g�̖��O,�e�N�X�`���̃A�h���X,WhenSpawn,WhenHitTocharacter,WhenHitToWall,EveryTick
	void LoadStripEffect(std::string stripEffectFileName);
	void EveryTickProcess();
	void AddRectElement(int index, DirectX::XMMATRIX verocity, float vars[4], float angle, DirectX::XMVECTOR pos, StatusEffect damage);
	void AddStripElement(int index, DirectX::XMMATRIX verocity, float vars[4], DirectX::XMVECTOR* wpos, DirectX::XMFLOAT2* uvs, int stripLength, StatusEffect damage);

	void Draw();
};
class Attack {
public:
	typedef bool (*pAttack)(EachEffect&, float, float);
	static std::unordered_map<std::string, pAttack> AttackDictionary;
	static bool Explode(EachEffect&, float, float);
	static bool Penetrate(EachEffect&, float, float);
	static bool Stop(EachEffect&, float, float);
};
struct AttackFuncAndArg {
	Attack::pAttack Func;
	float Arg1;
	float Arg2;
	bool Run(EachEffect& effect) {
		return Func(effect, Arg1, Arg2);
	}
};
std::unordered_map<std::string, Attack::pAttack> Attack::AttackDictionary = 
{
	{"Explode",Attack::Explode},
	{"Penetrate",Attack::Penetrate},
	{"Stop",Attack::Stop},
};
StripEffect* AllStripEffect;
RectEffect* AllRectEffect;
#define StripE AllStripEffect
#define RectE AllRectEffect