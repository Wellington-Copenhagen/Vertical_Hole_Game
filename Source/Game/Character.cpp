#include "Character.h"
#include "framework.h"


AllCharacter::AllCharacter(std::string fileName) {

}
void AllCharacter::EveryTickProcess();
void AllCharacter::Add(int index);
void AllCharacter::Delete(int index);
void AllCharacter::Draw();

////////////////////////////////////////////////////////////////////////////////////////////////////////////


Character::Character(std::string textureName, int Size, float);
void Character::SetToBufferData();
void Character::CalcurateStatus();
void Character::ApplyWorldToHitBox();
void Character::AddElement();
void Character::SetDamage(int index, StatusEffect damage);

///////////////////////////////////////////////////////////////////////////////////////////////////////


EachCharacter::EachCharacter();
EachCharacter::~EachCharacter();
void EachCharacter::SetToBufferData();
void EachCharacter::CalcurateStatus();

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

StatusEffect::StatusEffect() {
	ZeroMemory(this, sizeof(StatusEffect));
}
void StatusEffect::ApplyDamage(StatusEffect damage) {
	DamageCut = damage.DamageCut;
	DamageUp = damage.DamageUp;
	MoveSpeedCoef = damage.MoveSpeedCoef;
	RegenerationStrength = damage.RegenerationStrength;
	RegenerationTime = damage.RegenerationTime;
	ShieldCount = damage.ShieldCount;
	PoisonStrength = damage.PoisonStrength;
	PoisonTime = damage.PoisonTime;
	if (damage.HPdamage > 0) {
		if (ShieldCount > 0) {
			ShieldCount--;
		}
		else {

		}
	}
}
void StatusEffect::EveryTickProcess();