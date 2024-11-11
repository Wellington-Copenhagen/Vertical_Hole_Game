#pragma once
#include "framework.h"
#include "Source/DirectX/DirectX.h"
#include "Source/DirectX/Texture.h"
#include "Source/DirectX/Buffers.h"
#include "Shape.h"
#include "Effect.h"
#define CountOfStatusEffect 9
struct StatusEffect {
	int HPdamage;//バフデバフではHPに対応
	int MaxHPdamage;
	int PoisonStrength;
	int PoisonTime;
	float DamageCut;
	int ShieldCount;
	int RegenerationStrength;
	int RegenerationTime;
	float DamageUp;
	float MoveSpeedCoef;
	StatusEffect();
	void ApplyDamage(StatusEffect damage);
	void EveryTickProcess();
};
//1人のキャラクター
class EachCharacter {
	friend Character;
private:
	Circle Pos;
	DirectX::XMVECTOR World;
	DirectX::XMVECTOR Verocity;
	float Attack1Range;
	float Attack2Range;
	float Attack1Damage;
	float Attack2Damage;
	StatusEffect BuffDebuff;
	std::list<std::list<RectEachEffect>::iterator> FixedEffects;
public:
	EachCharacter();
	~EachCharacter();
	void SetToBufferData();
	void CalcurateStatus();
	DirectX::XMMATRIX* GetWorld();
};
//1種類のキャラクター
class Character {
	friend AllCharacter;
private:
	Texture Texture;
	RectVertexType NormalUV;
	std::vector<EachCharacter> Characters;
	float StatusEffectSensitivity[CountOfStatusEffect];
	Attack::pAttack Attack1;
	Attack::pAttack Attack2;
	float MaxHP;
	float Attack1Range;
	float Attack2Range;
	float Attack1Damage;
	float Attack2Damage;
	int HPLevel;//Load
	int DamageLevel;//Load
	int AtackSpeedLevel;//Load
	float MoveSpeed;//Load
	float HitboxRadius;//Load
	DirectX::XMVECTOR RelHitboxPos;//Load
public:
	Character(std::string textureName,int Size,float );
	void SetToBufferData();
	void CalcurateStatus();
	void ApplyWorldToHitBox();
	void AddElement();
	void SetDamage(int index, StatusEffect damage);
};
class AllCharacter {
private:
	std::unordered_map<std::string, int> NameToIndex;
	std::vector<Character> Characters;
public:
	AllCharacter(std::string fileName);
	void EveryTickProcess();
	void Add(int index);
	void Delete(int index);
	void Draw();
};
class Attack {
public:
	typedef void (*pAttack)(EachCharacter&, int&, int&);
	static pAttack Attacks[];
	static void BowShot(EachCharacter&, int&, int&);
	static void Slash(EachCharacter&, int&, int&);
	static void Stab(EachCharacter&, int&, int&);
	static void Guard(EachCharacter&, int&, int&);
};