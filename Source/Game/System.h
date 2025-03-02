#pragma once
#include "../../Interfaces.h"
#include "Appearance.h"
#include "Entity.h"
#include "Input.h"
#include "HitBox.h"
//関連するシステムは同じものに収める方が良いと考えられる
class GameSystem;
class System {
public:
	Hurtboxes* pHurtboxes;
	Appearances<Interface::BlockDrawCallType, Interface::BlockInstanceType, 1, 256 * 256 * 2>* pBlockAppearance;
	//影、本体、模様
	Appearances<Interface::BallDrawCallType, Interface::BallInstanceType, 1, MaxBallCount>* pBallAppearance;
	Appearances<Interface::BulletDrawCallType, Interface::BulletInstanceType, 1, 256 * 256>* pBulletAppearance;
	AllEntities* pAllEntities;
	Interface::RawArchetype RequireComponents;
	std::vector<Interface::ArchetypeIndex> TargetArchetypes;
	System(GameSystem* pGameSystem);
	virtual void Update(SameArchetype* Components) = 0;
	void UpdateAll() {
		int archetypeCount = TargetArchetypes.size();
		for (int i = 0; i < archetypeCount; i++) {
			Update(&pAllEntities->EntityArraies[TargetArchetypes[i]]);
		}
	}
	void SetTargetList(AllEntities* pAllEntities) {
		int size = pAllEntities->Archetypes.size();
		for (int i = 0; i < size; i++) {
			if (RequireComponents.count() == (pAllEntities->Archetypes[i] & RequireComponents).count()) {
				TargetArchetypes.push_back(i);
			}
		}
	}
	void SetArchetype(CompNames compName) {
		RequireComponents.set(compName);
	}
};
namespace Systems {
	//追加削除
	class UpdateValidEntityCount : public System {
	public:
		UpdateValidEntityCount(GameSystem* pGameSystem) : System(pGameSystem) {
			SetTargetList(pAllEntities);
		}
		void Update(SameArchetype* input) override {
			int Isize = input->GenerateFlag.Components.size();
			for (int i = 0; i < Isize; i++) {
				if (input->GenerateFlag.Components[i].GeneratedOnThisTick) {
					input->RealEntityCount++;
				}
				if (input->KillFlag.Components[i].KillOnThisTick) {
					input->RealEntityCount--;
				}
				//この時点で追加関連の処理はすべて終わっている必要がある
			}
			input->ValidEntityCount = input->RealEntityCount;
		}
	};
	class KillGenerate : public System {
	public:
		KillGenerate(GameSystem* pGameSystem) : System(pGameSystem) {
			SetArchetype(CompNames::KillFlag);
			SetArchetype(CompNames::GenerateFlag);
			SetTargetList(pAllEntities);
		}
		void Update(SameArchetype* input) override {
			int Isize = input->ValidEntityCount;
			for (int i = 0; i < Isize; i++) {
				if (input->GenerateFlag.Components[i].GeneratedOnThisTick) {
					input->GenerateFlag.Components[i].GeneratedOnThisTick = false;
				}
				if (input->KillFlag.Components[i].KillOnThisTick) {
					input->Delete(pAllEntities, i);
					i--;
				}
				//この時点で追加関連の処理はすべて終わっている必要がある
			}
		}
	};
	class ControlUnitCount : public System {
	public:
		ControlUnitCount(GameSystem* pGameSystem) : System(pGameSystem) {
			SetArchetype(CompNames::UnitData);
			SetTargetList(pAllEntities);
		}
		void Update(SameArchetype* input) override {
			int Isize = input->ValidEntityCount;
			for (int i = 0; i < Isize; i++) {
				if (input->KillFlag.Components[i].KillOnThisTick) {
					pAllEntities->UnitCount[input->UnitData.Components[i].Team]--;
				}
				if (input->GenerateFlag.Components[i].GeneratedOnThisTick) {
					pAllEntities->UnitCount[input->UnitData.Components[i].Team]++;
				}
			}
			input->ValidEntityCount = input->IdArray.size();
		}
	};
	//意思決定
	class UnitAction : public System {
	public:
		UnitAction(GameSystem* pGameSystem) : System(pGameSystem) {
			SetArchetype(CompNames::Motion);
			SetArchetype(CompNames::WorldPosition);
			SetArchetype(CompNames::AI);
			SetArchetype(CompNames::UnitData);
			SetTargetList(pAllEntities);
		}
		void Update(SameArchetype* input) override {
			int Isize = input->ValidEntityCount;
			for (int i = 0; i < Isize; i++) {
				if (input->IdArray[i] == Interface::PlayingBall) {
					//プレイヤーの行動
					input->Motion.Components[i].WorldDelta.Parallel = { 0.0f, 0.0f, 0.0f ,0.0f };
					input->Motion.Components[i].WorldDelta.Ratio = 1;
					input->Motion.Components[i].WorldDelta.Rotate = 0;
					float moveTilePerTick = input->UnitData.Components[i].MoveTilePerTick * input->UnitData.Components[i].SpeedBuff;
					if (Input::KeyPushed.test(0x57)) {//w
						input->Motion.Components[i].WorldDelta.Parallel = { 0.0f, moveTilePerTick, 0.0f ,0.0f };
					}
					if (Input::KeyPushed.test(0x53)) {//s
						input->Motion.Components[i].WorldDelta.Parallel = { 0.0f, -1 * moveTilePerTick, 0.0f ,0.0f };
					}
					if (Input::KeyPushed.test(0x41)) {//a
						input->Motion.Components[i].WorldDelta.Parallel = { -1 * moveTilePerTick, 0.0f, 0.0f ,0.0f };
					}
					if (Input::KeyPushed.test(0x44)) {//d
						input->Motion.Components[i].WorldDelta.Parallel = { moveTilePerTick, 0.0f, 0.0f ,0.0f };
					}
					if (Input::KeyPushed.test(0x51)) {//q
						input->Motion.Components[i].WorldDelta.Rotate = 0.03f;
					}
					if (Input::KeyPushed.test(0x45)) {//e
						input->Motion.Components[i].WorldDelta.Rotate = -0.03f;
					}
				}
				else {
					//AIの行動
					bool ShouldChangeOrder = false;
					Interface::Order* pCurrentOrder = &input->AI.Components[i].Orders[0];
					if (pCurrentOrder->Doing == Interface::WhatDoing::Moving) {
						float length = DirectX::XMVector2Length(DirectX::XMVectorSubtract(
							input->WorldPosition.Components[i].WorldPos.Parallel,
							pCurrentOrder->MoveFor)).m128_f32[0];
						if (length < 0.1) {
							//移動終了
							ShouldChangeOrder = true;
						}
						else {

							if (length > 10) {
								//方向転換しての移動

							}
							else {
								//方向転換しないでの移動

							}
						}
					}
					else if (pCurrentOrder->Doing == Interface::WhatDoing::Chasing) {
						Interface::EntityPointer targetPointer = pAllEntities->INtoIndex[pCurrentOrder->Target];
						float length = DirectX::XMVector2Length(DirectX::XMVectorSubtract(
							input->WorldPosition.Components[i].WorldPos.Parallel,
							pAllEntities->EntityArraies[targetPointer.Archetype].WorldPosition.Components[targetPointer.Index].WorldPos.Parallel)).m128_f32[0];

					}
					if (ShouldChangeOrder) {

					}
				}
			}
		}
	};
	//運動に関する
	class UpdateWorld : public System {
	public:
		UpdateWorld(GameSystem* pGameSystem) : System(pGameSystem) {
			SetArchetype(CompNames::Motion);
			SetArchetype(CompNames::WorldPosition);
			SetArchetype(CompNames::MoveFlag);
			SetArchetype(CompNames::HitFlag);
			SetArchetype(CompNames::PositionReference);
			SetTargetList(pAllEntities);
		}
		void Update(SameArchetype* input) override {
			int Isize = input->ValidEntityCount;
			for (int i = 0; i < Isize; i++) {
				DirectX::XMVECTOR zeros{ 0.0f,0.0f,0.0f,0.0f };
				if (input->PositionReference.Components[i].IsCore) {
					if (input->WorldPosition.Components[i].NeedUpdate) {
						input->MoveFlag.Components[i].Moved = true;
						input->WorldPosition.Components[i].WorldPos = input->WorldPosition.Components[i].NextTickWorldPos;
						input->WorldPosition.Components[i].WorldMatrix = input->WorldPosition.Components[i].WorldPos.GetMatrix();
					}
					else {
						input->MoveFlag.Components[i].Moved = false;
						input->WorldPosition.Components[i].NextTickWorldPos = input->WorldPosition.Components[i].WorldPos;
					}
				}
			}
			for (int i = 0; i < Isize; i++) {
				if (!input->PositionReference.Components[i].IsCore) {
					Interface::EntityPointer corePointer = pAllEntities->INtoIndex[input->PositionReference.Components[i].ReferenceTo];
					if (pAllEntities->EntityArraies[corePointer.Archetype].MoveFlag.Components[corePointer.Index].Moved ||
						input->GenerateFlag.Components[i].GeneratedOnThisTick) {
						input->WorldPosition.Components[i].WorldPos = input->WorldPosition.Components[i].NextTickWorldPos;
						input->MoveFlag.Components[i].Moved = true;
						input->WorldPosition.Components[i].WorldMatrix = input->WorldPosition.Components[i].WorldPos.GetMatrix();
					}
					else {
						input->MoveFlag.Components[i].Moved = false;
						input->WorldPosition.Components[i].NextTickWorldPos = input->WorldPosition.Components[i].WorldPos;
					}
				}
			}
		}
	};
	class CalcurateGeneratedWorld : public System {
	public:
		CalcurateGeneratedWorld(GameSystem* pGameSystem) : System(pGameSystem) {
			SetArchetype(CompNames::WorldPosition);
			SetArchetype(CompNames::PositionReference);
			SetArchetype(CompNames::GenerateFlag);
			SetArchetype(CompNames::Motion);
			SetTargetList(pAllEntities);
		}
		void Update(SameArchetype* input) override {
			int Isize = input->ValidEntityCount;
			for (int i = 0; i < Isize; i++) {
				if (input->PositionReference.Components[i].IsCore &&
					input->GenerateFlag.Components[i].GeneratedOnThisTick) {
					input->WorldPosition.Components[i].WorldPos = input->Motion.Components[i].WorldDelta +
						input->WorldPosition.Components[i].WorldPos;
					input->WorldPosition.Components[i].NextTickWorldPos = input->WorldPosition.Components[i].WorldPos;
					input->WorldPosition.Components[i].WorldMatrix = input->WorldPosition.Components[i].WorldPos.GetMatrix();
				}
			}
			for (int i = 0; i < Isize; i++) {
				if (!input->PositionReference.Components[i].IsCore &&
					input->GenerateFlag.Components[i].GeneratedOnThisTick) {
					Interface::EntityPointer corePointer = pAllEntities->INtoIndex[input->PositionReference.Components[i].ReferenceTo];
					input->WorldPosition.Components[i].WorldPos = input->WorldPosition.Components[i].LocalReferenceToCore *
						pAllEntities->EntityArraies[corePointer.Archetype].WorldPosition.Components[corePointer.Index].WorldPos;
					input->WorldPosition.Components[i].NextTickWorldPos = input->WorldPosition.Components[i].WorldPos;
					input->WorldPosition.Components[i].WorldMatrix = input->WorldPosition.Components[i].WorldPos.GetMatrix();
				}
			}
		}
	};
	class CoreApplyLinearAcceralation : public System {
	public:
		CoreApplyLinearAcceralation(GameSystem* pGameSystem) : System(pGameSystem) {
			SetArchetype(CompNames::Motion);
			SetArchetype(CompNames::LinearAcceralation);
			SetArchetype(CompNames::PositionReference);
			SetTargetList(pAllEntities);
		}
		void Update(SameArchetype* input) override {
			int Isize = input->ValidEntityCount;
			for (int i = 0; i < Isize; i++) {
				if (input->PositionReference.Components[i].IsCore) {
					input->Motion.Components[i].WorldDelta = input->Motion.Components[i].WorldDelta +
						input->LinearAcceralation.Components[i].World;
				}
			}
		}
	};
	class CalculateNextTickWorld : public System {
	public:
		CalculateNextTickWorld(GameSystem* pGameSystem) : System(pGameSystem) {
			SetArchetype(CompNames::WorldPosition);
			SetArchetype(CompNames::PositionReference);
			SetArchetype(CompNames::Motion);
			SetTargetList(pAllEntities);
		}
		void Update(SameArchetype* input) override {
			int Isize = input->ValidEntityCount;
			for (int i = 0; i < Isize; i++) {
				DirectX::XMVECTOR zeros{ 0.0f,0.0f,0.0f,0.0f };
				if (input->PositionReference.Components[i].IsCore) {
					if (!DirectX::XMVector4Equal(zeros, input->Motion.Components[i].WorldDelta.Parallel) ||
						input->Motion.Components[i].WorldDelta.Ratio != 1 ||
						input->Motion.Components[i].WorldDelta.Rotate != 0) {
						input->WorldPosition.Components[i].NextTickWorldPos = input->Motion.Components[i].WorldDelta +
							input->WorldPosition.Components[i].WorldPos;
						input->WorldPosition.Components[i].NeedUpdate = true;// とりあえずtrueにしてあとでfalseにしていく
					}
					else {
						input->WorldPosition.Components[i].NeedUpdate = false;
					}
				}
			}
			for (int i = 0; i < Isize; i++) {
				if (!input->PositionReference.Components[i].IsCore) {
					Interface::EntityPointer corePointer = pAllEntities->INtoIndex[input->PositionReference.Components[i].ReferenceTo];
					if (pAllEntities->EntityArraies[corePointer.Archetype].WorldPosition.Components[corePointer.Index].NeedUpdate) {
						input->WorldPosition.Components[i].NextTickWorldPos = input->WorldPosition.Components[i].LocalReferenceToCore *
							pAllEntities->EntityArraies[corePointer.Archetype].WorldPosition.Components[corePointer.Index].NextTickWorldPos;
						input->WorldPosition.Components[i].NeedUpdate = true;
					}
					else {
						input->WorldPosition.Components[i].NeedUpdate = false;
					}
				}
			}
		}
	};
	//衝突に関する
	class WallHurtbox : public System {
	public:
		WallHurtbox(GameSystem* pGameSystem) : System(pGameSystem) {
			SetArchetype(CompNames::WallHurtbox);
			SetArchetype(CompNames::WorldPosition);
			SetArchetype(CompNames::GenerateFlag);
			SetArchetype(CompNames::KillFlag);
			SetTargetList(pAllEntities);
		}
		void Update(SameArchetype* input) override {
			int Isize = input->ValidEntityCount;
			for (int i = 0; i < Isize; i++) {
				if (input->GenerateFlag.Components[i].GeneratedOnThisTick) {
					pHurtboxes->SetWall((int)roundf(input->WorldPosition.Components[i].WorldPos.Parallel.m128_f32[0]),
						(int)roundf(input->WorldPosition.Components[i].WorldPos.Parallel.m128_f32[1]), input->IdArray[i]);
				}
				if (input->KillFlag.Components[i].KillOnThisTick) {
					pHurtboxes->DeleteWall((int)roundf(input->WorldPosition.Components[i].WorldPos.Parallel.m128_f32[0]),
						(int)roundf(input->WorldPosition.Components[i].WorldPos.Parallel.m128_f32[1]));
				}
			}
		}
	};
	class BallHurtbox : public System {
	public:
		BallHurtbox(GameSystem* pGameSystem) : System(pGameSystem) {
			SetArchetype(CompNames::AI);
			SetArchetype(CompNames::WorldPosition);
			SetArchetype(CompNames::UnitData);
			SetArchetype(CompNames::UnitOccupationbox);
			SetArchetype(CompNames::GenerateFlag);
			SetArchetype(CompNames::MoveFlag);
			SetArchetype(CompNames::KillFlag);
			SetTargetList(pAllEntities);
		}
		void Update(SameArchetype* input) override {
			int Isize = input->ValidEntityCount;
			for (int i = 0; i < Isize; i++) {
				input->UnitOccupationbox.Components[i].AlredayChecked = false;
				if (input->GenerateFlag.Components[i].GeneratedOnThisTick) {
					float radius = input->WorldPosition.Components[i].WorldPos.Ratio * 0.5;
					int bottom = (int)roundf(input->WorldPosition.Components[i].WorldPos.Parallel.m128_f32[1] - radius);
					int top = (int)roundf(input->WorldPosition.Components[i].WorldPos.Parallel.m128_f32[1] + radius);
					int left = (int)roundf(input->WorldPosition.Components[i].WorldPos.Parallel.m128_f32[0] - radius);
					int right = (int)roundf(input->WorldPosition.Components[i].WorldPos.Parallel.m128_f32[0] + radius);
					input->UnitOccupationbox.Components[i].OccupingRectBottom = bottom;
					input->UnitOccupationbox.Components[i].OccupingRectTop = top;
					input->UnitOccupationbox.Components[i].OccupingRectLeft = left;
					input->UnitOccupationbox.Components[i].OccupingRectRight = right;
					for (int x = left; x <= right; x++) {
						for (int y = bottom; y <= top; y++) {
							pHurtboxes->SetUnit(x, y, input->IdArray[i]);
						}
					}
				}
				if (input->WorldPosition.Components[i].NeedUpdate)
				{
					int OccupyingBottom = input->UnitOccupationbox.Components[i].OccupingRectBottom;
					int OccupyingTop = input->UnitOccupationbox.Components[i].OccupingRectTop;
					int OccupyingLeft = input->UnitOccupationbox.Components[i].OccupingRectLeft;
					int OccupyingRight = input->UnitOccupationbox.Components[i].OccupingRectRight;
					float radius = input->WorldPosition.Components[i].WorldPos.Ratio * 0.5;
					int CurrentBottom = (int)roundf(input->WorldPosition.Components[i].WorldPos.Parallel.m128_f32[1] - radius);
					int CurrentTop = (int)roundf(input->WorldPosition.Components[i].WorldPos.Parallel.m128_f32[1] + radius);
					int CurrentLeft = (int)roundf(input->WorldPosition.Components[i].WorldPos.Parallel.m128_f32[0] - radius);
					int CurrentRight = (int)roundf(input->WorldPosition.Components[i].WorldPos.Parallel.m128_f32[0] + radius);
					//占有エリアが変わったなら
					if (OccupyingBottom != CurrentBottom ||
						OccupyingTop != CurrentTop ||
						OccupyingLeft != CurrentLeft ||
						OccupyingRight != CurrentRight) {
						for (int x = OccupyingLeft; x <= OccupyingRight; x++) {
							for (int y = OccupyingBottom; y <= OccupyingTop; y++) {
								pHurtboxes->DeleteUnit(x, y, input->IdArray[i]);
							}
						}
						for (int x = CurrentLeft; x <= CurrentRight; x++) {
							for (int y = CurrentBottom; y <= CurrentTop; y++) {
								pHurtboxes->SetUnit(x, y, input->IdArray[i]);
							}
						}
					}
					input->UnitOccupationbox.Components[i].OccupingRectBottom = CurrentBottom;
					input->UnitOccupationbox.Components[i].OccupingRectTop = CurrentTop;
					input->UnitOccupationbox.Components[i].OccupingRectLeft = CurrentLeft;
					input->UnitOccupationbox.Components[i].OccupingRectRight = CurrentRight;
				}
				if (input->KillFlag.Components[i].KillOnThisTick) {
					int bottom = input->UnitOccupationbox.Components[i].OccupingRectBottom;
					int top = input->UnitOccupationbox.Components[i].OccupingRectBottom;
					int left = input->UnitOccupationbox.Components[i].OccupingRectBottom;
					int right = input->UnitOccupationbox.Components[i].OccupingRectBottom;
					for (int x = left; x < right; x++) {
						for (int y = bottom; y < top; y++) {
							pHurtboxes->DeleteUnit(x, y, input->IdArray[i]);
						}
					}
				}
			}
		}
	};
	//次ティックでの衝突を衝突とみなし、移動の阻害と衝突の判定を同時に行う。
	//GiveDamageコンポーネントの存在がダメージ付与することを、
	//貫通するのか、移動を阻害されるのかのフラグによって衝突後にどういった運動につながるかを決める。
	class CheckBulletCollision : public System {
	public:
		CheckBulletCollision(GameSystem* pGameSystem) : System(pGameSystem) {
			SetArchetype(CompNames::Motion);
			SetArchetype(CompNames::WorldPosition);
			SetArchetype(CompNames::CircleHitbox);
			SetArchetype(CompNames::GiveDamage);
			SetArchetype(CompNames::BulletData);
			SetArchetype(CompNames::HitFlag);
			SetTargetList(pAllEntities);
		}
		void Update(SameArchetype* input) override {
			int Isize = input->ValidEntityCount;
			for (int i = 0; i < Isize; i++) {
				input->HitFlag.Components[i].IsHit = false;
				input->HitFlag.Components[i].IsHit = pHurtboxes->CheckCircleCollid(
					input->WorldPosition.Components[i].NextTickWorldPos.Parallel,
					input->CircleHitbox.Components[i].Radius,
					//判定は1回、壁に衝突するかはballData参照、キャラクターとの衝突もballData参照、味方とは衝突しない
					&input->GiveDamage.Components[i].Damage, true, input->BulletData.Components[i].Team);
				if (input->HitFlag.Components[i].IsHit && input->BulletData.Components[i].InterfareToWall) {
					
				}
			}
		}
	};
	class CheckUnitCollision : public System {
	public:
		CheckUnitCollision(GameSystem* pGameSystem) : System(pGameSystem) {
			SetArchetype(CompNames::Motion);
			SetArchetype(CompNames::WorldPosition);
			SetArchetype(CompNames::UnitOccupationbox);
			SetArchetype(CompNames::UnitData);
			SetArchetype(CompNames::HitFlag);
			SetTargetList(pAllEntities);
		}
		void Update(SameArchetype* input) override {
			int Isize = input->ValidEntityCount;
			for (int i = 0; i < Isize; i++) {
				input->UnitOccupationbox.Components[i].AlredayChecked = true;
				input->HitFlag.Components[i].IsHit = false;
				//そもそも動こうとしている場合
				if (input->WorldPosition.Components[i].NeedUpdate) {
					//衝突している場合
					if (pHurtboxes->CheckUnitCollid(input->IdArray[i])) {
						if (i == 0) {
							OutputDebugStringA("Collided\n");
						}
						input->WorldPosition.Components[i].NeedUpdate = false;
						input->HitFlag.Components[i].IsHit = true;
					}
					if (i == 0)OutputDebugStringA("Collision Check End\n");
				}
			}
		}
	};
	//見た目に関する
	class BlockAppearance : public System {
	public:
		BlockAppearance(GameSystem* pGameSystem) : System(pGameSystem) {
			SetArchetype(CompNames::WorldPosition);
			SetArchetype(CompNames::BlockAppearance);
			SetArchetype(CompNames::GenerateFlag);
			SetArchetype(CompNames::MoveFlag);
			SetArchetype(CompNames::AppearanceChanged);
			SetArchetype(CompNames::KillFlag);
			SetTargetList(pAllEntities);
		}
		void Update(SameArchetype* input) override {
			int Isize = input->ValidEntityCount;
			for (int i = 0; i < Isize; i++) {
				//見た目の登録
				if (input->GenerateFlag.Components[i].GeneratedOnThisTick) {
					Interface::BlockInstanceType* pInstance = nullptr;
					input->BlockAppearance.Components[i].BufferDataIndex =
						pBlockAppearance->Add(input->IdArray[i], &pInstance);
					pInstance->Set
					(&input->WorldPosition.Components[i].WorldMatrix,
						&input->BlockAppearance.Components[i].TexCoord12, &input->BlockAppearance.Components[i].TexCoord3M);
				}
				//見た目の更新
				if (input->MoveFlag.Components[i].Moved || input->AppearanceChanged.Components[i].Changed) {
					input->AppearanceChanged.Components[i].Changed = false;
					Interface::BlockInstanceType* pInstance = nullptr;
					pBlockAppearance->Update(input->BlockAppearance.Components[i].BufferDataIndex, &pInstance);
					pInstance->Set(&input->WorldPosition.Components[i].WorldMatrix,
						&input->BlockAppearance.Components[i].TexCoord12, &input->BlockAppearance.Components[i].TexCoord3M);
				}
				//見た目の削除
				if (input->KillFlag.Components[i].KillOnThisTick) {
					//移動させたSameArchIndexを受け取る
					Interface::SameArchIndex replaced = pBlockAppearance->Delete(
						input->BlockAppearance.Components[i].BufferDataIndex);
					//移動させたRectAppIdを変更する
					if (replaced != -1) {
						input->BlockAppearance.Components[replaced].BufferDataIndex =
							input->BlockAppearance.Components[i].BufferDataIndex;
					}
				}
			}
		}
	};
	//見た目に関する
	class BallAppearance : public System {
	public:
		BallAppearance(GameSystem* pGameSystem) : System(pGameSystem) {
			SetArchetype(CompNames::WorldPosition);
			SetArchetype(CompNames::BallAppearance);
			SetArchetype(CompNames::GenerateFlag);
			SetArchetype(CompNames::MoveFlag);
			SetArchetype(CompNames::AppearanceChanged);
			SetArchetype(CompNames::KillFlag);
			SetTargetList(pAllEntities);
		}
		void Update(SameArchetype* input) override {
			int Isize = input->ValidEntityCount;
			for (int i = 0; i < Isize; i++) {
				//見た目の登録
				if (input->GenerateFlag.Components[i].GeneratedOnThisTick) {
					Interface::BallInstanceType* pInstance = nullptr;

					//影
					input->BallAppearance.Components[i].BufferDataIndex =
						pBallAppearance[0].Add(input->IdArray[i], &pInstance);
					DirectX::XMMATRIX shadowWorld{
						{input->WorldPosition.Components[i].WorldPos.Ratio, 0.0f, 0.0f, 0.0f},
						{0.0f, input->WorldPosition.Components[i].WorldPos.Ratio, 0.0f, 0.0f},
						{0.0f, 0.0f, 0.0f, 0.0f},
						DirectX::XMVectorAdd({0.0f, input->WorldPosition.Components[i].WorldPos.Ratio * -0.1f, 0.0f, 0.0f},input->WorldPosition.Components[i].WorldPos.Parallel)
					};
					pInstance->Set(&shadowWorld,
						&input->BallAppearance.Components[i].TexCoord[0],
						&input->BallAppearance.Components[i].Color0[0],
						&input->BallAppearance.Components[i].Color1[0],
						&input->BallAppearance.Components[i].Color2[0]);


					//本体
					pBallAppearance[1].Add(input->IdArray[i], &pInstance);
					DirectX::XMMATRIX baseWorld{
						{input->WorldPosition.Components[i].WorldPos.Ratio, 0.0f, 0.0f, 0.0f },
						{0.0f, input->WorldPosition.Components[i].WorldPos.Ratio, 0.0f, 0.0f},
						{0.0f, 0.0f, 0.0f, 0.0f},
						input->WorldPosition.Components[i].WorldPos.Parallel
					};
					pInstance->Set(&baseWorld,
						&input->BallAppearance.Components[i].TexCoord[1],
						&input->BallAppearance.Components[i].Color0[1],
						&input->BallAppearance.Components[i].Color1[1],
						&input->BallAppearance.Components[i].Color2[1]);

					//模様
					//上にあるものが奥に見えるのの再現
					pBallAppearance[2].Add(input->IdArray[i], &pInstance);
					DirectX::XMMATRIX patternWorld{
						0.0f, 0.0f, 0.0f, 0.0f,
							0.0f, 0.0f, 0.0f, 0.0f,
							0.0f, 0.0f, 0.0f, 0.0f,
							0.0f, input->WorldPosition.Components[i].WorldPos.Ratio*0.2f, 0.0f, 0.0f
					};
					patternWorld = patternWorld + input->WorldPosition.Components[i].WorldMatrix;
					pInstance->Set(&patternWorld,
						&input->BallAppearance.Components[i].TexCoord[2],
						&input->BallAppearance.Components[i].Color0[2],
						&input->BallAppearance.Components[i].Color1[2],
						&input->BallAppearance.Components[i].Color2[2]);


				}
				//見た目の更新
				if (input->MoveFlag.Components[i].Moved || input->AppearanceChanged.Components[i].Changed) {
					input->AppearanceChanged.Components[i].Changed = false;
					Interface::BallInstanceType* pInstance = nullptr;


					//影
					pBallAppearance[0].Update(input->BallAppearance.Components[i].BufferDataIndex, &pInstance);
					DirectX::XMMATRIX shadowWorld{
						{input->WorldPosition.Components[i].WorldPos.Ratio, 0.0f, 0.0f, 0.0f},
						{0.0f, input->WorldPosition.Components[i].WorldPos.Ratio, 0.0f, 0.0f},
						{0.0f, 0.0f, 0.0f, 0.0f},
						DirectX::XMVectorAdd({0.0f, input->WorldPosition.Components[i].WorldPos.Ratio * -0.1f, 0.0f, 0.0f},input->WorldPosition.Components[i].WorldPos.Parallel)
					};
					pInstance->Set(&shadowWorld,
						&input->BallAppearance.Components[i].TexCoord[0],
						&input->BallAppearance.Components[i].Color0[0],
						&input->BallAppearance.Components[i].Color1[0],
						&input->BallAppearance.Components[i].Color2[0]);


					pBallAppearance[1].Update(input->BallAppearance.Components[i].BufferDataIndex, &pInstance);
					DirectX::XMMATRIX baseWorld{
						{input->WorldPosition.Components[i].WorldPos.Ratio, 0.0f, 0.0f, 0.0f },
						{0.0f, input->WorldPosition.Components[i].WorldPos.Ratio, 0.0f, 0.0f},
						{0.0f, 0.0f, 0.0f, 0.0f},
						input->WorldPosition.Components[i].WorldPos.Parallel
					};
					pInstance->Set(&baseWorld,
						&input->BallAppearance.Components[i].TexCoord[1],
						&input->BallAppearance.Components[i].Color0[1],
						&input->BallAppearance.Components[i].Color1[1],
						&input->BallAppearance.Components[i].Color2[1]);

					pBallAppearance[2].Update(input->BallAppearance.Components[i].BufferDataIndex, &pInstance);
					DirectX::XMMATRIX patternWorld{
						0.0f, 0.0f, 0.0f, 0.0f,
						0.0f, 0.0f, 0.0f, 0.0f,
						0.0f, 0.0f, 0.0f, 0.0f,0.0f, 
						input->WorldPosition.Components[i].WorldPos.Ratio * 0.2f, 0.0f, 0.0f
					};
					patternWorld = patternWorld + input->WorldPosition.Components[i].WorldMatrix;
					pInstance->Set(&patternWorld,
						&input->BallAppearance.Components[i].TexCoord[2],
						&input->BallAppearance.Components[i].Color0[2],
						&input->BallAppearance.Components[i].Color1[2],
						&input->BallAppearance.Components[i].Color2[2]);
				}
				//見た目の削除
				if (input->KillFlag.Components[i].KillOnThisTick) {
					//移動させたSameArchIndexを受け取る
					Interface::SameArchIndex replaced = pBallAppearance->Delete(
						input->BallAppearance.Components[i].BufferDataIndex);
					//移動させたRectAppIdを変更する
					if (replaced != -1) {
						input->BallAppearance.Components[replaced].BufferDataIndex =
							input->BallAppearance.Components[i].BufferDataIndex;
					}
				}
			}
		}
	};
	//見た目に関する
	class BulletAppearance : public System {
	public:
		BulletAppearance(GameSystem* pGameSystem) : System(pGameSystem) {
			SetArchetype(CompNames::WorldPosition);
			SetArchetype(CompNames::BulletAppearance);
			SetArchetype(CompNames::GenerateFlag);
			SetArchetype(CompNames::MoveFlag);
			SetArchetype(CompNames::KillFlag);
			SetTargetList(pAllEntities);
		}
		void Update(SameArchetype* input) override {
			int Isize = input->ValidEntityCount;
			for (int i = 0; i < Isize; i++) {
				//見た目の登録
				if (input->GenerateFlag.Components[i].GeneratedOnThisTick) {
					Interface::BulletInstanceType* pInstance = nullptr;
					input->BulletAppearance.Components[i].BufferDataIndex =
						pBulletAppearance->Add(input->IdArray[i], &pInstance);
					pInstance->Set(&input->WorldPosition.Components[i].WorldMatrix,
						&input->BulletAppearance.Components[i].TexCoord,
						&input->BulletAppearance.Components[i].Color0,
						&input->BulletAppearance.Components[i].Color0);
				}
				//見た目の更新
				if (input->MoveFlag.Components[i].Moved || input->AppearanceChanged.Components[i].Changed) {
					input->AppearanceChanged.Components[i].Changed = false;
					Interface::BulletInstanceType* pInstance = nullptr;
					pBulletAppearance->Update(input->BallAppearance.Components[i].BufferDataIndex, &pInstance);
					pInstance->Set(&input->WorldPosition.Components[i].WorldMatrix,
						&input->BulletAppearance.Components[i].TexCoord,
						&input->BulletAppearance.Components[i].Color0,
						&input->BulletAppearance.Components[i].Color0);
				}
				//見た目の削除
				if (input->KillFlag.Components[i].KillOnThisTick) {
					//移動させたSameArchIndexを受け取る
					Interface::SameArchIndex replaced = pBulletAppearance->Delete(
						input->BulletAppearance.Components[i].BufferDataIndex);
					//移動させたRectAppIdを変更する
					if (replaced != -1) {
						input->BulletAppearance.Components[replaced].BufferDataIndex =
							input->BulletAppearance.Components[i].BufferDataIndex;
					}
				}
			}
		}
	};
	//オブジェクトの生成に関する
	class GenerateHitEffect : public System {
	public:
		GenerateHitEffect(GameSystem* pGameSystem) : System(pGameSystem) {
			SetArchetype(CompNames::WorldPosition);
			SetArchetype(CompNames::HitEffect);
			SetArchetype(CompNames::HitFlag);
			SetTargetList(pAllEntities);
		}
		void Update(SameArchetype* input) override {
			int Isize = input->ValidEntityCount;
			for (int i = 0; i < Isize; i++) {
			}
		}
	};
	class GenerateTrajectory : public System {
	public:
		GenerateTrajectory(GameSystem* pGameSystem) : System(pGameSystem) {
			SetArchetype(CompNames::WorldPosition);
			SetArchetype(CompNames::Trajectory);
			SetTargetList(pAllEntities);
		}
		void Update(SameArchetype* input) override {

		}
	};
	// ミッション進行に関する
	class InvationObserve : public System {
	public:
		InvationObserve(GameSystem* pGameSystem) : System(pGameSystem) {
			SetArchetype(CompNames::Spawn);
			SetArchetype(CompNames::InvationObservance);
			SetTargetList(pAllEntities);
		}
		void Update(SameArchetype* input) override {
		}
	};
	class UnitCountObserve : public System {
	public:

		UnitCountObserve(GameSystem* pGameSystem) : System(pGameSystem) {
			SetArchetype(CompNames::Spawn);
			SetArchetype(CompNames::UnitCountObservance);
			SetTargetList(pAllEntities);
		}
		void Update(SameArchetype* input) override {
			int Isize = input->ValidEntityCount;
			for (int i = 0; i < Isize; i++) {
				// まだフラグが立ってない(まだスポーンが行われてない)かつボーダーを超えた場合
				if (!input->Spawn.Components[i].SpawnFlag && 
					pAllEntities->UnitCount[input->UnitCountObservance.Components[i].Team] <= input->UnitCountObservance.Components[i].Border) {
					input->Spawn.Components[i].SpawnFlag = true;
				}
			}
		}
	};
	class Spawn : public System {
	public:
		Spawn(GameSystem* pGameSystem) : System(pGameSystem) {
			SetArchetype(CompNames::Spawn);
			SetTargetList(pAllEntities);
		}
		void Update(SameArchetype* input) override {
			int Isize = input->ValidEntityCount;
			for (int i = 0; i < Isize; i++) {
				if (input->Spawn.Components[i].SpawnFlag) {
					input->Spawn.Components[i].TimeGapSince--;
					Interface::UnitIndex toSpawn = input->Spawn.Components[i].ToSpawn;
					auto Xdist = std::uniform_real_distribution((float)input->Spawn.Components[i].SpawnAreaLeft,
						(float)input->Spawn.Components[i].SpawnAreaRight);
					auto Ydist = std::uniform_real_distribution((float)input->Spawn.Components[i].SpawnAreaBottom,
						(float)input->Spawn.Components[i].SpawnAreaTop);
					//占有エリアのサイズ
					//半径1.5の円となるので1.5倍
					float radius = pAllEntities->UnitInfos[toSpawn].Ratio * 0.5;
					//遅れ分を消化済み
					if (input->Spawn.Components[i].TimeGapSince <= 0) {
						for (int j = 0; j < input->Spawn.Components[i].HowManyInEachTick; j++) {
							float Xpos = Xdist(Interface::RandEngine);
							float Ypos = Ydist(Interface::RandEngine);
							if (pHurtboxes->IsAbleToSpawn(radius,radius,radius,radius, Xpos, Ypos) &&
								input->Spawn.Components[i].CountLeft >= 1) {
								pHurtboxes->SetOccupation(radius,radius,radius,radius, Xpos, Ypos);
								Interface::RelationOfCoord pos = Interface::RelationOfCoord();
								pos.Parallel = {
									(float)Xpos,(float)Ypos,0,1
								};
								pAllEntities->AddUnitWithMagnification(toSpawn,
									&input->Spawn.Components[i].CoreData, &pos);
								input->Spawn.Components[i].CountLeft--;
							}
						}
					}
					auto probDist = std::uniform_real_distribution(0.0f, 1.0f);
					if (probDist(Interface::RandEngine) <
						input->Spawn.Components[i].HowManyInEachTick -
						floor(input->Spawn.Components[i].HowManyInEachTick)) {
						int Xpos = Xdist(Interface::RandEngine);
						int Ypos = Ydist(Interface::RandEngine);

						if (pHurtboxes->IsAbleToSpawn(radius,radius,radius,radius, Xpos, Ypos) &&
							input->Spawn.Components[i].CountLeft >= 1) {
							pHurtboxes->SetOccupation(radius, radius, radius, radius, Xpos, Ypos);
							Interface::RelationOfCoord pos = Interface::RelationOfCoord();
							pos.Parallel = {
								(float)Xpos,(float)Ypos,0,1
							};
							pAllEntities->AddUnitWithMagnification(toSpawn,
								&input->Spawn.Components[i].CoreData, &pos);
							input->Spawn.Components[i].CountLeft--;
						}
					}
				}
			}
		}
	};
	class TestA : public System {
	public:
		TestA(GameSystem* pGameSystem) : System(pGameSystem) {
			SetArchetype(CompNames::Test1);
			SetArchetype(CompNames::TestResult);
			SetTargetList(pAllEntities);
		}
		void Update(SameArchetype* input) override {
			int Isize = input->ValidEntityCount;
			for (int i = 0; i < Isize; i++) {
				input->TestResult.Components[i].UpdatedByTestA = true;
			}
		}
	};
	class TestB : public System {
	public:
		TestB(GameSystem* pGameSystem) : System(pGameSystem) {
			SetArchetype(CompNames::Test2);
			SetArchetype(CompNames::TestResult);
			SetTargetList(pAllEntities);
		}
		void Update(SameArchetype* input) override {
			int Isize = input->ValidEntityCount;
			for (int i = 0; i < Isize; i++) {
				input->TestResult.Components[i].UpdatedByTestB = true;
			}
		}
	};
	class TestC : public System {
	public:
		TestC(GameSystem* pGameSystem) : System(pGameSystem) {
			SetArchetype(CompNames::Test1);
			SetArchetype(CompNames::Test2);
			SetArchetype(CompNames::TestResult);
			SetTargetList(pAllEntities);
		}
		void Update(SameArchetype* input) override {
			int Isize = input->ValidEntityCount;
			for (int i = 0; i < Isize; i++) {
				input->TestResult.Components[i].UpdatedByTestC = true;
			}
		}
	};
	class TestD : public System {
	public:
		TestD(GameSystem* pGameSystem) : System(pGameSystem) {
			SetArchetype(CompNames::TestResult);
			SetTargetList(pAllEntities);
		}
		void Update(SameArchetype* input) override {
			int Isize = input->ValidEntityCount;
			for (int i = 0; i < Isize; i++) {
				input->TestResult.Components[i].UpdatedByTestD = true;
			}
		}
	};
}
// 現ティックの状態を描画した後、次ティックの位置を予想して衝突にかかわる情報の受け渡しと移動だけを行う
// データの処理は次ティック
// 処理→当たり判定や見た目の繁栄→削除追加→衝突・移動→描画
class AllSystem {
public:
	std::vector<System*> SystemArray;
	AllSystem() {
		SystemArray = std::vector<System*>();
	}
	AllSystem(GameSystem* pGameSystem) {
		SystemArray = std::vector<System*>();
		//テスト
		SystemArray.push_back(new Systems::TestA(pGameSystem));
		SystemArray.push_back(new Systems::TestB(pGameSystem));
		SystemArray.push_back(new Systems::TestC(pGameSystem));
		SystemArray.push_back(new Systems::TestD(pGameSystem));
		//位置の更新

		SystemArray.push_back(new Systems::UpdateWorld(pGameSystem));

		//ミッション進行
		SystemArray.push_back(new Systems::InvationObserve(pGameSystem));
		SystemArray.push_back(new Systems::UnitCountObserve(pGameSystem));
		SystemArray.push_back(new Systems::Spawn(pGameSystem));
		//運動、意思決定
		SystemArray.push_back(new Systems::UnitAction(pGameSystem));
		SystemArray.push_back(new Systems::CoreApplyLinearAcceralation(pGameSystem));

		//エフェクトの生成
		SystemArray.push_back(new Systems::GenerateHitEffect(pGameSystem));
		SystemArray.push_back(new Systems::GenerateTrajectory(pGameSystem));

		// これ以前の処理でこのティックの削除と追加の決定はすべて行われる必要がある

		SystemArray.push_back(new Systems::UpdateValidEntityCount(pGameSystem));

		// ここより下に削除と追加にかかわる処理はする必要がある

		SystemArray.push_back(new Systems::ControlUnitCount(pGameSystem));

		SystemArray.push_back(new Systems::CalcurateGeneratedWorld(pGameSystem));
		//描画
		SystemArray.push_back(new Systems::BlockAppearance(pGameSystem));
		SystemArray.push_back(new Systems::BallAppearance(pGameSystem));
		SystemArray.push_back(new Systems::BulletAppearance(pGameSystem));

		//衝突マップに設定する
		SystemArray.push_back(new Systems::WallHurtbox(pGameSystem));
		SystemArray.push_back(new Systems::BallHurtbox(pGameSystem));

		// ここまでに削除と追加にかかわる処理は終わっている必要がある

		//削除追加
		SystemArray.push_back(new Systems::KillGenerate(pGameSystem));

		SystemArray.push_back(new Systems::CalculateNextTickWorld(pGameSystem));

		//衝突判定
		SystemArray.push_back(new Systems::CheckBulletCollision(pGameSystem));
		SystemArray.push_back(new Systems::CheckUnitCollision(pGameSystem));
	}
	void Update() {
		int size = SystemArray.size();
		OutputDebugStringA("Tick Start\n");
		for (int i = 0; i < size; i++) {
			SystemArray[i]->UpdateAll();
		}
		OutputDebugStringA("Tick End\n");
	}
};