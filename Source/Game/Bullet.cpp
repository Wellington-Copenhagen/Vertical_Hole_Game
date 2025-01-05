#include "Bullet.h"
void Bullet::EveryTickAction() {
	Movement->Change(this);
	WhileMove->Change(this);
}
Damage Bullet::WhenHit() {
	HitAction->Change(this);
	return Damage::GetCorrectedDamage(&BaseDamage,&DamageMultiply);
}
void BulletArray::CreateBulletInstance(BulletGenerateCoef* coef, MotionInfo* initialMotion) {
	Instances[coef->PrototypeIndex].push_front(
		new Bullet(initialMotion, &Prototypes[coef->PrototypeIndex], pEnv, coef));
}
void BallisticDecVel::Change(Bullet* parent) {
	parent->AboutMotion.Velocity.r[3] = DirectX::XMVector4Transform(parent->AboutMotion.Velocity.r[3], M);
}
void BulletEmission::Change(Bullet* parent) {
	if (HowManyGenerate != 1) {
		for (int i = 0; i < HowManyGenerate; i++) {
			float heading = Width / -2.0 + Width * i / (HowManyGenerate - 1);
			parent->AboutMotion.WorldMatrix = MotionInfo::RotateOnly3x3(heading, parent->AboutMotion.WorldMatrix);
			pEnv.AddBullet(&EmitBulletData, &parent->AboutMotion);
		}
	}
	else {
		pEnv.AddBullet(&EmitBulletData, &parent->AboutMotion);
	}
}
void NonPenetrativeHit::Change(Bullet* parent){}
void BulletArray::ExecuteEveryTickAction() {
	int size = Instances.size();
	for (int i = 0; i < size; i++) {
		std::list<Bullet*>::iterator itr = Instances[i].begin();
		std::list<Bullet*>::iterator itrEnd = Instances[i].end();
		while (itr != itrEnd) {
			(*itr)->EveryTickAction();
			itr++;
		}
	}
}
inline void BulletArray::ResetBeforeMoveLimiting() {
	int size = Instances.size();
	for (int i = 0; i < size; i++) {
		std::list<Bullet*>::iterator itr = Instances[i].begin();
		std::list<Bullet*>::iterator itrEnd = Instances[i].end();
		while (itr != itrEnd) {
			(*itr)->ResetBeforeMoveLimiting();
			itr++;
		}
	}
}
inline void BulletArray::ApplyMovement() {
	int size = Instances.size();
	for (int i = 0; i < size; i++) {
		std::list<Bullet*>::iterator itr = Instances[i].begin();
		std::list<Bullet*>::iterator itrEnd = Instances[i].end();
		while (itr != itrEnd) {
			(*itr)->ApplyMovement();
			itr++;
		}
	}
}