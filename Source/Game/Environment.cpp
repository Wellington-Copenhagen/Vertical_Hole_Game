#include "Environment.h"

void Environment::ExecuteEveryTickAction() {
	CombatUnits->ExecuteEveryTickAction();
	Effects->ExecuteEveryTickAction();
	Bullets->ExecuteEveryTickAction();

}
void Environment::CollidAndDamage() {
	{
		int CUsize = CombatUnits->Instances.size();
		for (int cu = 0; cu < CUsize; cu++) {
			auto itrCU = CombatUnits->Instances[cu].begin();
			auto endCU = CombatUnits->Instances[cu].end();
			while (itrCU != endCU) {
				int Bsize = Bullets->Instances.size();
				for (int b = 0; b < Bsize; b++) {
					auto itrB = Bullets->Instances[b].begin();
					auto endB = Bullets->Instances[b].end();
					while (itrB != endB) {
						if (Collision::IsColliding(&(*itrCU)->Hitbox, &(*itrB)->Hitbox, false)) {
							(*itrCU)->SetDamage((*itrB)->WhenHit());
						}
						itrB++;
					}
				}
				itrCU++;
			}
		}
	}
}
void Environment::MoveLimitAndMove() {
	Bullets->ResetBeforeMoveLimiting();
	Effects->ResetBeforeMoveLimiting();
	CombatUnits->ResetBeforeMoveLimiting();
	{
		int MovingSize = CombatUnits->Instances.size();
		for (int m = 0; m < MovingSize; m++) {
			auto itrMoving = CombatUnits->Instances[m].begin();
			auto endMoving = CombatUnits->Instances[m].end();
			while (itrMoving != endMoving) {
				int WallSize = Walls->Instances.size();
				for (int w = 0; w < WallSize; w++) {
					auto itrW = Walls->Instances[w].begin();
					auto endW = Walls->Instances[w].end();
					while (itrW != endW) {
						Collision::PenetDepth(&(*itrMoving)->Hitbox, &(*itrW)->Hitbox);
						itrW++;
					}
				}
				itrMoving++;
			}
		}
	}
	{
		int MovingSize = CombatUnits->Instances.size();
		for (int m = 0; m < MovingSize; m++) {
			auto itrMoving = CombatUnits->Instances[m].begin();
			auto endMoving = CombatUnits->Instances[m].end();
			while (itrMoving != endMoving) {
				int BlockingSize = CombatUnits->Instances.size();
				for (int b = 0; b < BlockingSize; b++) {
					auto itrBlocking = CombatUnits->Instances[b].begin();
					auto endBlocking = CombatUnits->Instances[b].end();
					while (itrBlocking != endBlocking) {
						if (m != b) {
							Collision::PenetDepth(&(*itrMoving)->Hitbox, &(*itrBlocking)->Hitbox);
						}
						itrBlocking++;
					}
				}
				itrMoving++;
			}
		}
	}
	Bullets->ApplyMovement();
	Effects->ApplyMovement();
	CombatUnits->ApplyMovement();
}