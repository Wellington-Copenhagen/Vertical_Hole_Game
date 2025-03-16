#pragma once
#include "../../Interfaces.h"
#include "Appearance.h"
#include "Input.h"
#include "HitBox.h"
#include "Entity.h"
#include "StringDraw.h"
extern GraphicalStringDraw<65536, 2048, 32> globalStringDraw;
extern int Tick;
//�֘A����V�X�e���͓������̂Ɏ��߂�����ǂ��ƍl������
class GameSystem;
class System {
public:
	Hurtboxes* pHurtboxes;
	Entities* pEntities;
	Appearances<Interface::BlockDrawCallType, Interface::BlockInstanceType, 1, WorldWidth* WorldHeight>* pFloorAppearance;
	Appearances<Interface::BlockDrawCallType, Interface::BlockInstanceType, 1, WorldWidth* WorldHeight>* pWallAppearance;
	//�e�A�{�́A�͗l
	Appearances<Interface::BallDrawCallType, Interface::BallInstanceType, 1, MaxBallCount>* pBallAppearance;
	Appearances<Interface::BulletDrawCallType, Interface::BulletInstanceType, 1, MaxBulletCount>* pBulletAppearance;
	System(GameSystem* pGameSystem);
	virtual void Update() = 0;
};
namespace Systems {
	//�ǉ��폜
	class KillGenerate : public System {
	public:
		KillGenerate(GameSystem* pGameSystem) : System(pGameSystem){}
		void Update() override {
			for (auto [entity, generateFlag, killFlag] : pEntities->Registry.view<
				Component::GenerateFlag, 
				Component::KillFlag>().each()) {
				generateFlag.GeneratedOnThisTick = false;
				if (killFlag.KillOnThisTick) {
					pEntities->Registry.destroy(entity);
				}
			}
		}
	};
	class ControlUnitCount : public System {
	public:
		ControlUnitCount(GameSystem* pGameSystem) : System(pGameSystem) {}
		void Update() override {
			for (auto [entity, generateFlag, killFlag, unitData] : pEntities->Registry.view<
				Component::GenerateFlag, 
				Component::KillFlag, 
				Component::UnitData>().each()) {
				if (generateFlag.GeneratedOnThisTick) {
					pEntities->UnitCountEachTeam[unitData.Team]++;
				}
				if (killFlag.KillOnThisTick) {
					pEntities->UnitCountEachTeam[unitData.Team]--;
				}
			}
		}
	};
	class BindCoreUnit : public System {
	public:
		BindCoreUnit(GameSystem* pGameSystem) : System(pGameSystem) {}
		void Update() override {
			for (auto [entity, ballData, generateFlag] : pEntities->Registry.view<
				Component::BallData,
				Component::GenerateFlag>().each()) {
				if (generateFlag.GeneratedOnThisTick) {
					pEntities->Registry.get<Component::UnitData>(ballData.CoreId).BallIds[
						pEntities->Registry.get<Component::UnitData>(ballData.CoreId).BallCount] = entity;
					pEntities->Registry.get<Component::UnitData>(ballData.CoreId).BallCount++;
				}
			}
		}
	};
	class CorpsAction : public System {
	public:
		void FormLine(Component::CorpsData& corpsData, DirectX::XMVECTOR pos, float heading, float formationWidth, float formationThickness) {
			int memberCount = corpsData.AllMemberUnit.size();
			float areaPerUnit = memberCount / (formationWidth * formationThickness);
			// ���ݕ����ɉ��l���邩
			int thicknessCount = std::ceil(formationThickness * std::sqrtf(areaPerUnit));
			int widthCount = std::ceil(memberCount / thicknessCount);
			// heading�̕�������ɂ��č��と�������E��̏��Ŕz�u���Ă���
			DirectX::XMVECTOR thickDirectionInterval = DirectX::XMVECTOR{
				std::cosf(heading - PI)*formationThickness / thicknessCount,
				std::sinf(heading - PI) * formationThickness / thicknessCount,
				0,0
			};
			DirectX::XMVECTOR widthDirectionInterval = DirectX::XMVECTOR{
				std::cosf(heading - PI / 2.0f) * formationWidth / widthCount,
				std::sinf(heading - PI / 2.0f) * formationWidth / widthCount,
				0,0
			};
			corpsData.CurrentMovementOrder = Interface::MovementOrder::HoldPosition;
			for (int i = 0; i < memberCount; i++) {
				DirectX::XMVECTOR nextPosition = DirectX::XMVectorAdd(pos, DirectX::XMVectorAdd(
					DirectX::XMVectorScale(thickDirectionInterval, i % thicknessCount - (thicknessCount - 1) / 2),
					DirectX::XMVectorScale(widthDirectionInterval, i / thicknessCount - (widthCount - 1) / 2)
				));
				Component::UnitData& unitData = pEntities->Registry.get<Component::UnitData>(corpsData.AllMemberUnit[i]);
				unitData.FinalTarget.Pos = nextPosition;
			}
		}
		void FormCenter(Component::CorpsData& corpsData, DirectX::XMVECTOR pos, float heading, float FormationWidth, float FormationThickness);
		void ChaseLeader(Component::CorpsData& corpsData) {
			corpsData.CurrentMovementOrder = Interface::MovementOrder::Chase;
			corpsData.DistanceThresholdMin = 1;
			corpsData.DistanceThresholdMax = 10;
			for (entt::entity member : corpsData.AllMemberUnit) {
				Component::UnitData& unitData = pEntities->Registry.get<Component::UnitData>(member);
				unitData.TargetEntity = corpsData.LeaderUnit;
			}
		}
		void Charge(Component::CorpsData& corpsData) {
			corpsData.CurrentMovementOrder = Interface::MovementOrder::Charge;
			corpsData.DistanceThresholdMin = 1;
			corpsData.DistanceThresholdMax = 10;
			for (entt::entity member : corpsData.AllMemberUnit) {
				Component::UnitData& unitData = pEntities->Registry.get<Component::UnitData>(member);
				unitData.TargetEntity = corpsData.LeaderUnit;
			}
		}
		void CeaseFire(Component::CorpsData& corpsData) {
			corpsData.CurrentAttackOrder = Interface::AttackOrder::CeaseFire;
		}
		void FireAtNearest(Component::CorpsData& corpsData) {
			corpsData.CurrentAttackOrder = Interface::AttackOrder::Nearest;
		}
		void FireAtStrongest(Component::CorpsData& corpsData) {
			corpsData.CurrentAttackOrder = Interface::AttackOrder::CeaseFire;
		}
		CorpsAction(GameSystem* pGameSystem):System(pGameSystem) {}
		void Update() override {
			for (auto [entity, corpsData, worldPosition] : pEntities->Registry.view<
			Component::CorpsData, Component::WorldPosition>().each()) {
				Charge(corpsData);
			}
		}
	};
	//�ӎv����
	class UnitAction : public System {
	public:
		UnitAction(GameSystem* pGameSystem) : System(pGameSystem) {}
		void Update() override {
			for (auto [entity, unitData, motion, worldPosition] : pEntities->Registry.view<
				Component::UnitData,
				Component::Motion,
				Component::WorldPosition>().each()) {
				//�v���C���[�̍s��
				if (entity == Interface::PlayingBall) {
					motion.WorldDelta.Parallel = { 0.0f, 0.0f, 0.0f ,0.0f };
					motion.WorldDelta.Ratio = 1;
					motion.WorldDelta.Rotate = 0;
					float moveTilePerTick = unitData.MoveTilePerTick * unitData.SpeedBuff;
					if (Input::KeyPushed.test(0x57)) {//w
						motion.WorldDelta.Parallel = { 0.0f, moveTilePerTick, 0.0f ,0.0f };
					}
					if (Input::KeyPushed.test(0x53)) {//s
						motion.WorldDelta.Parallel = { 0.0f, -1 * moveTilePerTick, 0.0f ,0.0f };
					}
					if (Input::KeyPushed.test(0x41)) {//a
						motion.WorldDelta.Parallel = { -1 * moveTilePerTick, 0.0f, 0.0f ,0.0f };
					}
					if (Input::KeyPushed.test(0x44)) {//d
						motion.WorldDelta.Parallel = { moveTilePerTick, 0.0f, 0.0f ,0.0f };
					}
					if (Input::KeyPushed.test(0x51)) {//q
						motion.WorldDelta.Rotate = 0.03f;
					}
					if (Input::KeyPushed.test(0x45)) {//e
						motion.WorldDelta.Rotate = -0.03f;
					}
				}
				//AI�̍s��
				else {
					Interface::MovementOrder movementOrder = pEntities->Registry.get<Component::CorpsData>(unitData.Leader).CurrentMovementOrder;
					Interface::AttackOrder attackOrder = pEntities->Registry.get<Component::CorpsData>(unitData.Leader).CurrentAttackOrder;
					float distanceThresholdMin = pEntities->Registry.get<Component::CorpsData>(unitData.Leader).DistanceThresholdMin;
					float distanceThresholdMax = pEntities->Registry.get<Component::CorpsData>(unitData.Leader).DistanceThresholdMax;
					if (Tick % 30 == 0) {
						int x = (int)worldPosition.WorldPos.Parallel.m128_f32[0];
						int y = (int)worldPosition.WorldPos.Parallel.m128_f32[1];
						if (movementOrder == Interface::MovementOrder::Chase) {
							pHurtboxes->GetNearestUnit(x, y, unitData.Team, unitData.AttackRange);
						}
					}
					if (movementOrder == Interface::MovementOrder::HoldPosition
						 || movementOrder == Interface::MovementOrder::Chase) {
						if (movementOrder == Interface::MovementOrder::Chase) {
							Component::WorldPosition* targetWorldPos = pEntities->Registry.try_get<Component::WorldPosition>(unitData.TargetEntity);
							if (targetWorldPos == nullptr) {

							}
							else {
								unitData.FinalTarget.Pos = targetWorldPos->WorldPos.Parallel;
							}
						}
						float length = DirectX::XMVector2Length(DirectX::XMVectorSubtract(unitData.AreaBorderTarget.Pos, worldPosition.WorldPos.Parallel)).m128_f32[0];
						if (length < 0.5) {
							// 2�ڂ̖ڕW�n�_�ɓ��B����
							int current = pEntities->GetAreaIndex(worldPosition.WorldPos.Parallel);
							int target = max(0, pEntities->GetAreaIndex(unitData.FinalTarget.Pos));

							while (true) {
								//1�G���A���i�߂Ă����ĒʘH���ʂ�Ȃ��Ȃ��������A�ڕW�G���A�܂ŒʘH���ʂ��Ă���Ƃ킩�������ɒT�����~�߂�
								if (unitData.NextPosReloadTick < Tick && current == target) {
									// ���ݒn�_�ƖڕW�n�_�����񂾒����̎��ӁA
									// ���ڕW����w�肳�ꂽ�����O����J���Ă���A
									// ���݈ʒu���璼�i�ł���o�H������ꏊ�����̈ړ��ڕW�Ƃ���
									if (distanceThresholdMax != 0) {
										DirectX::XMVECTOR gap = DirectX::XMVectorSubtract(worldPosition.WorldPos.Parallel, unitData.FinalTarget.Pos);
										gap = DirectX::XMVectorScale(gap, min(1.2, Interface::UniformRandFloat(distanceThresholdMin,distanceThresholdMax) / DirectX::XMVector2Length(gap).m128_f32[0]));
										gap = DirectX::XMVector2Transform(gap, DirectX::XMMatrixRotationZ(Interface::UniformRandFloat(PI / -4.0f, PI / 4.0f)));
										unitData.AreaBorderTarget.Pos = DirectX::XMVectorAdd(unitData.FinalTarget.Pos, gap);
										if (!pHurtboxes->IsWayClear(&unitData.AreaBorderTarget.Pos, &worldPosition.WorldPos.Parallel)) {
											unitData.AreaBorderTarget.Pos = worldPosition.WorldPos.Parallel;
										}
									}
									else {
										unitData.AreaBorderTarget = unitData.FinalTarget;
									}
									unitData.HeadingFree = true;
									unitData.NextPosReloadTick = Tick + 120;
									break;
								}
								Interface::WayPoint targetNotSureWayClear;
								targetNotSureWayClear = pEntities->mRouting.GetWayPoint(current, target, worldPosition.WorldPos.Parallel);
								if (pHurtboxes->IsWayClear(&targetNotSureWayClear.Pos, &worldPosition.WorldPos.Parallel)) {
									// ���̃G���A���E�̃E�F�C�|�C���g�܂ň꒼���ōs����ꍇ
									unitData.AreaBorderTarget = targetNotSureWayClear;
									current = pEntities->mRouting.AllArea[current].WhereToGo[target];
								}
								else {
									unitData.HeadingFree = false;
									break;
								}
							}
						}
						DirectX::XMVECTOR color = { 0,1,0,1 };


						globalStringDraw.Append("areaBorder", &color, &unitData.AreaBorderTarget.Pos, 1, 10, StrDrawPos::AsCenter);
					}
					// �ڕW�n�_�ւ̈ړ����s��

					{
						DirectX::XMVECTOR target = DirectX::XMVectorAdd(DirectX::XMVectorScale(
							unitData.AreaBorderTarget.Pos, 0.5), DirectX::XMVectorScale(
								worldPosition.WorldPos.Parallel, 0.5));
						if (!unitData.HeadingFree) {
							// �i�������ɐ��񂪂���ꍇ
							// �Ȃ��ꍇ�͈꒼���Ɍ�����
							float distance = DirectX::XMVector2Length(DirectX::XMVectorSubtract(unitData.AreaBorderTarget.Pos, worldPosition.WorldPos.Parallel)).m128_f32[0];
							target = DirectX::XMVectorAdd(target, DirectX::XMVectorScale(unitData.AreaBorderTarget.Heading, distance * -0.2));
						}
						float length = DirectX::XMVector2Length(DirectX::XMVectorSubtract(target, worldPosition.WorldPos.Parallel)).m128_f32[0];
						target = DirectX::XMVectorScale(target, min(1,unitData.MoveTilePerTick / length));
						motion.WorldDelta.Parallel = target;
					}
				}
			}
		}
	};
	//�^���Ɋւ���
	class UpdateWorld : public System {
	public:
		UpdateWorld(GameSystem* pGameSystem) : System(pGameSystem) {}
		void Update() override {
			for(auto [entity,motion,worldPosition,moveFlag,hitFlag,positionReference] : pEntities->Registry.view<
				Component::Motion,
				Component::WorldPosition,
				Component::MoveFlag,
				Component::HitFlag,
				Component::PositionReference>().each()) {
				DirectX::XMVECTOR zeros{ 0.0f,0.0f,0.0f,0.0f };
				if (positionReference.IsCore) {
					if (worldPosition.NeedUpdate) {
						moveFlag.Moved = true;
						worldPosition.WorldPos = worldPosition.WorldPos + motion.WorldDelta;
						worldPosition.WorldMatrix = worldPosition.WorldPos.GetMatrix();
					}
					else {
						moveFlag.Moved = false;
						worldPosition.NextTickWorldPos = worldPosition.WorldPos;
					}
				}
			}
			for (auto [entity, motion, worldPosition, moveFlag, hitFlag, positionReference,generateFlag] : pEntities->Registry.view <
				Component::Motion,
				Component::WorldPosition,
				Component::MoveFlag,
				Component::HitFlag,
				Component::PositionReference,
				Component::GenerateFlag > ().each()) {
				if (!positionReference.IsCore) {
					if (pEntities->Registry.get<Component::MoveFlag>(positionReference.ReferenceTo).Moved ||
						generateFlag.GeneratedOnThisTick) {
						worldPosition.WorldPos = worldPosition.LocalReferenceToCore *
							pEntities->Registry.get<Component::WorldPosition>(positionReference.ReferenceTo).NextTickWorldPos;
						moveFlag.Moved = true;
						worldPosition.WorldMatrix = worldPosition.WorldPos.GetMatrix();
					}
					else {
						moveFlag.Moved = false;
						worldPosition.NextTickWorldPos = worldPosition.WorldPos;
					}
				}
			}
		}
	};
	class CalcurateGeneratedWorld : public System {
	public:
		CalcurateGeneratedWorld(GameSystem* pGameSystem) : System(pGameSystem) {}
		void Update() override {
			for(auto [entity,worldPosition,positionReference,generateFlag,motion] : pEntities->Registry.view<
				Component::WorldPosition,
				Component::PositionReference,
				Component::GenerateFlag,
				Component::Motion>().each()) {
				if (positionReference.IsCore &&
					generateFlag.GeneratedOnThisTick) {
					worldPosition.WorldPos = motion.WorldDelta +
						worldPosition.WorldPos;
					worldPosition.NextTickWorldPos = worldPosition.WorldPos;
					worldPosition.WorldMatrix = worldPosition.WorldPos.GetMatrix();
				}
			}
			for (auto [entity, worldPosition, positionReference, generateFlag, motion] : pEntities->Registry.view<
				Component::WorldPosition,
				Component::PositionReference, 
				Component::GenerateFlag, 
				Component::Motion>().each()) {
				if (!positionReference.IsCore &&
					generateFlag.GeneratedOnThisTick) {
					worldPosition.WorldPos = worldPosition.LocalReferenceToCore *
						pEntities->Registry.get < Component::WorldPosition>(positionReference.ReferenceTo).WorldPos;
					worldPosition.NextTickWorldPos = worldPosition.WorldPos;
					worldPosition.WorldMatrix = worldPosition.WorldPos.GetMatrix();
				}
			}
		}
	};
	class CoreApplyLinearAcceralation : public System {
	public:
		CoreApplyLinearAcceralation(GameSystem* pGameSystem) : System(pGameSystem) {}
		void Update() override {
			for (auto [entity, motion, linearAcceralation, positionReference] : pEntities->Registry.view < Component::Motion, Component::LinearAcceralation, Component::PositionReference > ().each()) {
				if (positionReference.IsCore) {
					motion.WorldDelta = motion.WorldDelta +
						linearAcceralation.World;
				}
			}
		}
	};
	class CalculateNextTickWorld : public System {
	public:
		CalculateNextTickWorld(GameSystem* pGameSystem) : System(pGameSystem) {}
		void Update() override {
			
			for (auto [entity, worldPosition, positionReference, motion] : pEntities->Registry.view <
				Component::WorldPosition,
				Component::PositionReference,
				Component::Motion > ().each()) {
				DirectX::XMVECTOR zeros{ 0.0f,0.0f,0.0f,0.0f };
				if (positionReference.IsCore) {
					if (!DirectX::XMVector4Equal(zeros, motion.WorldDelta.Parallel) ||
						motion.WorldDelta.Ratio != 1 ||
						motion.WorldDelta.Rotate != 0) {
						worldPosition.NextTickWorldPos = motion.WorldDelta +
							worldPosition.WorldPos;
						worldPosition.NeedUpdate = true;// �Ƃ肠����true�ɂ��Ă��Ƃ�false�ɂ��Ă���
					}
					else {
						worldPosition.NeedUpdate = false;
					}
				}
			}
			for (auto [entity, worldPosition, positionReference, motion] : pEntities->Registry.view < 
				Component::WorldPosition, 
				Component::PositionReference,
				Component::Motion >().each()) {
				if (!positionReference.IsCore) {
					if (pEntities->Registry.get<Component::WorldPosition>(positionReference.ReferenceTo).NeedUpdate) {
						worldPosition.NextTickWorldPos = worldPosition.LocalReferenceToCore *
							pEntities->Registry.get<Component::WorldPosition>(positionReference.ReferenceTo).NextTickWorldPos;
						worldPosition.NeedUpdate = true;
					}
					else {
						worldPosition.NeedUpdate = false;
					}
				}
			}
		}
	};
	//�Փ˂Ɋւ���
	class WallHurtbox : public System {
	public:
		WallHurtbox(GameSystem* pGameSystem) : System(pGameSystem) {}
		void Update() override {

			for (auto [entity, wallHurtbox, worldPosition, generateFlag, killFlag] : pEntities->Registry.view <
				Component::WallHurtbox,
				Component::WorldPosition,
				Component::GenerateFlag,
				Component::KillFlag >().each()) {
				if (generateFlag.GeneratedOnThisTick) {
					pHurtboxes->SetWall((int)roundf(worldPosition.WorldPos.Parallel.m128_f32[0]),
						(int)roundf(worldPosition.WorldPos.Parallel.m128_f32[1]), entity);
				}
				if (killFlag.KillOnThisTick) {
					pHurtboxes->DeleteWall((int)roundf(worldPosition.WorldPos.Parallel.m128_f32[0]),
						(int)roundf(worldPosition.WorldPos.Parallel.m128_f32[1]));
				}
			}
		}
	};
	class BallHurtbox : public System {
	public:
		BallHurtbox(GameSystem* pGameSystem) : System(pGameSystem) {}
		void Update() override {

			for (auto [entity, worldPosition, unitData, unitOccupationbox, generateFlag, moveFlag, killFlag] : pEntities->Registry.view<
				Component::WorldPosition,
				Component::UnitData,
				Component::UnitOccupationbox,
				Component::GenerateFlag,
				Component::MoveFlag,
				Component::KillFlag	>().each()) {
				unitOccupationbox.AlredayChecked = false;
				if (generateFlag.GeneratedOnThisTick) {
					float radius = worldPosition.WorldPos.Ratio * 0.5;
					int bottom = (int)roundf(worldPosition.WorldPos.Parallel.m128_f32[1] - radius);
					int top = (int)roundf(worldPosition.WorldPos.Parallel.m128_f32[1] + radius);
					int left = (int)roundf(worldPosition.WorldPos.Parallel.m128_f32[0] - radius);
					int right = (int)roundf(worldPosition.WorldPos.Parallel.m128_f32[0] + radius);
					unitOccupationbox.OccupingRectBottom = bottom;
					unitOccupationbox.OccupingRectTop = top;
					unitOccupationbox.OccupingRectLeft = left;
					unitOccupationbox.OccupingRectRight = right;
					for (int x = left; x <= right; x++) {
						for (int y = bottom; y <= top; y++) {
							pHurtboxes->SetUnit(x, y, entity,unitData.Team);
						}
					}
				}
				if (worldPosition.NeedUpdate)
				{
					int OccupyingBottom = unitOccupationbox.OccupingRectBottom;
					int OccupyingTop = unitOccupationbox.OccupingRectTop;
					int OccupyingLeft = unitOccupationbox.OccupingRectLeft;
					int OccupyingRight = unitOccupationbox.OccupingRectRight;
					float radius = worldPosition.WorldPos.Ratio * 0.5;
					int CurrentBottom = (int)roundf(worldPosition.WorldPos.Parallel.m128_f32[1] - radius);
					int CurrentTop = (int)roundf(worldPosition.WorldPos.Parallel.m128_f32[1] + radius);
					int CurrentLeft = (int)roundf(worldPosition.WorldPos.Parallel.m128_f32[0] - radius);
					int CurrentRight = (int)roundf(worldPosition.WorldPos.Parallel.m128_f32[0] + radius);
					//��L�G���A���ς�����Ȃ�
					if (OccupyingBottom != CurrentBottom ||
						OccupyingTop != CurrentTop ||
						OccupyingLeft != CurrentLeft ||
						OccupyingRight != CurrentRight) {
						for (int x = OccupyingLeft; x <= OccupyingRight; x++) {
							for (int y = OccupyingBottom; y <= OccupyingTop; y++) {
								pHurtboxes->DeleteUnit(x, y, entity,unitData.Team);
							}
						}
						for (int x = CurrentLeft; x <= CurrentRight; x++) {
							for (int y = CurrentBottom; y <= CurrentTop; y++) {
								pHurtboxes->SetUnit(x, y, entity,unitData.Team);
							}
						}
					}
					unitOccupationbox.OccupingRectBottom = CurrentBottom;
					unitOccupationbox.OccupingRectTop = CurrentTop;
					unitOccupationbox.OccupingRectLeft = CurrentLeft;
					unitOccupationbox.OccupingRectRight = CurrentRight;
				}
				if (killFlag.KillOnThisTick) {
					int bottom = unitOccupationbox.OccupingRectBottom;
					int top = unitOccupationbox.OccupingRectBottom;
					int left = unitOccupationbox.OccupingRectBottom;
					int right = unitOccupationbox.OccupingRectBottom;
					for (int x = left; x < right; x++) {
						for (int y = bottom; y < top; y++) {
							pHurtboxes->DeleteUnit(x, y, entity,unitData.Team);
						}
					}
				}
			}
		}
	};
	//���e�B�b�N�ł̏Փ˂��Փ˂Ƃ݂Ȃ��A�ړ��̑j�Q�ƏՓ˂̔���𓯎��ɍs���B
	//GiveDamage�R���|�[�l���g�̑��݂��_���[�W�t�^���邱�Ƃ��A
	//�ђʂ���̂��A�ړ���j�Q�����̂��̃t���O�ɂ���ďՓˌ�ɂǂ��������^���ɂȂ��邩�����߂�B
	class CheckBulletCollision : public System {
	public:
		CheckBulletCollision(GameSystem* pGameSystem) : System(pGameSystem) {}
		void Update() override {
			
			for(auto [entity,motion,worldPosition,circleHitbox,giveDamage,bulletData,hitFlag] : pEntities->Registry.view<
				Component::Motion,
				Component::WorldPosition,
				Component::CircleHitbox,
				Component::GiveDamage,
				Component::BulletData,
				Component::HitFlag>().each()) {
				hitFlag.IsHit = false;
				hitFlag.IsHit = pHurtboxes->CheckCircleCollid(
					worldPosition.NextTickWorldPos.Parallel,
					circleHitbox.Radius,
					//�����1��A�ǂɏՓ˂��邩��ballData�Q�ƁA�L�����N�^�[�Ƃ̏Փ˂�ballData�Q�ƁA�����Ƃ͏Փ˂��Ȃ�
					&giveDamage.Damage, true, bulletData.Team);
				if (hitFlag.IsHit && bulletData.InterfareToWall) {
					
				}
			}
		}
	};
	class CheckUnitCollision : public System {
	public:
		CheckUnitCollision(GameSystem* pGameSystem) : System(pGameSystem) {}
		void Update() override {
			
			for(auto [entity,motion,worldPosition,unitOccupationbox,unitData,hitFlag] : pEntities->Registry.view<
				Component::Motion,
				Component::WorldPosition,
				Component::UnitOccupationbox,
				Component::UnitData,
				Component::HitFlag>().each()) {
				bool h = pEntities->Registry.get<Component::HitFlag>(entity).IsHit;
				unitOccupationbox.AlredayChecked = true;
				hitFlag.IsHit = false;
				//���������������Ƃ��Ă���ꍇ
				if (worldPosition.NeedUpdate) {
					//�Փ˂��Ă���ꍇ
					if (pHurtboxes->CheckUnitCollid(entity)) {
						hitFlag.IsHit = true;
					}
				}
			}
		}
	};
	//�����ڂɊւ���
	class BlockAppearance : public System {
	public:
		BlockAppearance(GameSystem* pGameSystem) : System(pGameSystem) {}
		void Update() override {
			
			for(auto [entity,worldPosition,blockAppearance,blockData,generateFlag,moveFlag,appearanceChanged,killFlag] : pEntities->Registry.view<
				Component::WorldPosition,
				Component::BlockAppearance,
				Component::BlockData,
				Component::GenerateFlag,
				Component::MoveFlag,
				Component::AppearanceChanged,
				Component::KillFlag>().each()) {
				if (blockData.OnTop) {
					// �e�̍X�V
					int x = (int)worldPosition.WorldPos.Parallel.m128_f32[0];
					int y = (int)worldPosition.WorldPos.Parallel.m128_f32[1];
					if (pEntities->ShouldUpdateShadow[y * WorldWidth + x]) {
						blockAppearance.TexCoord3M.m128_f32[2] = pEntities->GetWallShadow(x, y);
					}
					//�����ڂ̓o�^
					if (generateFlag.GeneratedOnThisTick) {
						Interface::BlockInstanceType* pInstance = nullptr;
						blockAppearance.BufferDataIndex =
							pWallAppearance->Add(entity, &pInstance);
						pInstance->Set
						(&worldPosition.WorldMatrix,
							&blockAppearance.TexCoord12, &blockAppearance.TexCoord3M);
					}
					//�����ڂ̍X�V
					if (moveFlag.Moved || appearanceChanged.Changed) {
						appearanceChanged.Changed = false;
						Interface::BlockInstanceType* pInstance = nullptr;
						pWallAppearance->Update(blockAppearance.BufferDataIndex, &pInstance);
						pInstance->Set(&worldPosition.WorldMatrix,
							&blockAppearance.TexCoord12, &blockAppearance.TexCoord3M);
					}
					//�����ڂ̍폜
					if (killFlag.KillOnThisTick) {
						pEntities->DeleteWall(x, y);
						//�ړ�������SameArchIndex���󂯎��
						entt::entity replaced = pWallAppearance->Delete(
							blockAppearance.BufferDataIndex);
						//�ړ�������RectAppId��ύX����
						Component::BlockAppearance* pReplace = pEntities->Registry.try_get<Component::BlockAppearance>(replaced);
						if (pReplace != nullptr) {
							pReplace->BufferDataIndex = blockAppearance.BufferDataIndex;
						}
					}
				}
				else {
					// �e�̍X�V
					int x = (int)worldPosition.WorldPos.Parallel.m128_f32[0];
					int y = (int)worldPosition.WorldPos.Parallel.m128_f32[1];
					if (pEntities->ShouldUpdateShadow[y * WorldWidth + x]) {
						blockAppearance.TexCoord3M.m128_f32[2] = pEntities->GetFloorShadow(x, y);
					}
					//�����ڂ̓o�^
					if (generateFlag.GeneratedOnThisTick) {
						Interface::BlockInstanceType* pInstance = nullptr;
						blockAppearance.BufferDataIndex =
							pFloorAppearance->Add(entity, &pInstance);
						pInstance->Set
						(&worldPosition.WorldMatrix,
							&blockAppearance.TexCoord12, &blockAppearance.TexCoord3M);
					}
					//�����ڂ̍X�V
					if (moveFlag.Moved || appearanceChanged.Changed) {
						appearanceChanged.Changed = false;
						Interface::BlockInstanceType* pInstance = nullptr;
						pFloorAppearance->Update(blockAppearance.BufferDataIndex, &pInstance);
						pInstance->Set(&worldPosition.WorldMatrix,
							&blockAppearance.TexCoord12, &blockAppearance.TexCoord3M);
					}
					//�����ڂ̍폜
					if (killFlag.KillOnThisTick) {
						//�ړ�������SameArchIndex���󂯎��
						entt::entity replaced = pFloorAppearance->Delete(
							blockAppearance.BufferDataIndex);
						//�ړ�������RectAppId��ύX����
						Component::BlockAppearance* pReplace = pEntities->Registry.try_get<Component::BlockAppearance>(replaced);
						if (pReplace != nullptr) {
							pReplace->BufferDataIndex = blockAppearance.BufferDataIndex;
						}
					}
				}
			}
			pEntities->ShouldUpdateShadow.reset();
		}
	};
	//�����ڂɊւ���
	class BallAppearance : public System {
	public:
		BallAppearance(GameSystem* pGameSystem) : System(pGameSystem) {}
		void Update() override {
			
			for(auto [entity,worldPosition,ballAppearance,generateFlag,moveFlag,appearanceChanged,killFlag] : pEntities->Registry.view<
				Component::WorldPosition,
				Component::BallAppearance,
				Component::GenerateFlag,
				Component::MoveFlag,
				Component::AppearanceChanged,
				Component::KillFlag>().each()) {
				//�����ڂ̓o�^
				if (generateFlag.GeneratedOnThisTick) {
					Interface::BallInstanceType* pInstance = nullptr;

					//�e
					ballAppearance.BufferDataIndex =
						pBallAppearance[0].Add(entity, &pInstance);
					DirectX::XMMATRIX shadowWorld{
						{worldPosition.WorldPos.Ratio, 0.0f, 0.0f, 0.0f},
						{0.0f, worldPosition.WorldPos.Ratio, 0.0f, 0.0f},
						{0.0f, 0.0f, 0.0f, 0.0f},
						DirectX::XMVectorAdd({0.0f, worldPosition.WorldPos.Ratio * -0.1f, 0.0f, 0.0f},worldPosition.WorldPos.Parallel)
					};
					pInstance->Set(&shadowWorld,
						&ballAppearance.TexCoord[0],
						&ballAppearance.Color0[0],
						&ballAppearance.Color1[0],
						&ballAppearance.Color2[0]);


					//�{��
					pBallAppearance[1].Add(entity, &pInstance);
					DirectX::XMMATRIX baseWorld{
						{worldPosition.WorldPos.Ratio, 0.0f, 0.0f, 0.0f },
						{0.0f, worldPosition.WorldPos.Ratio, 0.0f, 0.0f},
						{0.0f, 0.0f, 0.0f, 0.0f},
						worldPosition.WorldPos.Parallel
					};
					pInstance->Set(&baseWorld,
						&ballAppearance.TexCoord[1],
						&ballAppearance.Color0[1],
						&ballAppearance.Color1[1],
						&ballAppearance.Color2[1]);

					//�͗l
					//��ɂ�����̂����Ɍ�����̂̍Č�
					pBallAppearance[2].Add(entity, &pInstance);
					DirectX::XMMATRIX patternWorld{
						0.0f, 0.0f, 0.0f, 0.0f,
							0.0f, 0.0f, 0.0f, 0.0f,
							0.0f, 0.0f, 0.0f, 0.0f,
							0.0f, worldPosition.WorldPos.Ratio*0.2f, 0.0f, 0.0f
					};
					patternWorld = patternWorld + worldPosition.WorldMatrix;
					pInstance->Set(&patternWorld,
						&ballAppearance.TexCoord[2],
						&ballAppearance.Color0[2],
						&ballAppearance.Color1[2],
						&ballAppearance.Color2[2]);


				}
				//�����ڂ̍X�V
				if (moveFlag.Moved || appearanceChanged.Changed) {
					appearanceChanged.Changed = false;
					Interface::BallInstanceType* pInstance = nullptr;


					//�e
					pBallAppearance[0].Update(ballAppearance.BufferDataIndex, &pInstance);
					DirectX::XMMATRIX shadowWorld{
						{worldPosition.WorldPos.Ratio, 0.0f, 0.0f, 0.0f},
						{0.0f, worldPosition.WorldPos.Ratio, 0.0f, 0.0f},
						{0.0f, 0.0f, 0.0f, 0.0f},
						DirectX::XMVectorAdd({0.0f, worldPosition.WorldPos.Ratio * -0.1f, 0.0f, 0.0f},worldPosition.WorldPos.Parallel)
					};
					pInstance->Set(&shadowWorld,
						&ballAppearance.TexCoord[0],
						&ballAppearance.Color0[0],
						&ballAppearance.Color1[0],
						&ballAppearance.Color2[0]);


					pBallAppearance[1].Update(ballAppearance.BufferDataIndex, &pInstance);
					DirectX::XMMATRIX baseWorld{
						{worldPosition.WorldPos.Ratio, 0.0f, 0.0f, 0.0f },
						{0.0f, worldPosition.WorldPos.Ratio, 0.0f, 0.0f},
						{0.0f, 0.0f, 0.0f, 0.0f},
						worldPosition.WorldPos.Parallel
					};
					pInstance->Set(&baseWorld,
						&ballAppearance.TexCoord[1],
						&ballAppearance.Color0[1],
						&ballAppearance.Color1[1],
						&ballAppearance.Color2[1]);

					pBallAppearance[2].Update(ballAppearance.BufferDataIndex, &pInstance);
					DirectX::XMMATRIX patternWorld{
						0.0f, 0.0f, 0.0f, 0.0f,
						0.0f, 0.0f, 0.0f, 0.0f,
						0.0f, 0.0f, 0.0f, 0.0f,0.0f, 
						worldPosition.WorldPos.Ratio * 0.2f, 0.0f, 0.0f
					};
					patternWorld = patternWorld + worldPosition.WorldMatrix;
					pInstance->Set(&patternWorld,
						&ballAppearance.TexCoord[2],
						&ballAppearance.Color0[2],
						&ballAppearance.Color1[2],
						&ballAppearance.Color2[2]);
				}
				//�����ڂ̍폜
				if (killFlag.KillOnThisTick) {
					//�ړ�������SameArchIndex���󂯎��
					entt::entity replaced = pBallAppearance->Delete(
						ballAppearance.BufferDataIndex);
					//�ړ�������RectAppId��ύX����
					Component::BallAppearance* pReplace = pEntities->Registry.try_get<Component::BallAppearance>(replaced);
					if (pReplace != nullptr) {
						pReplace->BufferDataIndex = ballAppearance.BufferDataIndex;
					}
				}
			}
		}
	};
	//�����ڂɊւ���
	class BulletAppearance : public System {
	public:
		BulletAppearance(GameSystem* pGameSystem) : System(pGameSystem) {}
		void Update() override {
			
			for(auto [entity,worldPosition,bulletAppearance,generateFlag,moveFlag,appearanceChanged,killFlag] : pEntities->Registry.view<
				Component::WorldPosition,
				Component::BulletAppearance,
				Component::GenerateFlag,
				Component::MoveFlag,
				Component::AppearanceChanged,
				Component::KillFlag>().each()) {
				//�����ڂ̓o�^
				if (generateFlag.GeneratedOnThisTick) {
					Interface::BulletInstanceType* pInstance = nullptr;
					bulletAppearance.BufferDataIndex =
						pBulletAppearance->Add(entity, &pInstance);
					pInstance->Set(&worldPosition.WorldMatrix,
						&bulletAppearance.TexCoord,
						&bulletAppearance.Color0,
						&bulletAppearance.Color0);
				}
				//�����ڂ̍X�V
				if (moveFlag.Moved || appearanceChanged.Changed) {
					appearanceChanged.Changed = false;
					Interface::BulletInstanceType* pInstance = nullptr;
					pBulletAppearance->Update(bulletAppearance.BufferDataIndex, &pInstance);
					pInstance->Set(&worldPosition.WorldMatrix,
						&bulletAppearance.TexCoord,
						&bulletAppearance.Color0,
						&bulletAppearance.Color0);
				}
				//�����ڂ̍폜
				if (killFlag.KillOnThisTick) {
					//�ړ�������SameArchIndex���󂯎��
					entt::entity replaced = pBulletAppearance->Delete(
						bulletAppearance.BufferDataIndex);
					//�ړ�������RectAppId��ύX����
					Component::BulletAppearance* pReplace = pEntities->Registry.try_get<Component::BulletAppearance>(replaced);
					if (pReplace != nullptr) {
						pReplace->BufferDataIndex = bulletAppearance.BufferDataIndex;
					}
				}
			}
		}
	};
	//�I�u�W�F�N�g�̐����Ɋւ���
	class GenerateHitEffect : public System {
	public:
		GenerateHitEffect(GameSystem* pGameSystem) : System(pGameSystem) {}
		void Update() override {
			
			for(auto [entity,worldPosition,hitEffece,hitFlag] : pEntities->Registry.view<
				Component::WorldPosition,
				Component::HitEffect,
				Component::HitFlag>().each()) {
			}
		}
	};
	class GenerateTrajectory : public System {
	public:
		GenerateTrajectory(GameSystem* pGameSystem) : System(pGameSystem) {}
		void Update() override {
			for (auto [entity,worldPosition,trajectory] : pEntities->Registry.view<
				Component::WorldPosition,
				Component::Trajectory>().each()) {
			}
		}
	};
	// �~�b�V�����i�s�Ɋւ���
	class InvationObserve : public System {
	public:
		InvationObserve(GameSystem* pGameSystem) : System(pGameSystem) {}
		void Update() override {
			for (auto [entity,spawn,invationObservance] : pEntities->Registry.view<
				Component::Spawn,
				Component::InvationObservance>().each()) {
			}
		}
	};
	class UnitCountObserve : public System {
	public:
		UnitCountObserve(GameSystem* pGameSystem) : System(pGameSystem) {}
		void Update() override {
			
			for(auto [entity,spawn,unitCountObservance] : pEntities->Registry.view<
				Component::Spawn,
				Component::UnitCountObservance>().each()) {
				// �܂��t���O�������ĂȂ�(�܂��X�|�[�����s���ĂȂ�)���{�[�_�[�𒴂����ꍇ
				if (!spawn.SpawnFlag && 
					pEntities->UnitCountEachTeam[unitCountObservance.Team] <= unitCountObservance.Border) {
					spawn.SpawnFlag = true;
				}
			}
		}
	};
	class Spawn : public System {
	public:
		Spawn(GameSystem* pGameSystem) : System(pGameSystem) {}
		void Update() override {
			
			for (auto [entity,spawn] : pEntities->Registry.view < Component::Spawn > ().each()) {
				if (spawn.SpawnFlag) {
					spawn.TimeGapSince--;

					//�x�ꕪ�������ς�
					if (spawn.TimeGapSince <= 0) {
						// �������̃X�|�[��
						{
							Interface::UnitIndex leader = spawn.Leader;
							//��L�G���A�̃T�C�Y
							//���a1.5�̉~�ƂȂ�̂�1.5�{
							float radius = pEntities->UnitInfos[leader].Ratio * 0.5;
							float Xpos = Interface::UniformRandFloat(spawn.SpawnAreaLeft, spawn.SpawnAreaRight);
							float Ypos = Interface::UniformRandFloat(spawn.SpawnAreaBottom, spawn.SpawnAreaTop);
							if (pHurtboxes->IsAbleToSpawn(radius, radius, radius, radius, Xpos, Ypos)) {
								Interface::RelationOfCoord pos = Interface::RelationOfCoord();
								pos.Parallel = {
									(float)Xpos,(float)Ypos,0,1
								};
								spawn.CoreData.IsLeader = true;
								spawn.CoreData.LeaderId = pEntities->EmplaceUnitWithInitData(leader, &spawn.CoreData, &pos);
							}
						}
						// �����̃X�|�[��
						{
							spawn.CoreData.IsLeader = false;
							Interface::UnitIndex member = spawn.Member;
							//��L�G���A�̃T�C�Y
							//���a1.5�̉~�ƂȂ�̂�1.5�{
							float radius = pEntities->UnitInfos[member].Ratio * 0.5;
							while (0 < spawn.CountLeft) {
								float Xpos = Interface::UniformRandFloat(spawn.SpawnAreaLeft, spawn.SpawnAreaRight);
								float Ypos = Interface::UniformRandFloat(spawn.SpawnAreaBottom, spawn.SpawnAreaTop);
								if (pHurtboxes->IsAbleToSpawn(radius, radius, radius, radius, Xpos, Ypos)) {
									Interface::RelationOfCoord pos = Interface::RelationOfCoord();
									pos.Parallel = {
										(float)Xpos,(float)Ypos,0,1
									};
									pEntities->EmplaceUnitWithInitData(member, &spawn.CoreData, &pos);
									spawn.CountLeft--;
								}
							}
						}
					}
				}
			}
		}
	};
	/*
	class TestA : public System {
	public:
		TestA(GameSystem* pGameSystem) : System(pGameSystem) {}
		void Update() override {
			
			for(auto [entity,test1,testResult] : pEntities->Registry.view<
				Component::Test1,
				Component::TestResult>().each()) {
				testResult.UpdatedByTestA = true;
			}
		}
	};
	class TestB : public System {
	public:
		TestB(GameSystem* pGameSystem) : System(pGameSystem) {}
		void Update() override {
			
			for(auto [entity,test2,testResult] : pEntities->Registry.view<
				Component::Test2,
				Component::TestResult>().each()) {
				testResult.UpdatedByTestB = true;
			}
		}
	};
	class TestC : public System {
	public:
		TestC(GameSystem* pGameSystem) : System(pGameSystem) {}
		void Update() override {
			
			for(auto [entity,test1,test2,testResult] : pEntities->Registry.view<
				Component::Test1,
				Component::Test2,
				Component::TestResult>().each()) {
				testResult.UpdatedByTestC = true;
			}
		}
	};
	class TestD : public System {
	public:
		TestD(GameSystem* pGameSystem) : System(pGameSystem) {}
		void Update() override {
			
			for (auto [entity,testResult] : pEntities->Registry.view < Component::TestResult > ().each()) {
				testResult.UpdatedByTestD = true;
			}
		}
	};
	*/
}
// ���e�B�b�N�̏�Ԃ�`�悵����A���e�B�b�N�̈ʒu��\�z���ďՓ˂ɂ��������̎󂯓n���ƈړ��������s��
// �f�[�^�̏����͎��e�B�b�N
// �����������蔻��〈���ڂ̔ɉh���폜�ǉ����ՓˁE�ړ����`��
class AllSystem {
public:
	std::vector<System*> SystemArray;
	AllSystem() {
		SystemArray = std::vector<System*>();
	}
	AllSystem(GameSystem* pGameSystem) {
		SystemArray = std::vector<System*>();
		//�ʒu�̍X�V

		SystemArray.push_back(new Systems::UpdateWorld(pGameSystem));

		//�~�b�V�����i�s
		SystemArray.push_back(new Systems::InvationObserve(pGameSystem));
		SystemArray.push_back(new Systems::UnitCountObserve(pGameSystem));
		SystemArray.push_back(new Systems::Spawn(pGameSystem));
		//�^���A�ӎv����
		SystemArray.push_back(new Systems::CorpsAction(pGameSystem));
		SystemArray.push_back(new Systems::UnitAction(pGameSystem));
		SystemArray.push_back(new Systems::CoreApplyLinearAcceralation(pGameSystem));

		//�G�t�F�N�g�̐���
		SystemArray.push_back(new Systems::GenerateHitEffect(pGameSystem));
		SystemArray.push_back(new Systems::GenerateTrajectory(pGameSystem));

		// ����ȑO�̏����ł��̃e�B�b�N�̍폜�ƒǉ��̌���͂��ׂčs����K�v������

		// ������艺�ɍ폜�ƒǉ��ɂ�����鏈���͂���K�v������

		SystemArray.push_back(new Systems::ControlUnitCount(pGameSystem));
		SystemArray.push_back(new Systems::BindCoreUnit(pGameSystem));

		SystemArray.push_back(new Systems::CalcurateGeneratedWorld(pGameSystem));
		//�`��
		SystemArray.push_back(new Systems::BlockAppearance(pGameSystem));
		SystemArray.push_back(new Systems::BallAppearance(pGameSystem));
		SystemArray.push_back(new Systems::BulletAppearance(pGameSystem));

		//�Փ˃}�b�v�ɐݒ肷��
		SystemArray.push_back(new Systems::WallHurtbox(pGameSystem));
		SystemArray.push_back(new Systems::BallHurtbox(pGameSystem));

		// �����܂łɍ폜�ƒǉ��ɂ�����鏈���͏I����Ă���K�v������

		//�폜�ǉ�
		SystemArray.push_back(new Systems::KillGenerate(pGameSystem));

		SystemArray.push_back(new Systems::CalculateNextTickWorld(pGameSystem));

		//�Փ˔���
		SystemArray.push_back(new Systems::CheckBulletCollision(pGameSystem));
		SystemArray.push_back(new Systems::CheckUnitCollision(pGameSystem));
	}
	void Update() {
		int size = SystemArray.size();
		OutputDebugStringA("Tick Start\n");
		for (int i = 0; i < size; i++) {
			SystemArray[i]->Update();
		}
		OutputDebugStringA("Tick End\n");
	}
};