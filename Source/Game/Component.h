#pragma once
#include "Resource.h"
#include "../../Interfaces.h"
#include "../../Source/DirectX/Texture.h"
#include "PathBindJsonValue.h"


//Init�ɓ����̂͊e�C���X�^���X�����������Ƃ��ɕύX����������
//�ǂݍ��ݎ��Ɋm�肵�Ă��邱�Ƃ͏����Ȃ��Ă悢

//�S�G���e�B�e�B�œǂݍ��݂����I�u�W�F�N�g��AllEntities::GetArchetypeFromCompNames�Őݒ肷��
// System�̍Ō�̏����ɏ������A�폜�p�̂��̂�p�ӂ���
// Init�Ɋւ��Ă͈������قȂ�̂ł��ꂼ��SameArchetype::Add�ɏ���(void*�Ƃ��g���Ή��Ƃ��Ȃ�񂩂ȁH�^�`�F�b�N������Ȃ��̂ł���܂�肽���Ȃ�����)
// ����͓����ɐ�����������������̂�
// �R���|�[�l���g�̃f�[�^�^�A�ƊȒP�ȊǗ����s���֐�
extern SameFormatTextureArray BlockTextureArray;
extern SameFormatTextureArray ConstantTextureArray;
extern SameFormatTextureArray VariableTextureArray;
namespace Component {
	struct KillFlag {
		bool KillOnThisTick;
		void Init(Interface::EntityInitData* pInitData) {
			KillOnThisTick = false;
		}
		KillFlag() {
			KillOnThisTick = false;
		}
	};
	struct GenerateFlag {
		bool GeneratedOnThisTick;
		void Init(Interface::EntityInitData* pInitData) {
		}
		GenerateFlag() {
			GeneratedOnThisTick = true;
		}
	};
	struct MoveFlag {
		//�������ꍇ�`��f�[�^�̍X�V���K�v�Ȃ̂�
		// �O�e�B�b�N�Ŏ��ۂɓ�����
		bool Moved;
		// ���̃e�B�b�N�I�����ɓ�������
		bool TryMove;
		void Init(Interface::EntityInitData* pInitData) {
		}
		MoveFlag() {
			Moved = true;
			TryMove = false;
		}
	};
	struct AppearanceChanged {
	public:
		bool Changed;
		void Init(Interface::EntityInitData* pInitData) {
		}
		AppearanceChanged() {
			Changed = false;
		}
	};
	struct HitFlag {
	public:
		bool IsHit;
		void Init(Interface::EntityInitData* pInitData) {
		}
		HitFlag() {
			IsHit = false;
		}
	};
	/*
	struct PositionReference {
	public:
		entt::entity ReferenceTo;
		bool IsCore;
		void Init(Interface::EntityInitData* pInitData) {
			IsCore = pInitData->IsCore;
			ReferenceTo = pInitData->CoreId;
		}
		PositionReference() {

		}
	};
	*/

	// A0*C0=AC   0
	// B1 D1 BC+E 1
	// �ƂȂ�̂ŕϊ����������W��������O���Ƃ���K�v������

	//�^���Ɋւ��鐫��
	//�`���Փ˔���ɂ��g�p
	struct WorldPosition {
	public:
		DirectX::XMMATRIX LocalReferenceToCore;
		DirectX::XMMATRIX WorldPos;
		DirectX::XMMATRIX NextTickWorldPos;
		bool Updated;
		bool PositionalRoot;
		bool FreeToRorate;
		entt::entity Parent;
		void Init(Interface::EntityInitData* pInitData) {
			if (pInitData->IsCore) {
				WorldPos = LocalReferenceToCore * pInitData->Pos;
				NextTickWorldPos = WorldPos;
				PositionalRoot = true;
				Parent = pInitData->thisEntities;
			}
			else {
				LocalReferenceToCore = pInitData->Pos * LocalReferenceToCore;
				WorldPos = DirectX::XMMatrixIdentity();
				NextTickWorldPos = WorldPos;
				PositionalRoot = false;
				Parent = pInitData->CoreId;
			}
			if (WorldPos.r[3].m128_f32[3] > 1.9) {
				throw("");
			}
		}
		WorldPosition(PathBindJsonValue fromLoad, std::map<std::string, Interface::UnitIndex>* pUnitNameHash, std::map<std::string, entt::entity>* pEntNameHash) {
			float width = fromLoad.tryGetAsFloat("width", 1);
			LocalReferenceToCore = DirectX::XMMATRIX{
				width,0.0f,0.0f,0.0f,
				0.0f,width,0.0f,0.0f,
				0.0f,0.0f,1.0f,0.0f,
				0.0f,0.0f,0.0f,1.0f,
			};
			LocalReferenceToCore.r[3] = fromLoad.tryGetAsVector("parallel", { 0,0,0,1 });
			if (WorldPos.r[3].m128_f32[3] > 1.9) {
				throw("");
			}
			Updated = false;
		}

		void Update(DirectX::XMMATRIX* pParentWorldPos) {
			if (FreeToRorate) {
				float rotate = Interface::RotationOfMatrix(&WorldPos);
				WorldPos = LocalReferenceToCore * (*pParentWorldPos);
				WorldPos = Interface::SetRotation(&WorldPos, rotate);
			}
			else {
				WorldPos = LocalReferenceToCore * (*pParentWorldPos);
			}
			if (WorldPos.r[3].m128_f32[3] > 1.9) {
				throw("");
			}
			Updated = true;
		}
		bool UpdateAndGet(entt::registry* pRegistry, DirectX::XMMATRIX* pWorldPos) {
			if (PositionalRoot || Updated) {
				*pWorldPos = WorldPos;
				return true;
			}
			else {
				if (pRegistry->valid(Parent)) {
					DirectX::XMMATRIX parentWorldPos;
					if (pRegistry->get<WorldPosition>(Parent).UpdateAndGet(pRegistry, &parentWorldPos)) {
						Update(&parentWorldPos);
						*pWorldPos = WorldPos;
						return true;
					}
					else {
						pRegistry->get<KillFlag>(Parent).KillOnThisTick = true;
						return false;
					}
				}
				else {
					return false;
				}
			}
		}

		void UpdateNext(DirectX::XMMATRIX* pParentNextTickWorldPos) {
			if (FreeToRorate) {
				float rotate = Interface::RotationOfMatrix(&NextTickWorldPos);
				NextTickWorldPos = LocalReferenceToCore * (*pParentNextTickWorldPos);
				NextTickWorldPos = Interface::SetRotation(&NextTickWorldPos, rotate);
			}
			else {
				NextTickWorldPos = LocalReferenceToCore * (*pParentNextTickWorldPos);
			}
			Updated = true;
		}
		bool UpdateAndGetNext(entt::registry* pRegistry, DirectX::XMMATRIX* pNextTickWorldPos) {
			if (PositionalRoot || Updated) {
				*pNextTickWorldPos = NextTickWorldPos;
				return true;
			}
			else {
				if (pRegistry->valid(Parent)) {
					DirectX::XMMATRIX parentNextTickWorldPos;
					if (pRegistry->get<WorldPosition>(Parent).UpdateAndGetNext(pRegistry, &parentNextTickWorldPos)) {
						UpdateNext(&parentNextTickWorldPos);
						*pNextTickWorldPos = NextTickWorldPos;
						return true;
					}
					else {
						pRegistry->get<KillFlag>(Parent).KillOnThisTick = true;
						return false;
					}
				}
				else {
					return false;
				}
			}
		}
	};
	struct LineMotion {
		entt::entity Left;
		entt::entity Right;
		float Width; // ���������̕�(���a)
		float Margin; // ���������łǂꂾ���͂ݏo����
		void Init(Interface::EntityInitData* pInitData) {

		}
		LineMotion(PathBindJsonValue fromLoad, std::map<std::string, Interface::UnitIndex>* pUnitNameHash, std::map<std::string, entt::entity>* pEntNameHash) {
			Width = fromLoad.get("width").asFloat();
			Margin = fromLoad.tryGetAsFloat("margin", 0);
		}
	};
 	// TODO �^���Ɋւ��Ă͐���������
	struct LinearAcceralation {
	public:
		DirectX::XMMATRIX World;
		void Init(Interface::EntityInitData* pInitData) {

		}
		LinearAcceralation(PathBindJsonValue fromLoad, std::map<std::string, Interface::UnitIndex>* pUnitNameHash, std::map<std::string, entt::entity>* pEntNameHash) {
			World = Interface::GetMatrix(fromLoad.get("parallel").asVector(),
				fromLoad.tryGetAsFloat("zoom",0),
				fromLoad.tryGetAsFloat("rotate",0)
			);
		}
	};
	struct Motion {
	public:
		DirectX::XMMATRIX WorldDelta;
		//�R�A�̉�]��g�k�͂��̃p�[�c�̑傫����X���ɉe����^���Ă���̂œ��l�ɕω��̕����A������ς��Ȃ��Ƃ��������Ȃ�
		void Init(Interface::EntityInitData* pInitData) {
			DirectX::XMMATRIX temp = pInitData->Pos;
			temp.r[3] = { 0,0,0,1 };
			WorldDelta.r[3] = DirectX::XMVector4Transform(WorldDelta.r[3], temp);
		}
		Motion(PathBindJsonValue fromLoad, std::map<std::string, Interface::UnitIndex>* pUnitNameHash, std::map<std::string, entt::entity>* pEntNameHash) {
			DirectX::XMVECTOR parallel = fromLoad.tryGetAsVector("parallel", { 0,0,0,0 });
			parallel.m128_f32[3] = 0;
			float rotate = fromLoad.tryGetAsFloat("rotate", 0);
			float scale = fromLoad.tryGetAsFloat("zoom", 1);
			WorldDelta = Interface::GetMatrix(&parallel, rotate, scale);
		}
		void Headfor(float target,DirectX::XMMATRIX* worldPos,float maxAngulerSpeed) {
			float headingDist = target - Interface::RotationOfMatrix(worldPos);
			if (headingDist > PI) {
				headingDist -= 2 * PI;
			}
			if (headingDist < -1 * PI) {
				headingDist += 2 * PI;
			}
			float rotate = 0;
			if (headingDist > 0) {//q
				rotate = min(maxAngulerSpeed, headingDist);
			}
			if (headingDist < 0) {//e
				rotate = max(headingDist, -1 * maxAngulerSpeed);
			}
			WorldDelta = Interface::SetRotation(&WorldDelta, rotate);
		}
	};
	//�����ڂɊւ��鐫��
	struct Texture {
		float TexOffset;
		void Init(Interface::EntityInitData* pInitData) {
		}
		Texture(PathBindJsonValue fromLoad, std::map<std::string, Interface::UnitIndex>* pUnitNameHash, std::map<std::string, entt::entity>* pEntNameHash) {
			TexOffset = 0;
		}
	};
	struct ConstantAppearance {
		Interface::RectAppId BufferDataIndex;
		float TexHeadIndex;
		void Init(Interface::EntityInitData* pInitData) {
		}
		ConstantAppearance(PathBindJsonValue fromLoad, std::map<std::string, Interface::UnitIndex>* pUnitNameHash, std::map<std::string, entt::entity>* pEntNameHash) {
			TexHeadIndex = (float)ConstantTextureArray.Append(fromLoad.get("texture"));
		}
		void SetInstanceId(Interface::RectAppId newId) {
			BufferDataIndex = newId;
		}

	};
	struct VariableAppearance {
		Interface::RectAppId BufferDataIndex;
		DirectX::XMVECTOR Color1;
		DirectX::XMVECTOR Color2;
		float TexHeadIndex;
		void Init(Interface::EntityInitData* pInitData) {
		}
		VariableAppearance(PathBindJsonValue fromLoad, std::map<std::string, Interface::UnitIndex>* pUnitNameHash, std::map<std::string, entt::entity>* pEntNameHash) {
			Color1 = fromLoad.get("color1").asVector();
			Color2 = fromLoad.get("color2").asVector();
			TexHeadIndex = (float)VariableTextureArray.Append(fromLoad.get("texture"));
		}
		void SetInstanceId(Interface::RectAppId newId) {
			BufferDataIndex = newId;
		}
	};
	struct ShadowAppearance {
		Interface::RectAppId BufferDataIndex;
		void Init(Interface::EntityInitData* pInitData) {
		}
		ShadowAppearance(PathBindJsonValue fromLoad, std::map<std::string, Interface::UnitIndex>* pUnitNameHash, std::map<std::string, entt::entity>* pEntNameHash) {
		}
		void SetInstanceId(Interface::RectAppId newId) {
			BufferDataIndex = newId;
		}
	};
	struct BlockAppearance {
	public:
		Interface::RectAppId BufferDataIndex;
		DirectX::XMVECTOR TexIndex;
		DirectX::XMVECTOR TexHeadIndex;
		//�u���b�N�ɐF�������
		DirectX::XMVECTOR Stain;
		void Init(Interface::EntityInitData* pInitData) {
		}
		BlockAppearance(PathBindJsonValue fromLoad, std::map<std::string, Interface::UnitIndex>* pUnitNameHash, std::map<std::string, entt::entity>* pEntNameHash) {
			TexHeadIndex = DirectX::XMVECTOR
			{
				(float)BlockTextureArray.Append(fromLoad.get("tex1")),
				(float)BlockTextureArray.Append(fromLoad.get("tex2")),
				(float)BlockTextureArray.Append(fromLoad.get("tex3")),
				(float)BlockTextureArray.Append(fromLoad.get("mask"))
			};
			TexIndex = TexHeadIndex;
			Stain = { 0,0,0,0 };
		}
		void CopyToBuffer(Interface::BlockIType* pDst,
			Component::WorldPosition& worldPosition) {
			pDst->Set(&worldPosition.WorldPos, &TexIndex);
		}
		void SetInstanceId(Interface::RectAppId newId) {
			BufferDataIndex = newId;
		}
	};
	/*

	struct BallAppearance {
	public:
		// ����(�����e)�A�{�̐F�E�A�A�͗l�E�n�C���C�g
		Interface::RectAppId BufferDataIndex;
		DirectX::XMVECTOR Color0[3];
		DirectX::XMVECTOR Color1[3];
		DirectX::XMVECTOR Color2[3];
		float TexIndex[3];
		float TexHeadIndex[3];
		void Init(Interface::EntityInitData* pInitData) {
			// 1��Base�̃C���f�b�N�X
			Color0[1] = pInitData->BaseColor0;
			Color1[1] = pInitData->BaseColor1;
		}
		void Load(PathBindJsonValue fromLoad,std::string layerName,int index) {
			TexHeadIndex[index] = (float)BallTextureArray.Append(fromLoad.get(layerName).get("texture"));
			TexIndex[index] = TexHeadIndex[index];
			Color0[index] = Interface::GetVectorFromJson(fromLoad.get(layerName).get("color0"));
			Color1[index] = Interface::GetVectorFromJson(fromLoad.get(layerName).get("color1"));
			Color2[index] = Interface::GetVectorFromJson(fromLoad.get(layerName).get("color2"));
		}
		BallAppearance(PathBindJsonValue fromLoad, std::map<std::string, Interface::UnitIndex>* pUnitNameHash, std::map<std::string, entt::entity>* pEntNameHash) {
			Load(fromLoad, "shadow", 0);
			Load(fromLoad, "base", 1);
			Load(fromLoad, "pattern", 2);
		}
		void CopyToBuffer(Interface::BallInstanceType* pDst[3],
			Component::WorldPosition& worldPosition) {
			//�e
			DirectX::XMMATRIX shadowWorld = worldPosition.WorldPos;
			pDst[0]->Set(&shadowWorld,
				TexIndex[0],
				&Color0[0],
				&Color1[0],
				&Color2[0]);


			//�{��
			DirectX::XMMATRIX baseWorld = worldPosition.WorldPos;
			pDst[1]->Set(&baseWorld,
				TexIndex[1],
				&Color0[1],
				&Color1[1],
				&Color2[1]);

			//�͗l
			//��ɂ�����̂����Ɍ�����̂̍Č�
			DirectX::XMMATRIX patternWorld = worldPosition.WorldPos;
			pDst[2]->Set(&patternWorld,
				TexIndex[2],
				&Color0[2],
				&Color1[2],
				&Color2[2]);
		}
		void SetInstanceId(Interface::RectAppId newId) {
			BufferDataIndex = newId;
		}
	};
	struct BulletAppearance {
	public:
		Interface::RectAppId BufferDataIndex;
		float TexIndex;
		float TexHeadIndex;
		void Init(Interface::EntityInitData* pInitData) {

		}
		BulletAppearance(PathBindJsonValue fromLoad, std::map<std::string, Interface::UnitIndex>* pUnitNameHash, std::map<std::string, entt::entity>* pEntNameHash) {
			TexHeadIndex = (float)BulletTextureArray.Append(fromLoad.get("texture"));
			TexIndex = TexHeadIndex;
		}
		void CopyToBuffer(Interface::BulletInstanceType* pDst,
			Component::WorldPosition& worldPosition) {
			pDst->Set(&worldPosition.WorldPos,TexIndex);
		}
		void SetInstanceId(Interface::RectAppId newId) {
			BufferDataIndex = newId;
		}
	};
	struct EffectAppearance {
	public:
		Interface::RectAppId BufferDataIndex;
		DirectX::XMVECTOR Color0;
		DirectX::XMVECTOR Color1;
		float TexIndex;
		float TexHeadIndex;
		void Init(Interface::EntityInitData* pInitData) {

		}
		EffectAppearance(PathBindJsonValue fromLoad, std::map<std::string, Interface::UnitIndex>* pUnitNameHash, std::map<std::string, entt::entity>* pEntNameHash) {
			TexHeadIndex = (float)BulletTextureArray.Append(fromLoad.get("texture"));
			TexIndex = TexHeadIndex;
			Color0 = Interface::GetVectorFromJson(fromLoad.get("baseColor0"));
			Color1 = Interface::GetVectorFromJson(fromLoad.get("baseColor1"));
		}
		void CopyToBuffer(Interface::EffectInstanceType* pDst,
			Component::WorldPosition& worldPosition) {
			pDst->Set(&worldPosition.WorldPos,
				TexIndex,
				&Color0,
				&Color1);
		}
		void SetInstanceId(Interface::RectAppId newId) {
			BufferDataIndex = newId;
		}
	};
	*/
	//�����蔻��Ɋւ��鐫��
	struct CircleHitbox {
	public:
		DirectX::XMVECTOR RelativeCenter;
		DirectX::XMVECTOR AbsoluteCenter;
		float Radius;
		// World�̔��a�ɑ΂��Ă̔��a�̔�
		float DiameterCoef;
		void Init(Interface::EntityInitData* pInitData) {

		}
		CircleHitbox(PathBindJsonValue fromLoad, std::map<std::string, Interface::UnitIndex>* pUnitNameHash, std::map<std::string, entt::entity>* pEntNameHash) {
			DiameterCoef = fromLoad.tryGetAsFloat("diameterCoef", 1);
			RelativeCenter = fromLoad.tryGetAsVector("relativeCenter",{0,0,0,1});
		}
	};
	// �e�{�[���ɕt�����铖���蔻��/��炢����Ɋւ��鐫��
	struct BallHurtbox {
	public:
		// World�̔��a�ɑ΂��Ă̔��a�̔�
		float DiameterCoef;
		void Init(Interface::EntityInitData* pInitData) {

		}
		BallHurtbox(PathBindJsonValue fromLoad, std::map<std::string, Interface::UnitIndex>* pUnitNameHash, std::map<std::string, entt::entity>* pEntNameHash) {
			DiameterCoef = fromLoad.get("diameterCoef").asFloat();
		}
	};
	// �R�A�ɕt�������L�Ɋւ��鐫��
	struct UnitOccupationbox {
	public:
		bool AlredayChecked;

		int OccupingRectTop;
		int OccupingRectBottom;
		int OccupingRectLeft;
		int OccupingRectRight;
		void Init(Interface::EntityInitData* pInitData) {

		}
		UnitOccupationbox(PathBindJsonValue fromLoad, std::map<std::string, Interface::UnitIndex>* pUnitNameHash, std::map<std::string, entt::entity>* pEntNameHash) {
			AlredayChecked = false;
		}
	};
	struct WallHurtbox {
	public:
		bool RefleshOccupation;
		void Init(Interface::EntityInitData* pInitData) {

		}
		WallHurtbox(PathBindJsonValue fromLoad, std::map<std::string, Interface::UnitIndex>* pUnitNameHash, std::map<std::string, entt::entity>* pEntNameHash) {
			RefleshOccupation = true;
		}
	};
	//�I�u�W�F�N�g�����̐���

	//�R�A�Ɋւ��鐫��
	//�o�t�̊Ǘ�
	struct CorpsData {
	public:

		// ���݂̕����̏�Ԃ̏��
		Interface::HostilityTeam Team;
		entt::entity LeaderUnit;
		std::vector<std::tuple<entt::entity, float>> HighDamageUnit;
		std::vector<entt::entity> AllMemberUnit;

		// �\�����郆�j�b�g�̏��

		// ���ݏo�Ă��閽�߂ɂ���
		Interface::MovementOrder CurrentMovementOrder;
		Interface::AttackOrder CurrentAttackOrder;
		Interface::Formation CurrentFormation;
		DirectX::XMVECTOR Position;
		float FormationWidth;
		float FormationThickness;
		float Heading;

		void Init(Interface::EntityInitData* pInitData) {
			Team = pInitData->Team;
			LeaderUnit = pInitData->thisEntities;
		}
		CorpsData(PathBindJsonValue fromLoad, std::map<std::string, Interface::UnitIndex>* pUnitNameHash, std::map<std::string, entt::entity>* pEntNameHash) {

		}
	};
	struct UnitData {
	public:
		Interface::HostilityTeam Team;
		entt::entity Leader;
		float MaxHP;


		// �ړ����x�n
		float RadianPerTick;
		float MoveTilePerTick;
		float Weight;
		float SpeedBuff;

		// �\������{�[���̏��
		entt::entity BallIds[7];
		int BallCount;

		// �s���̑I���ɂ��������
		float AttackRange;
		float AttackAzimath;


		// �I�����Ă���s���E�������^���ɔ��f������
		entt::entity TargetEntity;
		float DistanceThresholdMin;
		float DistanceThresholdMax;
		int PreviousTickArea;
		// ��
		Interface::WayPoint FinalTarget;
		DirectX::XMVECTOR PreviousTargetPos;
		bool TargetUpdated;
		// ��
		bool VectorNavigation;
		// �ʒu�U���Ɏg��
		bool HeadingFree;
		Interface::WayPoint AreaBorderTarget;
		// 
		DirectX::XMVECTOR Heading;
		int NextPosReloadTick;
		// Center�𒆐S�Ƃ���Length�͈͓̔��̏ꏊ�ֈړ�����
		// ��_�ʒu�A�����Œ�~����

		// �U���n�̖���
		DirectX::XMVECTOR AttackAt;
		bool Fire;


		void Init(Interface::EntityInitData* pInitData) {
			if (pInitData->IsLeader) {
				Leader = pInitData->thisEntities;
			}
			else {
				Leader = pInitData->LeaderId;
			}
			Team = pInitData->Team;
			Weight = pInitData->Weight;
			RadianPerTick = pInitData->RadianPerTick;
			MoveTilePerTick = pInitData->MoveTilePerTick;
			MoveTilePerTick = 0.04;
			SpeedBuff = pInitData->SpeedMultiply;
			BallCount = 0;
			DistanceThresholdMax = 0;
			DistanceThresholdMin = 0;
			FinalTarget.Pos = pInitData->Pos.r[3];
			AreaBorderTarget.Pos = pInitData->Pos.r[3];
			PreviousTickArea = -1;
			PreviousTargetPos = {0,0,0,1};
			Heading = { 0,0,0,0 };
			Fire = false;
			NextPosReloadTick = 0;

			MaxHP = MaxHP * pInitData->HealthMultiply;
		}
		UnitData(PathBindJsonValue fromLoad, std::map<std::string, Interface::UnitIndex>* pUnitNameHash, std::map<std::string, entt::entity>* pEntNameHash) {
			AttackRange = 20;
			MaxHP = 100;
		}
	};
	struct Attack {
	public:
		int CoolTime;
		int NextAbleTick;
		entt::entity Bullet;
		int Count;
		float headingError;// ���̊p�x���������ɕ����Ƃ��Č덷�Ƃ���
		float headingFork;// �������˂���Ƃ����̊p�x����������
		void Init(Interface::EntityInitData* pInitData) {

		}
		Attack(PathBindJsonValue fromLoad, std::map<std::string, Interface::UnitIndex>* pUnitNameHash, std::map<std::string, entt::entity>* pEntNameHash) {
			CoolTime = fromLoad.get("coolTime").asInt();
			NextAbleTick = 0;
			Bullet = (*pEntNameHash)[fromLoad.get("bullet").asString()];
			Count = fromLoad.get("count").asInt();
			headingError = fromLoad.get("headingError").asFloat();
			headingFork = fromLoad.get("headingFork").asFloat();
		}
	};
	struct BallData {
	public:
		entt::entity CoreId;
		float AngleFromCore;
		bool OnCenter;
		float Attack;
		float MovePower;
		float RotatePower;
		float Weight;

		void Init(Interface::EntityInitData* pInitData){
			AngleFromCore = atan2f(pInitData->Pos.r[3].m128_f32[1], pInitData->Pos.r[3].m128_f32[0]);
			OnCenter = pInitData->Pos.r[3].m128_f32[0] == 0 && pInitData->Pos.r[3].m128_f32[1] == 0;
			Attack = Attack * pInitData->DamageMultiply;
			CoreId = pInitData->CoreId;
		}
		BallData(PathBindJsonValue fromLoad, std::map<std::string, Interface::UnitIndex>* pUnitNameHash, std::map<std::string, entt::entity>* pEntNameHash) {
			Attack = fromLoad.get("attack").asFloat();
			MovePower = fromLoad.get("movePower").asFloat();
			RotatePower = fromLoad.get("rotatePower").asFloat();
			Weight = fromLoad.get("weight").asFloat();
		}
	};
	struct BulletData {
		bool InterfareToWall;
		bool InterfareToBall;
		Interface::HostilityTeam Team;
		void Init(Interface::EntityInitData* pInitData) {
			Team = pInitData->Team;
		}
		BulletData(PathBindJsonValue fromLoad, std::map<std::string, Interface::UnitIndex>* pUnitNameHash, std::map<std::string, entt::entity>* pEntNameHash) {
			InterfareToWall = fromLoad.tryGetAsBool("interfareToWall",true);
			InterfareToBall = fromLoad.tryGetAsBool("interfareToBall",true);
		}
	};
	struct BlockData {
	public:
		bool OnTop;
		bool Breakable;
		entt::entity DamageObserveEntity;
		void Init(Interface::EntityInitData* pInitData) {
			OnTop = pInitData->OnTop;
			Breakable = pInitData->Breakable;
			DamageObserveEntity = pInitData->DamageObserveEntity;
		}
		BlockData(PathBindJsonValue fromLoad, std::map<std::string, Interface::UnitIndex>* pUnitNameHash, std::map<std::string, entt::entity>* pEntNameHash) {

		}
	};
	struct GiveDamage {
	public:
		Interface::Damage Damage;
		void Init(Interface::EntityInitData* pInitData) {

		}
		GiveDamage(PathBindJsonValue fromLoad, std::map<std::string, Interface::UnitIndex>* pUnitNameHash, std::map<std::string, entt::entity>* pEntNameHash) {
			Damage = Interface::Damage(fromLoad);
		}
	};
	struct DamagePool {
	public:
		Interface::Damage Damage;
		void Init(Interface::EntityInitData* pInitData) {

		}
		DamagePool(PathBindJsonValue fromLoad, std::map<std::string, Interface::UnitIndex>* pUnitNameHash, std::map<std::string, entt::entity>* pEntNameHash) {
			Damage = Interface::Damage(fromLoad);
		}
		// �h��Ƃ��_���[�W�J�b�g���������ă_���[�W�����Z����
		void AddDamage(Interface::Damage* damage) {
			Damage += *damage;
		}
	};
	//���̃I�u�W�F�N�g�̐����ɂ�����鐫��
	struct HitEffect {
	public:
		entt::entity PrototypeEntity;
		float LeftRadian;// �����̕�
		float RightRadian;// �����̕�
		int EjectCount;// ������

		void Init(Interface::EntityInitData* pInitData) {

		}
		HitEffect(PathBindJsonValue fromLoad, std::map<std::string, Interface::UnitIndex>* pUnitNameHash, std::map<std::string, entt::entity>* pEntNameHash) {
			PrototypeEntity = (*pEntNameHash)[fromLoad.get("effectName").asString()];
			LeftRadian = fromLoad.get("leftRadian").asFloat();
			RightRadian = fromLoad.get("rightRadian").asFloat();
			EjectCount = fromLoad.get("count").asInt();
		}
	};
	struct Trajectory {
	public:
		entt::entity PrototypeEntity;
		int EjectStartTick;
		int EjectEndTick;
		int EjectInterval;
		float LeftRadian;// �����̕�
		float RightRadian;// �����̕�
		int EjectCount;
		void Init(Interface::EntityInitData* pInitData) {

		}
		Trajectory(PathBindJsonValue fromLoad, std::map<std::string, Interface::UnitIndex>* pUnitNameHash, std::map<std::string, entt::entity>* pEntNameHash) {
			PrototypeEntity = (*pEntNameHash)[fromLoad.get("effectName").asString()];
			LeftRadian = fromLoad.get("leftRadian").asFloat();
			RightRadian = fromLoad.get("rightRadian").asFloat();
			EjectCount = fromLoad.get("count").asInt();
			EjectStartTick = fromLoad.get("ejectStartTick").asInt();
			EjectEndTick = fromLoad.get("ejectEndTick").asInt();
			EjectInterval = fromLoad.get("ejectInterval").asInt();
		}
	};

	struct InvationObservance {
	public:
		int Left;
		int Right;
		int Bottom;
		int Top;
		void Init(Interface::EntityInitData* pInitData) {

		}
		InvationObservance(PathBindJsonValue fromLoad, std::map<std::string, Interface::UnitIndex>* pUnitNameHash, std::map<std::string, entt::entity>* pEntNameHash) {
			Left = fromLoad.get("observeArea").get("left").asInt();
			Right = fromLoad.get("observeArea").get("right").asInt();
			Bottom = fromLoad.get("observeArea").get("bottom").asInt();
			Top = fromLoad.get("observeArea").get("top").asInt();
		}
	};
	struct UnitCountObservance {
		int Border;
		Interface::HostilityTeam Team;
		void Init(Interface::EntityInitData* pInitData) {

		}
		UnitCountObservance(PathBindJsonValue fromLoad, std::map<std::string, Interface::UnitIndex>* pUnitNameHash, std::map<std::string, entt::entity>* pEntNameHash) {
			Border = fromLoad.get("borderCount").asInt();
			Team = fromLoad.get("team").asInt();
		}
	};
	struct Spawn {
		Interface::UnitIndex Leader;
		Interface::UnitIndex Member;
		Interface::EntityInitData CoreData;
		int CountLeft;
		int TimeGapSince;// �X�|�[���J�n�܂ł̎c��e�B�b�N
		bool SpawnFlag;
		int SpawnAreaLeft;
		int SpawnAreaRight;
		int SpawnAreaTop;
		int SpawnAreaBottom;
		void Init(Interface::EntityInitData* pInitData) {

		}
		Spawn(PathBindJsonValue fromLoad, std::map<std::string, Interface::UnitIndex>* pUnitNameHash, std::map<std::string, entt::entity>* pEntNameHash) {
			Leader = (*pUnitNameHash)[fromLoad.get("leaderUnit").asString()];
			Member = (*pUnitNameHash)[fromLoad.get("memberUnit").asString()];
			CoreData = Interface::EntityInitData(fromLoad.get("initData"));
			CountLeft = fromLoad.get("count").asInt();
			TimeGapSince = fromLoad.get("timeGap").asInt();
			SpawnFlag = false;
			SpawnAreaLeft = fromLoad.get("spawnArea").get("left").asInt();
			SpawnAreaRight = fromLoad.get("spawnArea").get("right").asInt();
			SpawnAreaTop = fromLoad.get("spawnArea").get("top").asInt();
			SpawnAreaBottom = fromLoad.get("spawnArea").get("bottom").asInt();
		}
	};

	struct Test1
	{
		Test1() {

		}
		void Init(Interface::EntityInitData* pInitData) {

		}
		Test1(PathBindJsonValue fromLoad, std::map<std::string, Interface::UnitIndex>* pUnitNameHash, std::map<std::string, entt::entity>* pEntNameHash) {

		}
	};
	struct Test2
	{
		Test2() {

		}
		void Init(Interface::EntityInitData* pInitData) {

		}
		Test2(PathBindJsonValue fromLoad, std::map<std::string, Interface::UnitIndex>* pUnitNameHash, std::map<std::string, entt::entity>* pEntNameHash) {

		}
	};
	struct Test3
	{
		int Num;
		Test3(int num) {
			Num = num;
		}
		void Init(Interface::EntityInitData* pInitData) {

		}
		Test3() {

		}
		Test3(PathBindJsonValue fromLoad, std::map<std::string, Interface::UnitIndex>* pUnitNameHash, std::map<std::string, entt::entity>* pEntNameHash) {

		}
	};
	struct TestResult {
		TestResult() {
			UpdatedByTestA = false;
			UpdatedByTestB = false;
			UpdatedByTestC = false;
			UpdatedByTestD = false;
		}
		bool UpdatedByTestA;
		bool UpdatedByTestB;
		bool UpdatedByTestC;
		bool UpdatedByTestD;
		void Init(Interface::EntityInitData* pInitData) {

		}
		TestResult(PathBindJsonValue fromLoad, std::map<std::string, Interface::UnitIndex>* pUnitNameHash, std::map<std::string, entt::entity>* pEntNameHash) {
			UpdatedByTestA = false;
			UpdatedByTestB = false;
			UpdatedByTestC = false;
			UpdatedByTestD = false;
		}
	};
};