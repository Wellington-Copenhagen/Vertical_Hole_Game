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
	Appearances<Interface::CharacterDrawCallType, Interface::CharacterInstanceType, 1, 1024>* pCharacterAppearance;
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
				input->GenerateFlag.Components[i].GeneratedOnThisTick = false;
				if (input->KillFlag.Components[i].KillOnThisTick) {
					input->Delete(pAllEntities, i);
					i--;
				}
				//この時点で追加関連の処理はすべて終わっている必要がある
			}
			input->ValidEntityCount = input->IdArray.size();
		}
	};
	//意思決定
	class CharacterAction : public System {
	public:
		CharacterAction(GameSystem* pGameSystem) : System(pGameSystem) {
			SetArchetype(CompNames::Motion);
			SetArchetype(CompNames::WorldPosition);
			SetArchetype(CompNames::AI);
			SetArchetype(CompNames::CharacterData);
			SetTargetList(pAllEntities);
		}
		void Update(SameArchetype* input) override {
			int Isize = input->ValidEntityCount;
			for (int i = 0; i < Isize; i++) {
				if (input->IdArray[i] == Interface::PlayingCharacter) {
					input->Motion.Components[i].WorldDelta.Parallel = { 0.0f, 0.0f, 0.0f ,0.0f };
					input->Motion.Components[i].WorldDelta.Ratio = 1;
					input->Motion.Components[i].WorldDelta.Rotate = 0;
					//プレイヤーの行動
					if (Input::KeyPushed.test(0x57)) {//w
						input->Motion.Components[i].WorldDelta.Parallel = { 0.0f, input->CharacterData.Components[i].TilePerTick, 0.0f ,0.0f };
					}
					if (Input::KeyPushed.test(0x53)) {//s
						input->Motion.Components[i].WorldDelta.Parallel = { 0.0f, -1 * input->CharacterData.Components[i].TilePerTick, 0.0f ,0.0f };
					}
					if (Input::KeyPushed.test(0x41)) {//a
						input->Motion.Components[i].WorldDelta.Parallel = { -1 * input->CharacterData.Components[i].TilePerTick, 0.0f, 0.0f ,0.0f };
					}
					if (Input::KeyPushed.test(0x44)) {//d
						input->Motion.Components[i].WorldDelta.Parallel = { input->CharacterData.Components[i].TilePerTick, 0.0f, 0.0f ,0.0f };
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
				}
			}
		}
	};
	//運動に関する
	class CoreUpdateWorld : public System {
	public:
		CoreUpdateWorld(GameSystem* pGameSystem) : System(pGameSystem) {
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
		}
	};
	class NotCoreUpdateWorld : public System {
	public:
		NotCoreUpdateWorld(GameSystem* pGameSystem) : System(pGameSystem) {
			SetArchetype(CompNames::WorldPosition);
			SetArchetype(CompNames::PositionReference);
			SetTargetList(pAllEntities);
		}
		void Update(SameArchetype* input) override {
			int Isize = input->ValidEntityCount;
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
	class CoreNextTickWorld : public System {
	public:
		CoreNextTickWorld(GameSystem* pGameSystem) : System(pGameSystem) {
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
						input->Motion.Components[i].WorldDelta.Rotate != 0 ||
						input->GenerateFlag.Components[i].GeneratedOnThisTick) {
						input->WorldPosition.Components[i].NextTickWorldPos = input->Motion.Components[i].WorldDelta +
							input->WorldPosition.Components[i].WorldPos;
						input->WorldPosition.Components[i].NeedUpdate = true;// とりあえずtrueにしてあとでfalseにしていく
					}
					else {
						input->WorldPosition.Components[i].NeedUpdate = false;
					}
				}
			}
		}
	};
	class NotCoreNextTickWorld : public System {
	public:
		NotCoreNextTickWorld(GameSystem* pGameSystem) : System(pGameSystem) {
			SetArchetype(CompNames::WorldPosition);
			SetArchetype(CompNames::PositionReference);
			SetArchetype(CompNames::Motion);
			SetTargetList(pAllEntities);
		}
		void Update(SameArchetype* input) override {
			int Isize = input->ValidEntityCount;
			for (int i = 0; i < Isize; i++) {
				if (!input->PositionReference.Components[i].IsCore) {
					Interface::EntityPointer corePointer = pAllEntities->INtoIndex[input->PositionReference.Components[i].ReferenceTo];
					if (pAllEntities->EntityArraies[corePointer.Archetype].WorldPosition.Components[corePointer.Index].NeedUpdate ||
						input->GenerateFlag.Components[i].GeneratedOnThisTick) {
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
	class CharacterHurtbox : public System {
	public:
		CharacterHurtbox(GameSystem* pGameSystem) : System(pGameSystem) {
			SetArchetype(CompNames::AI);
			SetArchetype(CompNames::WorldPosition);
			SetArchetype(CompNames::CharacterData);
			SetArchetype(CompNames::CharacterHurtbox);
			SetArchetype(CompNames::DamagePool);
			SetArchetype(CompNames::GenerateFlag);
			SetArchetype(CompNames::MoveFlag);
			SetArchetype(CompNames::KillFlag);
			SetTargetList(pAllEntities);
		}
		void Update(SameArchetype* input) override {
			int Isize = input->ValidEntityCount;
			for (int i = 0; i < Isize; i++) {
				if (input->GenerateFlag.Components[i].GeneratedOnThisTick) {
					float radius = input->WorldPosition.Components[i].WorldPos.Ratio * input->CharacterHurtbox.Components[i].DiameterCoef/2;
					input->CharacterHurtbox.Components[i].Radius = radius;
					input->CharacterHurtbox.Components[i].Center = input->WorldPosition.Components[i].WorldPos.Parallel;
					int bottom = (int)roundf(input->WorldPosition.Components[i].WorldPos.Parallel.m128_f32[1] - radius);
					int top = (int)roundf(input->WorldPosition.Components[i].WorldPos.Parallel.m128_f32[1] + radius);
					int left = (int)roundf(input->WorldPosition.Components[i].WorldPos.Parallel.m128_f32[0] - radius);
					int right = (int)roundf(input->WorldPosition.Components[i].WorldPos.Parallel.m128_f32[0] + radius);
					input->CharacterHurtbox.Components[i].OccupingRectBottom = bottom;
					input->CharacterHurtbox.Components[i].OccupingRectTop = top;
					input->CharacterHurtbox.Components[i].OccupingRectLeft = left;
					input->CharacterHurtbox.Components[i].OccupingRectRight = right;
					for (int x = left; x <= right; x++) {
						for (int y = bottom; y <= top; y++) {
							pHurtboxes->SetCharacter(x, y, input->IdArray[i]);
						}
					}
				}
				if (input->WorldPosition.Components[i].NeedUpdate)
				{
					int OccupyingBottom = input->CharacterHurtbox.Components[i].OccupingRectBottom;
					int OccupyingTop = input->CharacterHurtbox.Components[i].OccupingRectTop;
					int OccupyingLeft = input->CharacterHurtbox.Components[i].OccupingRectLeft;
					int OccupyingRight = input->CharacterHurtbox.Components[i].OccupingRectRight;
					float radius = input->WorldPosition.Components[i].WorldPos.Ratio * input->CharacterHurtbox.Components[i].DiameterCoef/2;
					input->CharacterHurtbox.Components[i].Radius = radius;
					input->CharacterHurtbox.Components[i].Center = input->WorldPosition.Components[i].WorldPos.Parallel;
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
								pHurtboxes->DeleteCharacter(x, y, input->IdArray[i]);
							}
						}
						for (int x = CurrentLeft; x <= CurrentRight; x++) {
							for (int y = CurrentBottom; y <= CurrentTop; y++) {
								pHurtboxes->SetCharacter(x, y, input->IdArray[i]);
							}
						}
					}
					input->CharacterHurtbox.Components[i].OccupingRectBottom = CurrentBottom;
					input->CharacterHurtbox.Components[i].OccupingRectTop = CurrentTop;
					input->CharacterHurtbox.Components[i].OccupingRectLeft = CurrentLeft;
					input->CharacterHurtbox.Components[i].OccupingRectRight = CurrentRight;
				}
				if (input->KillFlag.Components[i].KillOnThisTick) {
					int bottom = input->CharacterHurtbox.Components[i].OccupingRectBottom;
					int top = input->CharacterHurtbox.Components[i].OccupingRectBottom;
					int left = input->CharacterHurtbox.Components[i].OccupingRectBottom;
					int right = input->CharacterHurtbox.Components[i].OccupingRectBottom;
					for (int x = left; x < right; x++) {
						for (int y = bottom; y < top; y++) {
							pHurtboxes->DeleteCharacter(x, y, input->IdArray[i]);
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
					//判定は1回、壁に衝突するかはcharacterData参照、キャラクターとの衝突もcharacterData参照、味方とは衝突しない
					input->GiveDamage.Components[i].Damage,nullptr, true, input->BulletData.Components[i].Team, input->BulletData.Components[i].InterfareToWall, input->BulletData.Components[i].InterfareToCharacter, false, -1);
				if (input->HitFlag.Components[i].IsHit && input->BulletData.Components[i].InterfareToWall) {
					
				}
			}
		}
	};
	class CheckCharacterCollision : public System {
	public:
		CheckCharacterCollision(GameSystem* pGameSystem) : System(pGameSystem) {
			SetArchetype(CompNames::Motion);
			SetArchetype(CompNames::WorldPosition);
			SetArchetype(CompNames::CharacterHurtbox);
			SetArchetype(CompNames::GiveDamage);
			SetArchetype(CompNames::CharacterData);
			SetArchetype(CompNames::HitFlag);
			SetArchetype(CompNames::PositionReference);
			SetTargetList(pAllEntities);
		}
		void Update(SameArchetype* input) override {
			int Isize = input->ValidEntityCount;
			for (int i = 0; i < Isize; i++) {
				DirectX::XMVECTOR zeros{ 0.0f,0.0f,0.0f,0.0f };
				input->HitFlag.Components[i].IsHit = false;
				//そもそも動こうとしている場合
				if (input->WorldPosition.Components[i].NeedUpdate) {

					if (i == 0 && false){
						OutputDebugStringA("Collision Check Start\n");
							char buffer[256];
							sprintf_s(buffer, 256, "World:%f,%f,%f,%f\n",
								input->WorldPosition.Components[i].WorldPos.Parallel.m128_f32[0],
								input->WorldPosition.Components[i].WorldPos.Parallel.m128_f32[1],
								input->WorldPosition.Components[i].WorldPos.Parallel.m128_f32[2],
								input->WorldPosition.Components[i].WorldPos.Parallel.m128_f32[3]);
							OutputDebugStringA(buffer);
							sprintf_s(buffer, 256, "Next World:%f,%f,%f,%f\n",
								input->WorldPosition.Components[i].NextTickWorldPos.Parallel.m128_f32[0],
								input->WorldPosition.Components[i].NextTickWorldPos.Parallel.m128_f32[1],
								input->WorldPosition.Components[i].NextTickWorldPos.Parallel.m128_f32[2],
								input->WorldPosition.Components[i].NextTickWorldPos.Parallel.m128_f32[3]);
							OutputDebugStringA(buffer);
					}
					input->HitFlag.Components[i].IsHit = pHurtboxes->CheckCircleCollid(
						input->WorldPosition.Components[i].NextTickWorldPos.Parallel,
						input->CharacterHurtbox.Components[i].Radius,
						//判定は1回、壁に衝突するかはcharacterData参照、キャラクターとは常時衝突、全チームとも常時衝突
						input->GiveDamage.Components[i].Damage, &input->DamagePool.Components[i].Damage,
						true, input->CharacterData.Components[i].Team, input->CharacterData.Components[i].InterfareToWall, true, true,
						input->PositionReference.Components[i].ReferenceTo);


					//衝突している場合
					if (input->HitFlag.Components[i].IsHit) {
						if (i == 0) {
							OutputDebugStringA("Collided\n");
						}
						Interface::EntityPointer CorePointer = pAllEntities->INtoIndex[input->PositionReference.Components[i].ReferenceTo];
						pAllEntities->EntityArraies[CorePointer.Archetype].HitFlag.Components[CorePointer.Index].IsHit = true;
						input->HitFlag.Components[i].IsHit = true;
						pAllEntities->EntityArraies[CorePointer.Archetype].WorldPosition.Components[CorePointer.Index].NeedUpdate = false;
					}
					else {
						//移動阻害が起きない場合は絶対位置を移動後のものに更新する
						input->CharacterHurtbox.Components[i].Center = input->WorldPosition.Components[i].NextTickWorldPos.Parallel;
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
	class CharacterAppearance : public System {
	public:
		CharacterAppearance(GameSystem* pGameSystem) : System(pGameSystem) {
			SetArchetype(CompNames::WorldPosition);
			SetArchetype(CompNames::CharacterAppearance);
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
					Interface::CharacterInstanceType* pInstance = nullptr;

					//影
					input->CharacterAppearance.Components[i].BufferDataIndex =
						pCharacterAppearance[0].Add(input->IdArray[i], &pInstance);
					DirectX::XMMATRIX shadowWorld{
						{input->WorldPosition.Components[i].WorldPos.Ratio, 0.0f, 0.0f, 0.0f},
						{0.0f, input->WorldPosition.Components[i].WorldPos.Ratio, 0.0f, 0.0f},
						{0.0f, 0.0f, 0.0f, 0.0f},
						DirectX::XMVectorAdd({0.0f, input->WorldPosition.Components[i].WorldPos.Ratio * -0.1f, 0.0f, 0.0f},input->WorldPosition.Components[i].WorldPos.Parallel)
					};
					pInstance->Set(&shadowWorld,
						&input->CharacterAppearance.Components[i].TexCoord[0],
						&input->CharacterAppearance.Components[i].Color0[0],
						&input->CharacterAppearance.Components[i].Color1[0],
						&input->CharacterAppearance.Components[i].Color2[0]);


					//本体
					pCharacterAppearance[1].Add(input->IdArray[i], &pInstance);
					DirectX::XMMATRIX baseWorld{
						{input->WorldPosition.Components[i].WorldPos.Ratio, 0.0f, 0.0f, 0.0f },
						{0.0f, input->WorldPosition.Components[i].WorldPos.Ratio, 0.0f, 0.0f},
						{0.0f, 0.0f, 0.0f, 0.0f},
						input->WorldPosition.Components[i].WorldPos.Parallel
					};
					pInstance->Set(&baseWorld,
						&input->CharacterAppearance.Components[i].TexCoord[1],
						&input->CharacterAppearance.Components[i].Color0[1],
						&input->CharacterAppearance.Components[i].Color1[1],
						&input->CharacterAppearance.Components[i].Color2[1]);

					//模様
					//上にあるものが奥に見えるのの再現
					pCharacterAppearance[2].Add(input->IdArray[i], &pInstance);
					DirectX::XMMATRIX patternWorld{
						0.0f, 0.0f, 0.0f, 0.0f,
							0.0f, 0.0f, 0.0f, 0.0f,
							0.0f, 0.0f, 0.0f, 0.0f,
							0.0f, input->WorldPosition.Components[i].WorldPos.Ratio*0.2f, 0.0f, 0.0f
					};
					patternWorld = patternWorld + input->WorldPosition.Components[i].WorldMatrix;
					pInstance->Set(&patternWorld,
						&input->CharacterAppearance.Components[i].TexCoord[2],
						&input->CharacterAppearance.Components[i].Color0[2],
						&input->CharacterAppearance.Components[i].Color1[2],
						&input->CharacterAppearance.Components[i].Color2[2]);


				}
				//見た目の更新
				if (input->MoveFlag.Components[i].Moved || input->AppearanceChanged.Components[i].Changed) {
					input->AppearanceChanged.Components[i].Changed = false;
					Interface::CharacterInstanceType* pInstance = nullptr;


					//影
					pCharacterAppearance[0].Update(input->CharacterAppearance.Components[i].BufferDataIndex, &pInstance);
					DirectX::XMMATRIX shadowWorld{
						{input->WorldPosition.Components[i].WorldPos.Ratio, 0.0f, 0.0f, 0.0f},
						{0.0f, input->WorldPosition.Components[i].WorldPos.Ratio, 0.0f, 0.0f},
						{0.0f, 0.0f, 0.0f, 0.0f},
						DirectX::XMVectorAdd({0.0f, input->WorldPosition.Components[i].WorldPos.Ratio * -0.1f, 0.0f, 0.0f},input->WorldPosition.Components[i].WorldPos.Parallel)
					};
					pInstance->Set(&shadowWorld,
						&input->CharacterAppearance.Components[i].TexCoord[0],
						&input->CharacterAppearance.Components[i].Color0[0],
						&input->CharacterAppearance.Components[i].Color1[0],
						&input->CharacterAppearance.Components[i].Color2[0]);


					pCharacterAppearance[1].Update(input->CharacterAppearance.Components[i].BufferDataIndex, &pInstance);
					DirectX::XMMATRIX baseWorld{
						{input->WorldPosition.Components[i].WorldPos.Ratio, 0.0f, 0.0f, 0.0f },
						{0.0f, input->WorldPosition.Components[i].WorldPos.Ratio, 0.0f, 0.0f},
						{0.0f, 0.0f, 0.0f, 0.0f},
						input->WorldPosition.Components[i].WorldPos.Parallel
					};
					pInstance->Set(&baseWorld,
						&input->CharacterAppearance.Components[i].TexCoord[1],
						&input->CharacterAppearance.Components[i].Color0[1],
						&input->CharacterAppearance.Components[i].Color1[1],
						&input->CharacterAppearance.Components[i].Color2[1]);


					pCharacterAppearance[2].Update(input->CharacterAppearance.Components[i].BufferDataIndex, &pInstance);
					DirectX::XMMATRIX patternWorld{
						0.0f, 0.0f, 0.0f, 0.0f,
						0.0f, 0.0f, 0.0f, 0.0f,
						0.0f, 0.0f, 0.0f, 0.0f,0.0f, 
						input->WorldPosition.Components[i].WorldPos.Ratio * 0.2f, 0.0f, 0.0f
					};
					patternWorld = patternWorld + input->WorldPosition.Components[i].WorldMatrix;
					pInstance->Set(&patternWorld,
						&input->CharacterAppearance.Components[i].TexCoord[2],
						&input->CharacterAppearance.Components[i].Color0[2],
						&input->CharacterAppearance.Components[i].Color1[2],
						&input->CharacterAppearance.Components[i].Color2[2]);
				}
				//見た目の削除
				if (input->KillFlag.Components[i].KillOnThisTick) {
					//移動させたSameArchIndexを受け取る
					Interface::SameArchIndex replaced = pCharacterAppearance->Delete(
						input->CharacterAppearance.Components[i].BufferDataIndex);
					//移動させたRectAppIdを変更する
					if (replaced != -1) {
						input->CharacterAppearance.Components[replaced].BufferDataIndex =
							input->CharacterAppearance.Components[i].BufferDataIndex;
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
					pBulletAppearance->Update(input->CharacterAppearance.Components[i].BufferDataIndex, &pInstance);
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
		SystemArray.push_back(new Systems::CoreUpdateWorld(pGameSystem));
		SystemArray.push_back(new Systems::NotCoreUpdateWorld(pGameSystem));
		//運動、意思決定
		SystemArray.push_back(new Systems::CharacterAction(pGameSystem));
		SystemArray.push_back(new Systems::CoreApplyLinearAcceralation(pGameSystem));

		//エフェクトの生成
		SystemArray.push_back(new Systems::GenerateHitEffect(pGameSystem));
		SystemArray.push_back(new Systems::GenerateTrajectory(pGameSystem));

		// これ以前の処理でこのティックの削除と追加の決定はすべて行われる必要がある
		// ここより下に削除と追加にかかわる処理はする必要がある
		
		//描画
		SystemArray.push_back(new Systems::BlockAppearance(pGameSystem));
		SystemArray.push_back(new Systems::CharacterAppearance(pGameSystem));
		SystemArray.push_back(new Systems::BulletAppearance(pGameSystem));

		//衝突マップに設定する
		SystemArray.push_back(new Systems::WallHurtbox(pGameSystem));
		SystemArray.push_back(new Systems::CharacterHurtbox(pGameSystem));

		// ここまでに削除と追加にかかわる処理は終わっている必要がある

		//削除追加
		SystemArray.push_back(new Systems::KillGenerate(pGameSystem));

		SystemArray.push_back(new Systems::CoreNextTickWorld(pGameSystem));
		SystemArray.push_back(new Systems::NotCoreNextTickWorld(pGameSystem));

		//衝突判定
		SystemArray.push_back(new Systems::CheckBulletCollision(pGameSystem));
		SystemArray.push_back(new Systems::CheckCharacterCollision(pGameSystem));
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