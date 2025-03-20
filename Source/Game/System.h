#pragma once
#include "../../Interfaces.h"
#include "Appearance.h"
#include "Input.h"
#include "HitBox.h"
#include "Entity.h"
#include "StringDraw.h"
extern GraphicalStringDraw<65536, 2048, 32> globalStringDraw;
extern entt::entity Interface::PlayingUnit;
extern int Tick;
//関連するシステムは同じものに収める方が良いと考えられる
class GameSystem;
class System {
public:
	Hurtboxes* pHurtboxes;
	Entities* pEntities;
	Appearances<Interface::BlockDrawCallType, Interface::BlockInstanceType, 1, WorldWidth* WorldHeight>* pFloorAppearance;
	Appearances<Interface::BlockDrawCallType, Interface::BlockInstanceType, 1, WorldWidth* WorldHeight>* pWallAppearance;
	//影、本体、模様
	Appearances<Interface::BallDrawCallType, Interface::BallInstanceType, 1, MaxBallCount>* pBallAppearance;
	Appearances<Interface::BulletDrawCallType, Interface::BulletInstanceType, 1, MaxBulletCount>* pBulletAppearance;
	System(GameSystem* pGameSystem);
	virtual void Update() = 0;
};
namespace Systems {
	//追加削除
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
			int memberCount = corpsData.AllMemberUnit.size() + 1;// リーダー分1増やす
			float areaPerUnit = memberCount / (formationWidth * formationThickness);
			// 厚み方向に何人いるか
			int thicknessCount = std::ceil(formationThickness * std::sqrtf(areaPerUnit));
			int widthCount = std::ceil((float)memberCount / (float)thicknessCount);
			int leaderPos = min(memberCount - 1, (widthCount / 2) * thicknessCount + (thicknessCount / 2));
			// headingの方向を上にして左上→左下→右上の順で配置していく
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
			for (entt::entity member : corpsData.AllMemberUnit) {
				Component::UnitData& unitData = pEntities->Registry.get<Component::UnitData>(member);
				unitData.TargetEntity = corpsData.LeaderUnit;
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
				if (i == 0) {
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
				}
				if (i == 1) {
					FormLine(corpsData, {10,40,0,1},0,10,4);
				}
				if (i == 2) {
					ChaseLeader(corpsData, { 0,0,0,1 });
				}
				i++;
			}
		}
	};
	//意思決定
	class UnitAction : public System {
	public:
		UnitAction(GameSystem* pGameSystem) : System(pGameSystem) {}
		void Update() override {
			for (auto [entity, unitData, motion, worldPosition] : pEntities->Registry.view<
				Component::UnitData,
				Component::Motion,
				Component::WorldPosition>().each()) {
				globalStringDraw.SimpleAppend(std::to_string((int)entity), 0, 0, 0, worldPosition.WorldPos.Parallel, 1, 1, StrDrawPos::AsCenter);
				//プレイヤーの行動
				if (entity == Interface::PlayingUnit) {
					motion.WorldDelta.Ratio = 1;
					motion.WorldDelta.Rotate = 0;
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
					if (Input::KeyPushed.test(0x51)) {//q
						motion.WorldDelta.Rotate = 0.03f;
					}
					if (Input::KeyPushed.test(0x45)) {//e
						motion.WorldDelta.Rotate = -0.03f;
					}
					float length = DirectX::XMVector2Length(vector).m128_f32[0];
					vector = DirectX::XMVectorScale(vector, min(1, unitData.MoveTilePerTick / length));
					motion.WorldDelta.Parallel = DirectX::XMVectorScale(motion.WorldDelta.Parallel, 0.8);
					motion.WorldDelta.Parallel = DirectX::XMVectorAdd(motion.WorldDelta.Parallel, vector);
				}
				//AIの行動
				else {
					Component::CorpsData& corpsData = pEntities->Registry.get<Component::CorpsData>(unitData.Leader);
					if (corpsData.CurrentMovementOrder == Interface::MovementOrder::HoldPosition
						 || corpsData.CurrentMovementOrder == Interface::MovementOrder::Chase
						|| corpsData.CurrentMovementOrder == Interface::MovementOrder::Charge) {
						// Chargeモードで最も近い敵を探して更新する
						if (corpsData.CurrentMovementOrder == Interface::MovementOrder::Charge && Tick%10==0) {
							auto nearest = pEntities->GetNearestHostilingUnit(worldPosition.WorldPos.Parallel, unitData.Team);
							// 索敵範囲
							if (nearest.second < unitData.AttackRange * 2) {
								// 攻撃対象がいる場合
								unitData.TargetEntity = nearest.first;
								unitData.DistanceThresholdMax = unitData.AttackRange * 0.9;
								unitData.DistanceThresholdMin = unitData.AttackRange * 0.7;
							}
							else {
								// 攻撃対象がいない場合
								// リーダーに追従する
								unitData.TargetEntity = unitData.Leader;
								unitData.DistanceThresholdMax = 10;
								unitData.DistanceThresholdMin = 1;

							}
						}
						if ((corpsData.CurrentMovementOrder == Interface::MovementOrder::Charge || corpsData.CurrentMovementOrder == Interface::MovementOrder::Chase) && Tick % 5 == 0) {
							Component::WorldPosition* targetWorldPos = pEntities->Registry.try_get<Component::WorldPosition>(unitData.TargetEntity);
							if (targetWorldPos == nullptr) {
								// 標的が消失してる場合
								// いったんその場待機にして次の検索で新しい標的を探す
								unitData.FinalTarget.Pos = worldPosition.WorldPos.Parallel;
								unitData.DistanceThresholdMax = 0;
								unitData.DistanceThresholdMin = 0;
							}
							else {
								unitData.FinalTarget.Pos = targetWorldPos->WorldPos.Parallel;
							}
						}
						float length = DirectX::XMVector2Length(DirectX::XMVectorSubtract(unitData.AreaBorderTarget.Pos, worldPosition.WorldPos.Parallel)).m128_f32[0];
						int current = pEntities->mRouting.GetAreaIndex(worldPosition.WorldPos.Parallel);
						int previousArea = unitData.PreviousTickArea;
						if (DirectX::XMVector2Length(DirectX::XMVectorSubtract(unitData.PreviousTargetPos, unitData.FinalTarget.Pos)).m128_f32[0] > 3) {
							unitData.TargetUpdated = true;
							unitData.PreviousTargetPos = unitData.FinalTarget.Pos;
						}
						bool areaBorderShouldUpdate = length < 1 || unitData.PreviousTickArea != current || unitData.TargetUpdated;
						unitData.PreviousTickArea = current;
						if (areaBorderShouldUpdate || Tick%30==0) {
							int target = max(0, pEntities->mRouting.GetAreaIndex(pEntities->mRouting.NearestWalkablePosition(unitData.FinalTarget.Pos)));
							std::string searchLog = std::to_string(current);
							int count = 0;
							// AreaBorderTargetを決める
							while (true) {
								//1エリアずつ進めていって通路が通らなくなった時か、目標エリアまで通路が通っているとわかった時に探索を止める
								if (current == target) {
									if (pHurtboxes->IsWayClear(&unitData.FinalTarget.Pos, &worldPosition.WorldPos.Parallel)) {
										if (unitData.NextPosReloadTick > Tick && previousArea == target && !unitData.TargetUpdated) {
											// 計算負荷を減らすためにエリア間目標の再選定はクールタイムを設ける
											break;
										}
										// 現在地から最終目標まで直線で行くことができる


										// 現在地点と目標地点を結んだ直線の周辺、
										// かつ目標から指定された距離前後を開けており、
										// 現在位置から直進できる経路がある場所を次の移動目標とする
										if (unitData.DistanceThresholdMax > 0.5) {
											DirectX::XMVECTOR gap = DirectX::XMVectorSubtract(worldPosition.WorldPos.Parallel, unitData.FinalTarget.Pos);
											gap = DirectX::XMVectorScale(gap, Interface::UniformRandFloat(unitData.DistanceThresholdMin, unitData.DistanceThresholdMax) / DirectX::XMVector2Length(gap).m128_f32[0]);
											gap = DirectX::XMVector2Transform(gap, DirectX::XMMatrixRotationZ(Interface::UniformRandFloat(PI / -8.0f, PI / 8.0f)));
											unitData.AreaBorderTarget.Pos = DirectX::XMVectorAdd(unitData.FinalTarget.Pos, gap);
											if (pEntities->mRouting.GetAreaIndex(unitData.AreaBorderTarget.Pos)==-1) {
												unitData.AreaBorderTarget.Pos = pEntities->mRouting.NearestWalkablePosition(unitData.AreaBorderTarget.Pos);
											}
											if (!pHurtboxes->IsWayClear(&unitData.AreaBorderTarget.Pos, &worldPosition.WorldPos.Parallel)) {
												gap = DirectX::XMVectorSubtract(worldPosition.WorldPos.Parallel, unitData.FinalTarget.Pos);
												gap = DirectX::XMVectorScale(gap, min(1, Interface::UniformRandFloat(unitData.DistanceThresholdMin, unitData.DistanceThresholdMax) / DirectX::XMVector2Length(gap).m128_f32[0]));
												unitData.AreaBorderTarget.Pos = DirectX::XMVectorAdd(unitData.FinalTarget.Pos, gap);
												globalStringDraw.SimpleAppend("NotClear", 0, 1, 1, worldPosition.WorldPos.Parallel, 0.5, 1, StrDrawPos::AsCenter);
											}
										}
										else {
											unitData.AreaBorderTarget = unitData.FinalTarget;
										}
										unitData.HeadingFree = true;
										unitData.NextPosReloadTick = Tick + 120;
										break;
									}
									else {
										// 経路の途中だが直線でいけないことが判明した
										// いける限り直線で行くことにする
										unitData.HeadingFree = false;
										if (count == 0) {
											// 最初の経路がふさがっている場合は強引にでも移動する
											// スタック予防
											// 同じエリア内なので誤差の問題で実際は通れるとも考えられる
											unitData.AreaBorderTarget = unitData.FinalTarget;
										}
										break;
									}
								}
								Interface::WayPoint targetNotSureWayClear;
								targetNotSureWayClear = pEntities->mRouting.GetWayPoint(current, target, worldPosition.WorldPos.Parallel);
								if (pHurtboxes->IsWayClear(&targetNotSureWayClear.Pos, &worldPosition.WorldPos.Parallel)) {

									// そのエリア境界のウェイポイントまで一直線で行ける場合
									unitData.AreaBorderTarget = targetNotSureWayClear;
									current = pEntities->mRouting.AllArea[current].WhereToGo[target];
									searchLog = searchLog + "->" + std::to_string(current);
								}
								else {
									// 経路の途中だが直線でいけないことが判明した
									// いける限り直線で行くことにする
									unitData.HeadingFree = false;
									if (count == 0) {
										// 最初の経路がふさがっている場合は強引にでも移動する
										// スタック予防
										// 同じエリア内なので誤差の問題で実際は通れるとも考えられる
										unitData.AreaBorderTarget = targetNotSureWayClear;
									}
									break;
								}
								count++;
							}
							unitData.TargetUpdated = false;
						}
					}
					DirectX::XMVECTOR color = { 1,0,0,1 };
					globalStringDraw.Append("Border", &color, &unitData.AreaBorderTarget.Pos, 0.5, 1, StrDrawPos::AsCenter);
					color = { 0,0,1,1 };
					globalStringDraw.Append("Final", &color, &unitData.FinalTarget.Pos, 0.5, 1, StrDrawPos::AsCenter);
					// 目標地点への移動を行う
					{
						// 現在地と一時目標の中間地点に進入方向の逆側への補正をかけた地点へ向かう
						DirectX::XMVECTOR vector = DirectX::XMVectorAdd(DirectX::XMVectorScale(
							unitData.AreaBorderTarget.Pos, 0.5), DirectX::XMVectorScale(
								worldPosition.WorldPos.Parallel, -0.5));
						if (!unitData.HeadingFree) {
							// 進入方向に制約がある場合
							// ない場合は一直線に向かう
							float distance = DirectX::XMVector2Length(vector).m128_f32[0] * 2;
							vector = DirectX::XMVectorAdd(vector, DirectX::XMVectorScale(unitData.AreaBorderTarget.Heading, distance * -0.2));
						}
						float length = DirectX::XMVector2Length(vector).m128_f32[0];
						DirectX::XMVECTOR v = vector;
						vector = DirectX::XMVectorScale(vector, min(1, unitData.MoveTilePerTick / length));
						vector.m128_f32[2] = 0;
						vector.m128_f32[3] = 0;
						motion.WorldDelta.Parallel = DirectX::XMVectorScale(motion.WorldDelta.Parallel, 0.8);
						motion.WorldDelta.Parallel = DirectX::XMVectorAdd(motion.WorldDelta.Parallel, vector);
						color = { 0,1,0,1 };
						v = DirectX::XMVectorAdd(v, worldPosition.WorldPos.Parallel);
						v.m128_f32[2] = 0;
						globalStringDraw.Append("Temp", &color, &v, 0.5, 1, StrDrawPos::AsCenter);
					}
				}
			}
		}
	};
	//運動に関する
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
						worldPosition.NeedUpdate = true;// とりあえずtrueにしてあとでfalseにしていく
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
	//衝突に関する
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
					//占有エリアが変わったなら
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
	//次ティックでの衝突を衝突とみなし、移動の阻害と衝突の判定を同時に行う。
	//GiveDamageコンポーネントの存在がダメージ付与することを、
	//貫通するのか、移動を阻害されるのかのフラグによって衝突後にどういった運動につながるかを決める。
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
					//判定は1回、壁に衝突するかはballData参照、キャラクターとの衝突もballData参照、味方とは衝突しない
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
				//そもそも動こうとしている場合
				if (worldPosition.NeedUpdate) {
					//衝突している場合
					if (pHurtboxes->CheckUnitCollid(entity)) {
						hitFlag.IsHit = true;
					}
				}
			}
		}
	};
	//見た目に関する
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
					// 影の更新
					int x = (int)worldPosition.WorldPos.Parallel.m128_f32[0];
					int y = (int)worldPosition.WorldPos.Parallel.m128_f32[1];
					if (pEntities->ShouldUpdateShadow[y * WorldWidth + x]) {
						blockAppearance.TexCoord3M.m128_f32[2] = pEntities->GetWallShadow(x, y);
					}
					//見た目の登録
					if (generateFlag.GeneratedOnThisTick) {
						Interface::BlockInstanceType* pInstance = nullptr;
						blockAppearance.BufferDataIndex =
							pWallAppearance->Add(entity, &pInstance);
						pInstance->Set
						(&worldPosition.WorldMatrix,
							&blockAppearance.TexCoord12, &blockAppearance.TexCoord3M);
					}
					//見た目の更新
					if (moveFlag.Moved || appearanceChanged.Changed) {
						appearanceChanged.Changed = false;
						Interface::BlockInstanceType* pInstance = nullptr;
						pWallAppearance->Update(blockAppearance.BufferDataIndex, &pInstance);
						pInstance->Set(&worldPosition.WorldMatrix,
							&blockAppearance.TexCoord12, &blockAppearance.TexCoord3M);
					}
					//見た目の削除
					if (killFlag.KillOnThisTick) {
						pEntities->DeleteWall(x, y);
						//移動させたSameArchIndexを受け取る
						entt::entity replaced = pWallAppearance->Delete(
							blockAppearance.BufferDataIndex);
						//移動させたRectAppIdを変更する
						Component::BlockAppearance* pReplace = pEntities->Registry.try_get<Component::BlockAppearance>(replaced);
						if (pReplace != nullptr) {
							pReplace->BufferDataIndex = blockAppearance.BufferDataIndex;
						}
					}
				}
				else {
					// 影の更新
					int x = (int)worldPosition.WorldPos.Parallel.m128_f32[0];
					int y = (int)worldPosition.WorldPos.Parallel.m128_f32[1];
					if (pEntities->ShouldUpdateShadow[y * WorldWidth + x]) {
						blockAppearance.TexCoord3M.m128_f32[2] = pEntities->GetFloorShadow(x, y);
					}
					//見た目の登録
					if (generateFlag.GeneratedOnThisTick) {
						Interface::BlockInstanceType* pInstance = nullptr;
						blockAppearance.BufferDataIndex =
							pFloorAppearance->Add(entity, &pInstance);
						pInstance->Set
						(&worldPosition.WorldMatrix,
							&blockAppearance.TexCoord12, &blockAppearance.TexCoord3M);
					}
					//見た目の更新
					if (moveFlag.Moved || appearanceChanged.Changed) {
						appearanceChanged.Changed = false;
						Interface::BlockInstanceType* pInstance = nullptr;
						pFloorAppearance->Update(blockAppearance.BufferDataIndex, &pInstance);
						pInstance->Set(&worldPosition.WorldMatrix,
							&blockAppearance.TexCoord12, &blockAppearance.TexCoord3M);
					}
					//見た目の削除
					if (killFlag.KillOnThisTick) {
						//移動させたSameArchIndexを受け取る
						entt::entity replaced = pFloorAppearance->Delete(
							blockAppearance.BufferDataIndex);
						//移動させたRectAppIdを変更する
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
	//見た目に関する
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
				//見た目の登録
				if (generateFlag.GeneratedOnThisTick) {
					Interface::BallInstanceType* pInstance = nullptr;

					//影
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


					//本体
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

					//模様
					//上にあるものが奥に見えるのの再現
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
				//見た目の更新
				if (moveFlag.Moved || appearanceChanged.Changed) {
					appearanceChanged.Changed = false;
					Interface::BallInstanceType* pInstance = nullptr;


					//影
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
				//見た目の削除
				if (killFlag.KillOnThisTick) {
					//移動させたSameArchIndexを受け取る
					entt::entity replaced = pBallAppearance->Delete(
						ballAppearance.BufferDataIndex);
					//移動させたRectAppIdを変更する
					Component::BallAppearance* pReplace = pEntities->Registry.try_get<Component::BallAppearance>(replaced);
					if (pReplace != nullptr) {
						pReplace->BufferDataIndex = ballAppearance.BufferDataIndex;
					}
				}
			}
		}
	};
	//見た目に関する
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
				//見た目の登録
				if (generateFlag.GeneratedOnThisTick) {
					Interface::BulletInstanceType* pInstance = nullptr;
					bulletAppearance.BufferDataIndex =
						pBulletAppearance->Add(entity, &pInstance);
					pInstance->Set(&worldPosition.WorldMatrix,
						&bulletAppearance.TexCoord,
						&bulletAppearance.Color0,
						&bulletAppearance.Color0);
				}
				//見た目の更新
				if (moveFlag.Moved || appearanceChanged.Changed) {
					appearanceChanged.Changed = false;
					Interface::BulletInstanceType* pInstance = nullptr;
					pBulletAppearance->Update(bulletAppearance.BufferDataIndex, &pInstance);
					pInstance->Set(&worldPosition.WorldMatrix,
						&bulletAppearance.TexCoord,
						&bulletAppearance.Color0,
						&bulletAppearance.Color0);
				}
				//見た目の削除
				if (killFlag.KillOnThisTick) {
					//移動させたSameArchIndexを受け取る
					entt::entity replaced = pBulletAppearance->Delete(
						bulletAppearance.BufferDataIndex);
					//移動させたRectAppIdを変更する
					Component::BulletAppearance* pReplace = pEntities->Registry.try_get<Component::BulletAppearance>(replaced);
					if (pReplace != nullptr) {
						pReplace->BufferDataIndex = bulletAppearance.BufferDataIndex;
					}
				}
			}
		}
	};
	//オブジェクトの生成に関する
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
	// ミッション進行に関する
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
				// まだフラグが立ってない(まだスポーンが行われてない)かつボーダーを超えた場合
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

					//遅れ分を消化済み
					if (spawn.TimeGapSince <= 0 && 0 < spawn.CountLeft) {
						// 部隊長のスポーン
						Interface::UnitIndex leader = spawn.Leader;
						float Xpos = Interface::UniformRandInt(spawn.SpawnAreaLeft, spawn.SpawnAreaRight);
						float Ypos = Interface::UniformRandInt(spawn.SpawnAreaBottom, spawn.SpawnAreaTop);
						Interface::RelationOfCoord pos = Interface::RelationOfCoord();
						pos.Parallel = {
							(float)Xpos,(float)Ypos,0,1
						};
						pos.Parallel = pEntities->mRouting.NearestWalkablePosition(pos.Parallel);
						spawn.CoreData.IsLeader = true;
						spawn.CoreData.LeaderId = pEntities->EmplaceUnitWithInitData(leader, &spawn.CoreData, &pos);
						// 部下のスポーン
						spawn.CoreData.IsLeader = false;
						Interface::UnitIndex member = spawn.Member;
						while (0 < spawn.CountLeft) {
							Xpos = Interface::UniformRandFloat(spawn.SpawnAreaLeft, spawn.SpawnAreaRight - 1);
							Ypos = Interface::UniformRandFloat(spawn.SpawnAreaBottom, spawn.SpawnAreaTop - 1);
							Interface::RelationOfCoord pos = Interface::RelationOfCoord();
							pos.Parallel = {
								(float)Xpos,(float)Ypos,0,1
							};
							pos.Parallel = pEntities->mRouting.NearestWalkablePosition(pos.Parallel);
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
		//位置の更新

		SystemArray.push_back(new Systems::UpdateWorld(pGameSystem));

		//ミッション進行
		SystemArray.push_back(new Systems::InvationObserve(pGameSystem));
		SystemArray.push_back(new Systems::UnitCountObserve(pGameSystem));
		SystemArray.push_back(new Systems::Spawn(pGameSystem));
		//運動、意思決定
		SystemArray.push_back(new Systems::CorpsAction(pGameSystem));
		SystemArray.push_back(new Systems::UnitAction(pGameSystem));
		SystemArray.push_back(new Systems::CoreApplyLinearAcceralation(pGameSystem));

		//エフェクトの生成
		SystemArray.push_back(new Systems::GenerateHitEffect(pGameSystem));
		SystemArray.push_back(new Systems::GenerateTrajectory(pGameSystem));

		// これ以前の処理でこのティックの削除と追加の決定はすべて行われる必要がある

		// ここより下に削除と追加にかかわる処理はする必要がある

		SystemArray.push_back(new Systems::ControlUnitCount(pGameSystem));
		SystemArray.push_back(new Systems::BindCoreBall(pGameSystem));
		SystemArray.push_back(new Systems::BindLeaderMember(pGameSystem));

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
			SystemArray[i]->Update();
		}
		OutputDebugStringA("Tick End\n");
	}
};