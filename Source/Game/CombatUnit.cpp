#include "CombatUnit.h"
void CombatUnit::EveryTickAction() {

}
Damage CombatUnit::WhenHit(){}
void CombatUnit::SetDamage(Damage damage) {

}
void CombatUnitArray::CreateCombatUnitInstance(CombatUnitGenerateCoef* coef, MotionInfo* initialMotion) {
	Instances[coef->PrototypeIndex].push_front(
		new CombatUnit(initialMotion,&Prototypes[coef->PrototypeIndex],pEnv,coef));
}
void CombatUnitArray::ExecuteEveryTickAction() {
	int size = Instances.size();
	for (int i = 0; i < size; i++) {
		std::list<CombatUnit*>::iterator itr = Instances[i].begin();
		std::list<CombatUnit*>::iterator itrEnd = Instances[i].end();
		while (itr != itrEnd) {
			(*itr)->EveryTickAction();
			itr++;
		}
	}
}
inline void CombatUnitArray::ResetBeforeMoveLimiting() {
	int size = Instances.size();
	for (int i = 0; i < size; i++) {
		std::list<CombatUnit*>::iterator itr = Instances[i].begin();
		std::list<CombatUnit*>::iterator itrEnd = Instances[i].end();
		while (itr != itrEnd) {
			(*itr)->ResetBeforeMoveLimiting();
			itr++;
		}
	}
}
inline void CombatUnitArray::ApplyMovement() {
	int size = Instances.size();
	for (int i = 0; i < size; i++) {
		std::list<CombatUnit*>::iterator itr = Instances[i].begin();
		std::list<CombatUnit*>::iterator itrEnd = Instances[i].end();
		while (itr != itrEnd) {
			(*itr)->ApplyMovement();
			itr++;
		}
	}
}