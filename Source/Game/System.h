#pragma once
#include "../../Interfaces.h"
#include "Source/DirectX/Appearance.h"
#include "Input.h"
#include "HitBox.h"
#include "Entity.h"
#include "Source/DirectX/StringDraw.h"
#include "Source/Game/Camera.h"
extern GraphicalStringDraw globalStringDraw;
extern int Tick;
//�֘A����V�X�e���͓������̂Ɏ��߂�����ǂ��ƍl������
class GameSystem;
class System {
public:
	Hurtboxes* pHurtboxes;
	Entities* pEntities;
	EntityBindAppearances<Interface::BlockDCType, Interface::BlockIType, Component::BlockAppearance, 1>* pFloorAppearances;
	EntityBindAppearances<Interface::BlockDCType, Interface::BlockIType, Component::BlockAppearance, 1>* pWallAppearances;
	//�e�A�{�́A�͗l
	EntityBindAppearances < Interface::GeneralDCType, Interface::GeneralIType, Component::ConstantAppearance, 1 > *pConstantAppearances;
	EntityBindAppearances < Interface::EffectDCType, Interface::EffectIType, Component::VariableAppearance, 1 >* pVariableAppearances;

	EntityBindAppearances < Interface::GeneralDCType, Interface::GeneralIType, Component::ShadowAppearance, 1 >* pConstantShadowAppearances;
	EntityBindAppearances < Interface::GeneralDCType, Interface::GeneralIType, Component::ShadowAppearance, 1 >* pVariableShadowAppearances;
	System(GameSystem* pGameSystem);
	virtual void Update() = 0;
};
namespace Systems {
	//�ǉ��폜
	class KillGenerate : public System {
	public:
		KillGenerate(GameSystem* pGameSystem) : System(pGameSystem){}
		void Update() override {
			int count = 0;
			for (auto [entity, generateFlag, killFlag, worldPosition,motion] : pEntities->Registry.view<
				Component::GenerateFlag, 
				Component::KillFlag,
				Component::WorldPosition,
				Component::Motion>().each()) {
				generateFlag.GeneratedOnThisTick = false;
				if (killFlag.KillOnThisTick) {
					//Interface::OutputVector(worldPosition.WorldPos.r[3], "Killed", "");
					globalStringDraw.SimpleAppend("kill", 0, 0, 1, worldPosition.WorldPos.r[3], 1, 20, StrDrawPos::AsBottomLeftCorner);
					pEntities->Registry.destroy(entity);
				}
				count++;
			}
			if (Tick % 60 == 0) {
				for (auto [entity, generateFlag, killFlag, worldPosition] : pEntities->Registry.view<
					Component::GenerateFlag,
					Component::KillFlag,
					Component::WorldPosition>().each()) {
					generateFlag.GeneratedOnThisTick = false;
					if (killFlag.KillOnThisTick) {
						//Interface::OutputVector(worldPosition.WorldPos.r[3], "Killed", "");
						globalStringDraw.SimpleAppend("kill", 0, 0, 1, worldPosition.WorldPos.r[3], 1, 20, StrDrawPos::AsBottomLeftCorner);
						pEntities->Registry.destroy(entity);
					}
					count++;
				}
			}
			//Interface::fDebugOutput("entity count:%d\n", count);
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
	class BindCoreBall : public System {
	public:
		BindCoreBall(GameSystem* pGameSystem) : System(pGameSystem) {}
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
	class BindLeaderMember : public System {
	public:
		BindLeaderMember(GameSystem* pGameSystem) : System(pGameSystem) {}
		void Update() override {
			for (auto [entity, unitData, generateFlag] : pEntities->Registry.view<
				Component::UnitData,
				Component::GenerateFlag>().each()) {
				if (generateFlag.GeneratedOnThisTick && entity != unitData.Leader) {
					pEntities->Registry.get<Component::CorpsData>(unitData.Leader).AllMemberUnit.push_back(entity);
				}
			}
		}
	};
	class CorpsAction : public System {
	public:
		void FormLine(Component::CorpsData& corpsData, DirectX::XMVECTOR pos, float heading, float formationWidth, float formationThickness) {
			int memberCount = corpsData.AllMemberUnit.size() + 1;// ���[�_�[��1���₷
			float areaPerUnit = memberCount / (formationWidth * formationThickness);
			// ���ݕ����ɉ��l���邩
			int thicknessCount = std::ceil(formationThickness * std::sqrtf(areaPerUnit));
			int widthCount = std::ceil((float)memberCount / (float)thicknessCount);
			int leaderPos = min(memberCount - 1, (widthCount / 2) * thicknessCount + (thicknessCount / 2));
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
			bool afterLeader = false;
			for (int i = 0; i < memberCount; i++) {
				if (i == leaderPos) {
					DirectX::XMVECTOR nextPosition = DirectX::XMVectorAdd(pos, DirectX::XMVectorAdd(
						DirectX::XMVectorScale(thickDirectionInterval, i % thicknessCount - (thicknessCount - 1) / 2),
						DirectX::XMVectorScale(widthDirectionInterval, i / thicknessCount - (widthCount - 1) / 2)
					));
					Component::UnitData& unitData = pEntities->Registry.get<Component::UnitData>(corpsData.LeaderUnit);
					unitData.FinalTarget.Pos = nextPosition;
					afterLeader=true;
				}
				else {
					DirectX::XMVECTOR nextPosition;
					if (afterLeader) {
						nextPosition = DirectX::XMVectorAdd(pos, DirectX::XMVectorAdd(
							DirectX::XMVectorScale(thickDirectionInterval, i % thicknessCount - (thicknessCount - 1) / 2),
							DirectX::XMVectorScale(widthDirectionInterval, i / thicknessCount - (widthCount - 1) / 2)
						));
						Component::UnitData& unitData = pEntities->Registry.get<Component::UnitData>(corpsData.AllMemberUnit[i - 1]);
						unitData.FinalTarget.Pos = nextPosition;
						unitData.DistanceThresholdMin = 0;
						unitData.DistanceThresholdMax = 0;
					}
					else {
						nextPosition = DirectX::XMVectorAdd(pos, DirectX::XMVectorAdd(
							DirectX::XMVectorScale(thickDirectionInterval, i % thicknessCount - (thicknessCount - 1) / 2),
							DirectX::XMVectorScale(widthDirectionInterval, i / thicknessCount - (widthCount - 1) / 2)
						));
						Component::UnitData& unitData = pEntities->Registry.get<Component::UnitData>(corpsData.AllMemberUnit[i]);
						unitData.FinalTarget.Pos = nextPosition;
						unitData.DistanceThresholdMin = 0;
						unitData.DistanceThresholdMax = 0;
					}
				}
			}
		}
		void FormCenter(Component::CorpsData& corpsData, DirectX::XMVECTOR pos, float heading, float FormationWidth, float FormationThickness);
		void ChaseLeader(Component::CorpsData& corpsData,DirectX::XMVECTOR leaderTarget) {
			corpsData.CurrentMovementOrder = Interface::MovementOrder::Chase;
			int memberCount = corpsData.AllMemberUnit.size();
			for (int i = 0; i < memberCount; i++) {
				Component::UnitData& unitData = pEntities->Registry.get<Component::UnitData>(corpsData.AllMemberUnit[i]);
				unitData.TargetEntity = corpsData.LeaderUnit;
				unitData.DistanceThresholdMax = 10;
				unitData.DistanceThresholdMin = 1;
			}
		}
		void Charge(Component::CorpsData& corpsData) {
			corpsData.CurrentMovementOrder = Interface::MovementOrder::Charge;
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
			int i = 0;
			for (auto [entity, corpsData, worldPosition] : pEntities->Registry.view<
			Component::CorpsData, Component::WorldPosition>().each()) {
				for (int i = 0; i < corpsData.AllMemberUnit.size(); i++) {
					if (!pEntities->Registry.valid(corpsData.AllMemberUnit[i])) {
						corpsData.AllMemberUnit.erase(corpsData.AllMemberUnit.begin() + i);
						i--;
					}
				}
				if (i == 0) {
					FormLine(corpsData, { 12,40,0,1 }, 0, 16, 10);
				}
				if (i == 2) {
					ChaseLeader(corpsData, { 0,0,0,1 });
				}
				if (i == 1) {
					switch (Tick%1200)
					{
					case(0):
						FormLine(corpsData, { 50,40,0,1 }, PI / 2, 10, 4);
						break;
					case(300):
						FormLine(corpsData, { 50,70,0,1 }, PI / 2, 10, 4);
						break;
					case(600):
						FormLine(corpsData, { 50,40,0,1 }, PI / 2, 10, 4);
						break;
					case(900):
						FormLine(corpsData, { 50,12,0,1 }, PI / 2, 10, 4);
						break;
					default:
						break;
					}
					Charge(corpsData);
					FireAtNearest(corpsData);
				}
				i++;
			}
		}
	};
	//�ӎv����
	class UnitAction : public System {
	public:
		UnitAction(GameSystem* pGameSystem) : System(pGameSystem) {}
		void Update() override {
			for (auto [entity, unitData, motion, worldPosition, damagePool,killFlag] : pEntities->Registry.view<
				Component::UnitData,
				Component::Motion,
				Component::WorldPosition,
				Component::DamagePool,
			Component::KillFlag>().each()) {
				globalStringDraw.SimpleAppend(std::to_string((int)entity), 0, 0, 0, worldPosition.WorldPos.r[3], 1, 1, StrDrawPos::AsCenter);
				//�v���C���[�̍s��
				if (entity == pEntities->PlayingUnit) {
					motion.WorldDelta = Interface::SetScale(&motion.WorldDelta, 1);
					float moveTilePerTick = unitData.MoveTilePerTick * unitData.SpeedBuff;
					DirectX::XMVECTOR vector = { 0,0,0,0 };
					if (Input::KeyPushed.test(0x57)) {//w
						vector = DirectX::XMVectorAdd(vector, { 0.0f, 1.0f, 0.0f ,0.0f });
					}
					if (Input::KeyPushed.test(0x53)) {//s
						vector = DirectX::XMVectorAdd(vector, { 0.0f, -1.0f, 0.0f ,0.0f });
					}
					if (Input::KeyPushed.test(0x41)) {//a
						vector = DirectX::XMVectorAdd(vector, { -1.0f, 0.0f, 0.0f ,0.0f });
					}
					if (Input::KeyPushed.test(0x44)) {//d
						vector = DirectX::XMVectorAdd(vector, { 1.0f, 0.0f, 0.0f ,0.0f });
					}

					// �}�E�X�|�C���^�[�̕��Ɍ�������
					DirectX::XMVECTOR toMouse = DirectX::XMVectorSubtract(Camera::MousePointingWorldPos(Input::MouseX, Input::MouseY), worldPosition.WorldPos.r[3]);
					float pointFor = atan2(toMouse.m128_f32[1], toMouse.m128_f32[0]);
					motion.Headfor(pointFor, &worldPosition.WorldPos, 0.1f);
					Camera::CenterCoord = worldPosition.WorldPos.r[3];
					float length = DirectX::XMVector2Length(vector).m128_f32[0];
					vector = DirectX::XMVectorScale(vector, min(1, unitData.MoveTilePerTick / length));
					motion.WorldDelta.r[3] = DirectX::XMVectorScale(motion.WorldDelta.r[3], 0.8);
					motion.WorldDelta.r[3] = DirectX::XMVectorAdd(motion.WorldDelta.r[3], vector);
					if (Input::MouseLeftPushed) {
						unitData.Fire = true;
						unitData.AttackAt = Camera::MousePointingWorldPos(Input::MouseX, Input::MouseY);
					}
					else {
						unitData.Fire = false;
					}



				}
				//AI�̍s��
				else {
					// �ڕW�n�_�ւ̋����̊���������邩�A�������ʂ��Ă��Ȃ��ꍇ�i�r�Q�[�V�����𗊂��ĖڕW�n�_�֐ڋ߂���
					// �߂�����ꍇ�͗��������ֈړ�����x�N�g���𑫂������_���ړ�
					// ��������Ă���ꍇ�̓����_���Ɉړ����s��
					// �n�_�̐ݒ���s���̂ł͂Ȃ��ړ������Ǝ��Ԃ����ݒ肷��
					Component::CorpsData& corpsData = pEntities->Registry.get<Component::CorpsData>(unitData.Leader);
					if (corpsData.CurrentMovementOrder == Interface::MovementOrder::HoldPosition
						 || corpsData.CurrentMovementOrder == Interface::MovementOrder::Chase
						|| corpsData.CurrentMovementOrder == Interface::MovementOrder::Charge) {
						// Charge���[�h�ōł��߂��G��T���čX�V����
						if (corpsData.CurrentMovementOrder == Interface::MovementOrder::Charge && Tick%10==0) {
							auto nearest = pEntities->GetNearestHostilingUnit(worldPosition.WorldPos.r[3], unitData.Team);
							// ���G�͈�
							if (nearest.second < unitData.AttackRange * 2) {
								// �U���Ώۂ�����ꍇ
								unitData.TargetEntity = nearest.first;
								unitData.DistanceThresholdMax = unitData.AttackRange * 0.9;
								unitData.DistanceThresholdMin = unitData.AttackRange * 0.7;
							}
							else {
								// �U���Ώۂ����Ȃ��ꍇ
								// ���[�_�[�ɒǏ]����
								unitData.TargetEntity = unitData.Leader;
								unitData.DistanceThresholdMax = 10;
								unitData.DistanceThresholdMin = 1;

							}
						}
						if ((corpsData.CurrentMovementOrder == Interface::MovementOrder::Charge || corpsData.CurrentMovementOrder == Interface::MovementOrder::Chase) && Tick % 5 == 0) {
							Component::WorldPosition* targetWorldPos = pEntities->Registry.try_get<Component::WorldPosition>(unitData.TargetEntity);
							if (targetWorldPos == nullptr) {
								// �W�I���������Ă�ꍇ
								// �������񂻂̏�ҋ@�ɂ��Ď��̌����ŐV�����W�I��T��
								unitData.FinalTarget.Pos = worldPosition.WorldPos.r[3];
								unitData.DistanceThresholdMax = 0;
								unitData.DistanceThresholdMin = 0;
							}
							else {
								unitData.FinalTarget.Pos = targetWorldPos->WorldPos.r[3];
							}
						}
						// ���̎��_��FinalTarget�͊��S�ɍX�V�ς�

						DirectX::XMVECTOR toFinal = DirectX::XMVectorSubtract(unitData.FinalTarget.Pos, worldPosition.WorldPos.r[3]);
						float lengthToFinal = DirectX::XMVector2Length(toFinal).m128_f32[0];
						if (Tick % 10 == 0) {
							if (lengthToFinal < unitData.DistanceThresholdMax &&
								pHurtboxes->IsWayClear(&worldPosition.WorldPos.r[3], &unitData.FinalTarget.Pos)) {
								// �����A�ː������Ȃ�
								// �����_���ړ��ֈڍs
								unitData.VectorNavigation = true;
								if (unitData.NextPosReloadTick < Tick) {
									DirectX::XMVECTOR navigateHeading = { 0,0,0,0 };
									if (lengthToFinal < unitData.DistanceThresholdMin) {
										globalStringDraw.SimpleAppend("Too Near", 0, 1, 0, worldPosition.WorldPos.r[3], 0.5, 1, StrDrawPos::AsCenter);
										if (lengthToFinal < 1) {
											navigateHeading = DirectX::XMVectorAdd(navigateHeading, Interface::NormalRandHeadingVector(0, 2 * PI, 0));
										}
										else {
											// ���Ε����Ɍ����킹��
											navigateHeading = DirectX::XMVectorAdd(navigateHeading,
												DirectX::XMVectorScale(toFinal, 0.2 * (lengthToFinal - unitData.DistanceThresholdMin) / lengthToFinal));
										}
									}


									// �����ł̗U���ɂȂ��Ă��Ă��N�[���^�C������������Ă���
									unitData.Heading = DirectX::XMVectorAdd(Interface::NormalRandHeadingVector(0, 2 * PI, 0), motion.WorldDelta.r[3]);
									unitData.Heading = DirectX::XMVector2Normalize(unitData.Heading);
									unitData.Heading = DirectX::XMVectorAdd(unitData.Heading, navigateHeading);
									unitData.Heading = DirectX::XMVector2Normalize(unitData.Heading);
									globalStringDraw.SimpleAppend("New Vector", 1, 1, 0,
										DirectX::XMVectorAdd(worldPosition.WorldPos.r[3], unitData.Heading)
										, 0.5, 60, StrDrawPos::AsCenter);
									unitData.Heading = DirectX::XMVectorScale(unitData.Heading, unitData.MoveTilePerTick / 2);
									unitData.NextPosReloadTick = Tick + Interface::UniformRandInt(100,120);
									if (DirectX::XMVectorIsNaN(unitData.Heading).m128_f32[0]) {
										throw("");
									}
								}
							}
							else {
								// �������ː��ɖ�肪����̂ł����ƌo�H�ɏ]���ċ߂Â��K�v������
								unitData.VectorNavigation = false;
								if (unitData.NextPosReloadTick < Tick) {
									unitData.NextPosReloadTick = Tick + Interface::UniformRandInt(100, 120);
								}
							}
						}
						if (unitData.NextPosReloadTick >= Tick) {
							globalStringDraw.SimpleAppend("Cool Time", 0, 1, 0, worldPosition.WorldPos.r[3], 0.5, 1, StrDrawPos::AsCenter);
						}


						float lengthToAreaBorder = DirectX::XMVector2Length(DirectX::XMVectorSubtract(unitData.AreaBorderTarget.Pos, worldPosition.WorldPos.r[3])).m128_f32[0];
						int current = pEntities->mRouting.GetAreaIndex(pEntities->mRouting.NearestWalkablePosition(worldPosition.WorldPos.r[3]));
						int previousArea = unitData.PreviousTickArea;
						if (DirectX::XMVector2Length(DirectX::XMVectorSubtract(unitData.PreviousTargetPos, unitData.FinalTarget.Pos)).m128_f32[0] > 3) {
							unitData.TargetUpdated = true;
							unitData.PreviousTargetPos = unitData.FinalTarget.Pos;
						}
						bool areaBorderShouldUpdate = !unitData.VectorNavigation && (lengthToAreaBorder < 1 || unitData.PreviousTickArea != current || unitData.TargetUpdated);
						unitData.PreviousTickArea = current;
						if (areaBorderShouldUpdate || Tick%30==0) {
							int target = max(0, pEntities->mRouting.GetAreaIndex(pEntities->mRouting.NearestWalkablePosition(unitData.FinalTarget.Pos)));
							std::string searchLog = std::to_string(current);
							int count = 0;
							// AreaBorderTarget�����߂�
							while (true) {
								//1�G���A���i�߂Ă����ĒʘH���ʂ�Ȃ��Ȃ��������A�ڕW�G���A�܂ŒʘH���ʂ��Ă���Ƃ킩�������ɒT�����~�߂�
								if (current == target) {
									if (pHurtboxes->IsWayClear(&unitData.FinalTarget.Pos, &worldPosition.WorldPos.r[3])) {
										// ���ݒn����ŏI�ڕW�܂Œ����ōs�����Ƃ��ł���


										// ���ݒn�_�ƖڕW�n�_�����񂾒����̎��ӁA
										// ���ڕW����w�肳�ꂽ�����O����J���Ă���A
										// ���݈ʒu���璼�i�ł���o�H������ꏊ�����̈ړ��ڕW�Ƃ���
										/*
										if (unitData.DistanceThresholdMax > 0.5) {
											DirectX::XMVECTOR gap = DirectX::XMVectorSubtract(worldPosition.WorldPos.r[3], unitData.FinalTarget.Pos);
											gap = DirectX::XMVectorScale(gap, Interface::UniformRandFloat(unitData.DistanceThresholdMin, unitData.DistanceThresholdMax) / DirectX::XMVector2Length(gap).m128_f32[0]);
											gap = DirectX::XMVector2Transform(gap, DirectX::XMMatrixRotationZ(Interface::UniformRandFloat(PI / -8.0f, PI / 8.0f)));
											unitData.AreaBorderTarget.Pos = DirectX::XMVectorAdd(unitData.FinalTarget.Pos, gap);
											if (pEntities->mRouting.GetAreaIndex(unitData.AreaBorderTarget.Pos)==-1) {
												unitData.AreaBorderTarget.Pos = pEntities->mRouting.NearestWalkablePosition(unitData.AreaBorderTarget.Pos);
											}
											if (!pHurtboxes->IsWayClear(&unitData.AreaBorderTarget.Pos, &worldPosition.WorldPos.r[3])) {
												gap = DirectX::XMVectorSubtract(worldPosition.WorldPos.r[3], unitData.FinalTarget.Pos);
												gap = DirectX::XMVectorScale(gap, min(1, Interface::UniformRandFloat(unitData.DistanceThresholdMin, unitData.DistanceThresholdMax) / DirectX::XMVector2Length(gap).m128_f32[0]));
												unitData.AreaBorderTarget.Pos = DirectX::XMVectorAdd(unitData.FinalTarget.Pos, gap);
												globalStringDraw.SimpleAppend("NotClear", 0, 1, 1, worldPosition.WorldPos.r[3], 0.5, 1, StrDrawPos::AsCenter);
											}
										}
										else {
											unitData.AreaBorderTarget = unitData.FinalTarget;
										}
										*/
										unitData.AreaBorderTarget = unitData.FinalTarget;
										unitData.HeadingFree = true;
										break;
									}
									else {
										// �o�H�̓r�����������ł����Ȃ����Ƃ���������
										// ��������蒼���ōs�����Ƃɂ���
										unitData.HeadingFree = false;
										if (count == 0) {
											// �ŏ��̌o�H���ӂ������Ă���ꍇ�͋����ɂł��ړ�����
											// �X�^�b�N�\�h
											// �����G���A���Ȃ̂Ō덷�̖��Ŏ��ۂ͒ʂ��Ƃ��l������
											unitData.AreaBorderTarget = unitData.FinalTarget;
										}
										break;
									}
								}
								Interface::WayPoint targetNotSureWayClear;
								targetNotSureWayClear = pEntities->mRouting.GetWayPoint(current, target, worldPosition.WorldPos.r[3]);
								if (pHurtboxes->IsWayClear(&targetNotSureWayClear.Pos, &worldPosition.WorldPos.r[3])) {

									// ���̃G���A���E�̃E�F�C�|�C���g�܂ň꒼���ōs����ꍇ
									unitData.AreaBorderTarget = targetNotSureWayClear;
									current = pEntities->mRouting.AllArea[current].WhereToGo[target];
									searchLog = searchLog + "->" + std::to_string(current);
								}
								else {
									// �o�H�̓r�����������ł����Ȃ����Ƃ���������
									// ��������蒼���ōs�����Ƃɂ���
									unitData.HeadingFree = false;
									if (count == 0) {
										// �ŏ��̌o�H���ӂ������Ă���ꍇ�͋����ɂł��ړ�����
										// �X�^�b�N�\�h
										// �����G���A���Ȃ̂Ō덷�̖��Ŏ��ۂ͒ʂ��Ƃ��l������
										unitData.AreaBorderTarget = targetNotSureWayClear;
									}
									break;
								}
								count++;
							}
							unitData.TargetUpdated = false;
						}
					}
					globalStringDraw.SimpleAppend("Border", 1, 0, 0, unitData.AreaBorderTarget.Pos, 0.5, 1, StrDrawPos::AsCenter);
					globalStringDraw.SimpleAppend("Final", 0, 0, 1, unitData.FinalTarget.Pos, 0.5, 1, StrDrawPos::AsCenter);
					// �ڕW�n�_�ւ̈ړ����s��
					if (unitData.VectorNavigation) {
						if (unitData.NextPosReloadTick - 90 > Tick) {
							motion.WorldDelta.r[3] = DirectX::XMVectorScale(motion.WorldDelta.r[3], 0.8f);
							motion.WorldDelta.r[3] = DirectX::XMVectorAdd(motion.WorldDelta.r[3], unitData.Heading);
							globalStringDraw.SimpleAppend("Vector Navigation", 1, 0, 0, worldPosition.WorldPos.r[3], 0.5, 1, StrDrawPos::AsCenter);
						}
						else {
							motion.WorldDelta.r[3] = DirectX::XMVectorScale(motion.WorldDelta.r[3], 0.8f);
						}
					}
					else
					{
						// ���ݒn�ƈꎞ�ڕW�̒��Ԓn�_�ɐi�������̋t���ւ̕␳���������n�_�֌�����
						DirectX::XMVECTOR vector = DirectX::XMVectorAdd(DirectX::XMVectorScale(
							unitData.AreaBorderTarget.Pos, 0.5), DirectX::XMVectorScale(
								worldPosition.WorldPos.r[3], -0.5));
						float slipRange = DirectX::XMVector2Length(motion.WorldDelta.r[3]).m128_f32[0] / (1.0f - 0.8f);
						float distance = DirectX::XMVector2Length(vector).m128_f32[0] * 2;
						motion.WorldDelta.r[3] = DirectX::XMVectorScale(motion.WorldDelta.r[3], 0.8f);
						// �҂�����~�߂邽�߂ɂ��傤�ǂ����Ƃ���łŐi�ނ̂��~�߂�
						if (distance > slipRange) {
							if (!unitData.HeadingFree) {
								// �i�������ɐ��񂪂���ꍇ
								// �Ȃ��ꍇ�͈꒼���Ɍ�����
								vector = DirectX::XMVectorAdd(vector, DirectX::XMVectorScale(unitData.AreaBorderTarget.Heading, distance * -0.2f));
							}
							float length = DirectX::XMVector2Length(vector).m128_f32[0];
							DirectX::XMVECTOR v = vector;
							vector = DirectX::XMVectorScale(vector, min(1, unitData.MoveTilePerTick / length));
							vector.m128_f32[2] = 0;
							vector.m128_f32[3] = 0;
							motion.WorldDelta.r[3] = DirectX::XMVectorAdd(motion.WorldDelta.r[3], vector);
							v = DirectX::XMVectorAdd(v, worldPosition.WorldPos.r[3]);
							v.m128_f32[2] = 0;
							globalStringDraw.SimpleAppend("Temp", 0, 1, 0, v, 0.5, 1, StrDrawPos::AsCenter);
						}
						else {
							globalStringDraw.SimpleAppend("Coasting", 0, 1, 0, worldPosition.WorldPos.r[3], 0.5, 1, StrDrawPos::AsCenter);
						}
					}
					// �U��
					if (Tick % 10 == 0) {
						unitData.Fire = false;
						DirectX::XMVECTOR target = {0,0,0,1};
						if (corpsData.CurrentAttackOrder == Interface::AttackOrder::Nearest) {
							auto nearest = pEntities->GetNearestHostilingUnit(worldPosition.WorldPos.r[3], unitData.Team);
							if (std::get<1>(nearest) < unitData.AttackRange) {
								target = pEntities->Registry.get<Component::WorldPosition>(
									std::get<0>(nearest)).WorldPos.r[3];
								unitData.Fire = true;
							}
						}
						if (unitData.Fire) {
							unitData.AttackAt = target;
						}
					}
					{
						DirectX::XMVECTOR target = DirectX::XMVectorSubtract(unitData.AttackAt, worldPosition.WorldPos.r[3]);
						float pointFor = atan2(target.m128_f32[1], target.m128_f32[0]);
						motion.Headfor(pointFor, &worldPosition.WorldPos, 0.1f);
					}
				}

				// ���S����
				if (damagePool.Damage.physical > unitData.MaxHP && unitData.Leader!=entity) {
					killFlag.KillOnThisTick = true;
					for (int i = 0; i < 7; i++) {
						entt::entity ball = unitData.BallIds[i];
						pEntities->Registry.get<Component::KillFlag>(ball).KillOnThisTick = true;
					}
				}
			}
		}
	};
	class BallAction : public System {
	public:
		BallAction(GameSystem* pGameSystem) : System(pGameSystem) {}
		void Update() override{
			for (auto [entity, worldPosition, ballData, attack] : pEntities->Registry.view<
				Component::WorldPosition,
				Component::BallData,
				Component::Attack>().each()) {
				Component::UnitData& unitData = pEntities->Registry.get<Component::UnitData>(ballData.CoreId);
				Component::WorldPosition& unitWorldPosition = pEntities->Registry.get<Component::WorldPosition>(ballData.CoreId);
				DirectX::XMVECTOR target = DirectX::XMVectorSubtract(unitData.AttackAt, worldPosition.WorldPos.r[3]);
				float pointFor = atan2(target.m128_f32[1], target.m128_f32[0]);
				if (unitData.Fire && attack.NextAbleTick < Tick && !ballData.OnCenter &&
					abs(pointFor - (Interface::RotationOfMatrix(&unitWorldPosition.WorldPos) + ballData.AngleFromCore)) < PI / 2) {
					for (int i = 0; i < attack.Count; i++) {
						float heading = pointFor;
						heading = heading + attack.headingFork * (i - (attack.Count - 1) / 2);
						heading = heading + Interface::UniformRandFloat(attack.headingError * -1, attack.headingError);
						Interface::EntityInitData initData;
						initData.Team = unitData.Team;
						initData.Prototype = attack.Bullet;
						initData.Pos = Interface::GetMatrix(&worldPosition.WorldPos.r[3],heading,1);
						initData.IsCore = true;
						pEntities->EmplaceFromPrototypeEntity<false>(&initData);
						globalStringDraw.SimpleAppend("fire", 1, 0, 0, worldPosition.WorldPos.r[3], 1, 5, StrDrawPos::AsCenter);
					}
					attack.NextAbleTick = Tick + attack.CoolTime;
				}
			}
		}
	};
	class DamageUpdate : public System {
	public:
		DamageUpdate(GameSystem* pGameSystem) : System(pGameSystem) {}
		void Update() override {
			for (auto [entity, damagePool,ballData] : pEntities->Registry.view<
				Component::DamagePool,
				Component::BallData>().each()) {
				// �X���b�v�_���[�W��K��������
				Component::DamagePool& coreDamagePool = pEntities->Registry.get<Component::DamagePool>(ballData.CoreId);
				coreDamagePool.AddDamage(&damagePool.Damage);
				damagePool.Damage = Interface::Damage();
			}
		}
	};
	//�^���Ɋւ���
	class UpdateWorld : public System {
	public:
		UpdateWorld(GameSystem* pGameSystem) : System(pGameSystem) {}
		void Update() override {
			for (auto [entity, motion, worldPosition, moveFlag, hitFlag] : pEntities->Registry.view<
				Component::Motion,
				Component::WorldPosition,
				Component::MoveFlag,
				Component::HitFlag>().each()) {
				worldPosition.Updated = false;
			}
			for(auto [entity,motion,worldPosition,moveFlag,hitFlag] : pEntities->Registry.view<
				Component::Motion,
				Component::WorldPosition,
				Component::MoveFlag,
				Component::HitFlag>().each()) {
				if (worldPosition.PositionalRoot) {
					moveFlag.Moved = true;
					DirectX::XMVECTOR parallel = DirectX::XMVectorAdd(worldPosition.WorldPos.r[3], motion.WorldDelta.r[3]);
					worldPosition.WorldPos = worldPosition.WorldPos * motion.WorldDelta;
					worldPosition.WorldPos.r[3] = parallel;
					worldPosition.WorldPos.r[3].m128_f32[3] = 1;
					if (worldPosition.WorldPos.r[3].m128_f32[3] > 1.9) {
						throw("");
					}
					worldPosition.Updated = true;
				}
			}
			for (auto [entity, motion, worldPosition, moveFlag, hitFlag, killFlag] : pEntities->Registry.view <
				Component::Motion,
				Component::WorldPosition,
				Component::MoveFlag,
				Component::HitFlag,
				Component::KillFlag> ().each()) {
				if (!worldPosition.PositionalRoot) {
					DirectX::XMMATRIX temp;
					if (!worldPosition.UpdateAndGet(&pEntities->Registry,&temp)) {
						killFlag.KillOnThisTick = true;
					}
					if (worldPosition.WorldPos.r[3].m128_f32[3] > 1.9) {
						throw("");
					}
					moveFlag.Moved = true;
				}
			}
		}
	};
	class CoreApplyLinearAcceralation : public System {
	public:
		CoreApplyLinearAcceralation(GameSystem* pGameSystem) : System(pGameSystem) {}
		void Update() override {
			for (auto [entity, motion, linearAcceralation,worldPosition] : pEntities->Registry.view < 
				Component::Motion, 
				Component::LinearAcceralation,
				Component::WorldPosition> ().each()) {
				if (worldPosition.PositionalRoot) {
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

			for (auto [entity, motion, worldPosition] : pEntities->Registry.view<
				Component::Motion,
				Component::WorldPosition>().each()) {
				worldPosition.Updated = false;
			}
			for (auto [entity, worldPosition, motion] : pEntities->Registry.view <
				Component::WorldPosition,
				Component::Motion > ().each()) {
				if (worldPosition.PositionalRoot) {
					DirectX::XMVECTOR parallel = DirectX::XMVectorAdd(worldPosition.WorldPos.r[3], motion.WorldDelta.r[3]);
					worldPosition.NextTickWorldPos = worldPosition.WorldPos * motion.WorldDelta;
					worldPosition.NextTickWorldPos.r[3] = parallel;
				}
			}
			for (auto [entity, motion, worldPosition] : pEntities->Registry.view <
				Component::Motion,
				Component::WorldPosition>().each()) {
				if (!worldPosition.PositionalRoot) {
					DirectX::XMMATRIX temp;
					worldPosition.UpdateAndGetNext(&pEntities->Registry, &temp);
				}
			}
		}
	};
	class Unrotate : public System {
		Unrotate(GameSystem* pGameSystem) : System(pGameSystem) {}
		void Update() override {

			for (auto [entity, worldPosition] : pEntities->Registry.view <
				Component::WorldPosition>().each()) {
				worldPosition.WorldPos = Interface::SetRotation(&worldPosition.WorldPos, 0);
			}
		}
	};
	class LineMotion : public System {
		LineMotion(GameSystem* pGameSystem) : System(pGameSystem) {}
		void Update() override {

			for (auto [entity, worldPosition, lineMotion, killFlag] : pEntities->Registry.view <
				Component::WorldPosition,
				Component::LineMotion,
				Component::KillFlag>().each()) {
				Component::WorldPosition* pLeft = pEntities->Registry.try_get<Component::WorldPosition>(lineMotion.Left);
				Component::WorldPosition* pRight = pEntities->Registry.try_get<Component::WorldPosition>(lineMotion.Right);
				if (pLeft == nullptr || pRight == nullptr) {
					// �[�ɂȂ�I�u�W�F�N�g����������폜�����
					killFlag.KillOnThisTick = true;
				}
				else {
					DirectX::XMVECTOR center = DirectX::XMVectorScale(
						DirectX::XMVectorAdd(pLeft->WorldPos.r[3], pRight->WorldPos.r[3]), 0.5f);
					DirectX::XMVECTOR xCoord =DirectX::XMVectorSubtract(pRight->WorldPos.r[3], pLeft->WorldPos.r[3]);
					float length = DirectX::XMVector2Length(xCoord).m128_f32[0];
					xCoord = DirectX::XMVector2Normalize(xCoord);
					DirectX::XMVECTOR yCoord = { xCoord.m128_f32[1],xCoord.m128_f32[0],0,0 };
					xCoord = DirectX::XMVectorScale(xCoord, length + 2 * lineMotion.Margin);
					yCoord = DirectX::XMVectorScale(yCoord, 2 * lineMotion.Width);
					worldPosition.WorldPos = {
						xCoord,
						yCoord,
						{0,0,1,0},
						center
					};
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
					pHurtboxes->SetWall((int)roundf(worldPosition.WorldPos.r[3].m128_f32[0]),
						(int)roundf(worldPosition.WorldPos.r[3].m128_f32[1]), entity);
				}
				if (killFlag.KillOnThisTick) {
					pHurtboxes->DeleteWall((int)roundf(worldPosition.WorldPos.r[3].m128_f32[0]),
						(int)roundf(worldPosition.WorldPos.r[3].m128_f32[1]));
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
					float radius = Interface::ScaleOfMatrix(&worldPosition.WorldPos) * 0.5f;
					int bottom = (int)roundf(worldPosition.WorldPos.r[3].m128_f32[1] - radius);
					int top = (int)roundf(worldPosition.WorldPos.r[3].m128_f32[1] + radius);
					int left = (int)roundf(worldPosition.WorldPos.r[3].m128_f32[0] - radius);
					int right = (int)roundf(worldPosition.WorldPos.r[3].m128_f32[0] + radius);
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
				{
					int OccupyingBottom = unitOccupationbox.OccupingRectBottom;
					int OccupyingTop = unitOccupationbox.OccupingRectTop;
					int OccupyingLeft = unitOccupationbox.OccupingRectLeft;
					int OccupyingRight = unitOccupationbox.OccupingRectRight;
					float radius = Interface::ScaleOfMatrix(&worldPosition.WorldPos) * 0.5f;
					int CurrentBottom = (int)roundf(worldPosition.WorldPos.r[3].m128_f32[1] - radius);
					int CurrentTop = (int)roundf(worldPosition.WorldPos.r[3].m128_f32[1] + radius);
					int CurrentLeft = (int)roundf(worldPosition.WorldPos.r[3].m128_f32[0] - radius);
					int CurrentRight = (int)roundf(worldPosition.WorldPos.r[3].m128_f32[0] + radius);
					//��L�G���A���ς�����Ȃ�
					if (OccupyingBottom != CurrentBottom ||
						OccupyingTop != CurrentTop ||
						OccupyingLeft != CurrentLeft ||
						OccupyingRight != CurrentRight) {
						for (int x = OccupyingLeft; x <= OccupyingRight; x++) {
							for (int y = OccupyingBottom; y <= OccupyingTop; y++) {
								pHurtboxes->DeleteUnit(x, y, entity, unitData.Team);
							}
						}
						for (int x = CurrentLeft; x <= CurrentRight; x++) {
							for (int y = CurrentBottom; y <= CurrentTop; y++) {
								pHurtboxes->SetUnit(x, y, entity, unitData.Team);
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
			
			for(auto [entity,motion,worldPosition,circleHitbox,giveDamage,bulletData,hitFlag,killFlag] : pEntities->Registry.view<
				Component::Motion,
				Component::WorldPosition,
				Component::CircleHitbox,
				Component::GiveDamage,
				Component::BulletData,
				Component::HitFlag,
				Component::KillFlag>().each()) {
				hitFlag.IsHit = false;
				hitFlag.IsHit = pHurtboxes->CheckCircleCollid(entity);
				if (hitFlag.IsHit) {
					killFlag.KillOnThisTick = true;
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
				unitOccupationbox.AlredayChecked = true;
				hitFlag.IsHit = false;
				//�Փ˂��Ă���ꍇ
				if (pHurtboxes->CheckUnitCollid(entity)) {
					hitFlag.IsHit = true;
				}
			}
		}
	};
	//�����ڂɊւ���
	class DrawBlockAppearance : public System {
	public:
		DrawBlockAppearance(GameSystem* pGameSystem) : System(pGameSystem) {}
		void Update() override {
			if (Tick % 60 == 0) {
				for (auto [entity, worldPosition, appearance, blockData, generateFlag, moveFlag, appearanceChanged, killFlag] : pEntities->Registry.view<
					Component::WorldPosition,
					Component::BlockAppearance,
					Component::BlockData,
					Component::GenerateFlag,
					Component::MoveFlag,
					Component::AppearanceChanged,
					Component::KillFlag>().each()) {
					if (blockData.OnTop) {
						// �e�̍X�V
						int x = (int)worldPosition.WorldPos.r[3].m128_f32[0];
						int y = (int)worldPosition.WorldPos.r[3].m128_f32[1];
						if (pEntities->ShouldUpdateShadow[y * WorldWidth + x]) {
							appearance.TexIndex.m128_f32[3] =
								appearance.TexHeadIndex.m128_f32[3] + pEntities->GetShadowIndex(x, y, true);
						}
						//�����ڂ̓o�^
						if (generateFlag.GeneratedOnThisTick) {
							Interface::BlockIType* pInstance = nullptr;
							appearance.BufferDataIndex =
								pWallAppearances->Add(entity, &pInstance);
							pInstance->Set(&worldPosition.WorldPos, &appearance.TexIndex);
						}
						//�����ڂ̍X�V
						if (moveFlag.Moved || appearanceChanged.Changed) {
							appearanceChanged.Changed = false;
							Interface::BlockIType* pInstance = nullptr;
							pWallAppearances->Update(appearance.BufferDataIndex, &pInstance);
							pInstance->Set(&worldPosition.WorldPos, &appearance.TexIndex);
						}
					}
					else {
						// �e�̍X�V
						int x = (int)worldPosition.WorldPos.r[3].m128_f32[0];
						int y = (int)worldPosition.WorldPos.r[3].m128_f32[1];
						if (pEntities->ShouldUpdateShadow[y * WorldWidth + x]) {
							appearance.TexIndex.m128_f32[3] =
								appearance.TexHeadIndex.m128_f32[3] + pEntities->GetShadowIndex(x, y, false);
						}
						//�����ڂ̓o�^
						if (generateFlag.GeneratedOnThisTick) {
							Interface::BlockIType* pInstance = nullptr;
							appearance.BufferDataIndex =
								pFloorAppearances->Add(entity, &pInstance);
							pInstance->Set(&worldPosition.WorldPos, &appearance.TexIndex);
						}
						//�����ڂ̍X�V
						if (moveFlag.Moved || appearanceChanged.Changed) {
							appearanceChanged.Changed = false;
							Interface::BlockIType* pInstance = nullptr;
							pFloorAppearances->Update(appearance.BufferDataIndex, &pInstance);
							pInstance->Set(&worldPosition.WorldPos, &appearance.TexIndex);
						}
					}
				}
				pEntities->ShouldUpdateShadow.reset();
			}
		}
	};
	class DrawConstant : public System {
	public:
		DrawConstant(GameSystem* pGameSystem) : System(pGameSystem) {}
		void Update() override {
			for (auto [entity, worldPosition, appearance, generateFlag, moveFlag, appearanceChanged, texture] : pEntities->Registry.view<
				Component::WorldPosition,
				Component::ConstantAppearance,
				Component::GenerateFlag,
				Component::MoveFlag,
				Component::AppearanceChanged,
				Component::Texture>().each()) {
				//�����ڂ̓o�^
				if (generateFlag.GeneratedOnThisTick) {
					Interface::GeneralIType* pInstance;
					appearance.BufferDataIndex = pConstantAppearances->Add(entity, &pInstance);
					Interface::fDebugOutput("new const index:%d\n", appearance.BufferDataIndex);
					pInstance->Set(&worldPosition.WorldPos, texture.TexOffset + appearance.TexHeadIndex);
				}
				//�����ڂ̍X�V
				if (moveFlag.Moved || appearanceChanged.Changed) {
					appearanceChanged.Changed = false;
					Interface::GeneralIType* pInstance;
					pConstantAppearances->Update(appearance.BufferDataIndex, &pInstance);
					pInstance->Set(&worldPosition.WorldPos, texture.TexOffset + appearance.TexHeadIndex);
				}
			}
		}
	};
	class DrawVariable : public System {
	public:
		DrawVariable(GameSystem* pGameSystem) : System(pGameSystem) {}
		void Update() override {
			for (auto [entity, worldPosition, appearance, generateFlag, moveFlag, appearanceChanged, texture] : pEntities->Registry.view<
				Component::WorldPosition,
				Component::VariableAppearance,
				Component::GenerateFlag,
				Component::MoveFlag,
				Component::AppearanceChanged,
				Component::Texture>().each()) {
				globalStringDraw.SimpleAppend(std::to_string(appearance.BufferDataIndex), 0, 1, 0, worldPosition.WorldPos.r[3], 0.3, 1, StrDrawPos::AsTopLeftCorner);
				//�����ڂ̓o�^
				if (generateFlag.GeneratedOnThisTick) {
					Interface::EffectIType* pInstance;
					appearance.BufferDataIndex = pVariableAppearances->Add(entity, &pInstance);
					pInstance->Set(&worldPosition.WorldPos, texture.TexOffset + appearance.TexHeadIndex, &appearance.Color1, &appearance.Color2);
				}
				//�����ڂ̍X�V
				if (moveFlag.Moved || appearanceChanged.Changed) {
					appearanceChanged.Changed = false;
					Interface::EffectIType* pInstance;
					pVariableAppearances->Update(appearance.BufferDataIndex, &pInstance);
					pInstance->Set(&worldPosition.WorldPos, texture.TexOffset + appearance.TexHeadIndex, &appearance.Color1, &appearance.Color2);
				}
			}
		}
	};
	class DrawShadow : public System {
	public:
		DrawShadow(GameSystem* pGameSystem) : System(pGameSystem) {}
		void Update() override {
			for (auto [entity, worldPosition, appearance, mainAppearance,generateFlag, moveFlag, texture] : pEntities->Registry.view<
				Component::WorldPosition,
				Component::ShadowAppearance,
				Component::VariableAppearance,
				Component::GenerateFlag,
				Component::MoveFlag,
				Component::Texture>().each()) {
				//�����ڂ̓o�^
				if (generateFlag.GeneratedOnThisTick) {
					Interface::GeneralIType* pInstance;
					appearance.BufferDataIndex = pVariableShadowAppearances->Add(entity, &pInstance);
					pInstance->Set(&worldPosition.WorldPos, texture.TexOffset);
				}
				//�����ڂ̍X�V
				if (moveFlag.Moved) {
					Interface::GeneralIType* pInstance;
					pVariableShadowAppearances->Update(appearance.BufferDataIndex, &pInstance);
					pInstance->Set(&worldPosition.WorldPos, texture.TexOffset);
				}
			}
			for (auto [entity, worldPosition, appearance, mainAppearance, generateFlag, moveFlag, texture] : pEntities->Registry.view<
				Component::WorldPosition,
				Component::ShadowAppearance,
				Component::ConstantAppearance,
				Component::GenerateFlag,
				Component::MoveFlag,
				Component::Texture>().each()) {
				//�����ڂ̓o�^FUpda
				if (generateFlag.GeneratedOnThisTick) {
					Interface::GeneralIType* pInstance;
					appearance.BufferDataIndex = pConstantShadowAppearances->Add(entity, &pInstance);
					pInstance->Set(&worldPosition.WorldPos, texture.TexOffset);
				}
				//�����ڂ̍X�V
				if (moveFlag.Moved) {
					Interface::GeneralIType* pInstance;
					pConstantShadowAppearances->Update(appearance.BufferDataIndex, &pInstance);
					pInstance->Set(&worldPosition.WorldPos, texture.TexOffset);
				}
			}
		}
	};
	/*
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
					Interface::BallIType* pInstance[3];

					ballAppearance.BufferDataIndex = pBallAppearance->Add(entity, pInstance);
					ballAppearance.CopyToBuffer(pInstance, worldPosition);

				}
				//�����ڂ̍X�V
				if (moveFlag.Moved || appearanceChanged.Changed) {
					appearanceChanged.Changed = false;


					Interface::BallIType* pInstance[3];

					pBallAppearance->Update(ballAppearance.BufferDataIndex, pInstance);
					ballAppearance.CopyToBuffer(pInstance, worldPosition);
				}
			}
		}
	};
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
					Interface::BulletIType* pInstance = nullptr;
					bulletAppearance.BufferDataIndex =
						pBulletAppearance->Add(entity, &pInstance);
					bulletAppearance.CopyToBuffer(pInstance, worldPosition);
					Interface::fDebugOutput("entity:%d,appId:%d Generated\n", entity, bulletAppearance.BufferDataIndex);
				}
				//�����ڂ̍X�V
				if (moveFlag.Moved || appearanceChanged.Changed) {
					appearanceChanged.Changed = false;
					Interface::BulletIType* pInstance = nullptr;
					pBulletAppearance->Update(bulletAppearance.BufferDataIndex, &pInstance);
					bulletAppearance.CopyToBuffer(pInstance, worldPosition);
				}
				globalStringDraw.SimpleAppend(std::to_string((int)entity), 1, 0.5, 0.5, worldPosition.WorldPos.r[3], 0.5, 1, StrDrawPos::AsTopLeftCorner);
				globalStringDraw.SimpleAppend(std::to_string((int)bulletAppearance.BufferDataIndex), 0.5, 1, 0.5, worldPosition.WorldPos.r[3], 0.5, 1, StrDrawPos::AsBottomLeftCorner);
			}
			Interface::fDebugOutput("bullet app count:%d\n", pBulletAppearance->InstanceCount);
		}
	};
	//�����ڂɊւ���
	class EffectAppearance : public System {
	public:
		EffectAppearance(GameSystem* pGameSystem) : System(pGameSystem) {}
		void Update() override {

			for (auto [entity, worldPosition, effectAppearance, generateFlag, moveFlag, appearanceChanged, killFlag] : pEntities->Registry.view<
				Component::WorldPosition,
				Component::EffectAppearance,
				Component::GenerateFlag,
				Component::MoveFlag,
				Component::AppearanceChanged,
				Component::KillFlag>().each()) {
				//�����ڂ̓o�^
				if (generateFlag.GeneratedOnThisTick) {
					Interface::EffectIType* pInstance = nullptr;
					effectAppearance.BufferDataIndex =
						pEffectAppearance->Add(entity, &pInstance);
					effectAppearance.CopyToBuffer(pInstance, worldPosition);
				}
				//�����ڂ̍X�V
				if (moveFlag.Moved || appearanceChanged.Changed) {
					appearanceChanged.Changed = false;
					Interface::EffectIType* pInstance = nullptr;
					pEffectAppearance->Update(effectAppearance.BufferDataIndex, &pInstance);
					effectAppearance.CopyToBuffer(pInstance, worldPosition);
				}
			}
		}
	};
	*/
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
					if (spawn.TimeGapSince <= 0 && 0 < spawn.CountLeft) {
						// �������̃X�|�[��
						Interface::UnitIndex leader = spawn.Leader;
						float Xpos = Interface::UniformRandInt((float)spawn.SpawnAreaLeft, (float)spawn.SpawnAreaRight);
						float Ypos = Interface::UniformRandInt((float)spawn.SpawnAreaBottom, (float)spawn.SpawnAreaTop);
						DirectX::XMMATRIX pos = DirectX::XMMatrixIdentity();
						pos.r[3] = { (float)Xpos,(float)Ypos,0.0f,1.0f };
						pos.r[3] = pEntities->mRouting.NearestWalkablePosition(pos.r[3]);
						spawn.CoreData.IsLeader = true;
						spawn.CoreData.LeaderId = pEntities->EmplaceUnitWithInitData(leader, &spawn.CoreData, &pos);
						// �����̃X�|�[��
						spawn.CoreData.IsLeader = false;
						Interface::UnitIndex member = spawn.Member;
						while (0 < spawn.CountLeft) {
							Xpos = Interface::UniformRandFloat((float)spawn.SpawnAreaLeft, (float)spawn.SpawnAreaRight - 1.0f);
							Ypos = Interface::UniformRandFloat((float)spawn.SpawnAreaBottom, (float)spawn.SpawnAreaTop - 1.0f);
							DirectX::XMMATRIX pos = DirectX::XMMatrixIdentity();
							pos.r[3] = { (float)Xpos,(float)Ypos,0.0f,1.0f };
							pos.r[3] = pEntities->mRouting.NearestWalkablePosition(pos.r[3]);
							pEntities->EmplaceUnitWithInitData(member, &spawn.CoreData, &pos);
							spawn.CountLeft--;
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
	std::vector<int> ProcessTime;
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
		SystemArray.push_back(new Systems::DamageUpdate(pGameSystem));
		SystemArray.push_back(new Systems::CorpsAction(pGameSystem));
		SystemArray.push_back(new Systems::UnitAction(pGameSystem));
		SystemArray.push_back(new Systems::CoreApplyLinearAcceralation(pGameSystem));

		//�G�t�F�N�g�̐���
		SystemArray.push_back(new Systems::BallAction(pGameSystem));
		SystemArray.push_back(new Systems::GenerateHitEffect(pGameSystem));
		SystemArray.push_back(new Systems::GenerateTrajectory(pGameSystem));

		// ����ȑO�̏����ł��̃e�B�b�N�̍폜�ƒǉ��̌���͂��ׂčs����K�v������

		// ������艺�ɍ폜�ƒǉ��ɂ�����鏈���͂���K�v������

		SystemArray.push_back(new Systems::ControlUnitCount(pGameSystem));
		SystemArray.push_back(new Systems::BindCoreBall(pGameSystem));
		SystemArray.push_back(new Systems::BindLeaderMember(pGameSystem));
		//�`��
		SystemArray.push_back(new Systems::DrawBlockAppearance(pGameSystem));
		SystemArray.push_back(new Systems::DrawConstant(pGameSystem));
		SystemArray.push_back(new Systems::DrawVariable(pGameSystem));
		SystemArray.push_back(new Systems::DrawShadow(pGameSystem));

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
		ProcessTime = std::vector<int>(SystemArray.size(), 0);
	}
	// BlockAppearance,CalcurateGeneratedWorld�݂����ȃG���e�B�e�B�����Ȍ�̏����͂قƂ�ǂȂ��z�����\���ԐH���Ă邩��entt��entity�̌����ɂ��Ȃ莞�Ԃ��������Ă�\��������
	// GeneratedFlag,KillFlag�͐��������Ƃ�/�������肵���Ƃ��ɃG���e�B�e�B�ɒǉ���������ǂ��̂���
	// Release�r���h�ɂ������������
	void Update() {
		int size = SystemArray.size();
		OutputDebugStringA("Tick Start\n");
		for (int i = 0; i < size; i++) {
			auto start = std::chrono::system_clock::now();
			SystemArray[i]->Update();
			auto end = std::chrono::system_clock::now();
			ProcessTime[i] = ProcessTime[i] + std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

			//Interface::fDebugOutput("Process:%d finished\n", i);
			for (auto [entity, worldPosition] : SystemArray[0]->pEntities->Registry.view<
				Component::WorldPosition>().each()) {
				if (worldPosition.NextTickWorldPos.r[2].m128_f32[2] > 1.9f && worldPosition.NextTickWorldPos.r[2].m128_f32[2] < 2.1f) {
					throw("");
				}
				if (worldPosition.WorldPos.r[3].m128_f32[0] < -1 || worldPosition.WorldPos.r[3].m128_f32[0] > 1000||
					worldPosition.WorldPos.r[3].m128_f32[1] < -1 || worldPosition.WorldPos.r[3].m128_f32[1] > 1000||
					worldPosition.WorldPos.r[3].m128_f32[2] < -10 || worldPosition.WorldPos.r[3].m128_f32[2] > 10) {
					throw("");
				}
				if (Interface::ScaleOfMatrix(&worldPosition.WorldPos) > 4) {
					throw("");
				}
			}
			for (auto [entity, appearance] : SystemArray[0]->pEntities->Registry.view<
				Component::ConstantAppearance>().each()) {
				//Interface::fDebugOutput("const index:%d\n", appearance.BufferDataIndex);
			}
			int count = 0;
			for (auto [entity, unitData] : SystemArray[0]->pEntities->Registry.view<
				Component::UnitData>().each()) {
				if (unitData.Team == 1) {
					count++;
				}
			}
			//Interface::fDebugOutput("team 1 count:%d\n", count);
		}
		OutputDebugStringA("Tick End\n");
		if (Tick % 60 == 0) {
			for (int i = 0; i < size; i++) {
				Interface::fDebugOutput("System:%d's Average process time(ms/tick):%f\n", i, ProcessTime[i] / (60.0f * 1000.0f));
				ProcessTime[i] = 0;
			}
		}
	}
};