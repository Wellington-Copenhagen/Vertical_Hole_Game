#pragma once
#include "../../Interfaces.h"
#include "../../Source/DirectX/Texture.h"


//Init�ɓ����̂͊e�C���X�^���X�����������Ƃ��ɕύX����������
//�ǂݍ��ݎ��Ɋm�肵�Ă��邱�Ƃ͏����Ȃ��Ă悢

//�S�G���e�B�e�B�œǂݍ��݂����I�u�W�F�N�g��AllEntities::GetArchetypeFromCompNames�Őݒ肷��
// System�̍Ō�̏����ɏ������A�폜�p�̂��̂�p�ӂ���
// Init�Ɋւ��Ă͈������قȂ�̂ł��ꂼ��SameArchetype::Add�ɏ���(void*�Ƃ��g���Ή��Ƃ��Ȃ�񂩂ȁH�^�`�F�b�N������Ȃ��̂ł���܂�肽���Ȃ�����)
// ����͓����ɐ�����������������̂�
// �R���|�[�l���g�̃f�[�^�^�A�ƊȒP�ȊǗ����s���֐�
extern SameFormatTextureArray BlockTextureArray;
extern SameFormatTextureArray BallTextureArray;
extern SameFormatTextureArray BulletTextureArray;
extern SameFormatTextureArray LineTextureArray;
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

	// A0*C0=AC   0
	// B1 D1 BC+E 1
	// �ƂȂ�̂ŕϊ����������W��������O���Ƃ���K�v������

	//�^���Ɋւ��鐫��
	//�`���Փ˔���ɂ��g�p
	struct WorldPosition {
		// �����g�k�Az���𒆐S�Ƃ�����]�Ax,y,z�����̕��s�ړ����L�^�ł���
		// 1,1,3�̕ϐ��ŕ\���ł���̂ł��������s����g���������̕����悳�����ȋC������
		// �{�������߂�ɂ�2x2�������o���čs�񎮂ɒʂ��Ƃ��K�v����
		// ����ŋ��ʂ̃C���^�[�t�F�C�X�Ƃ��čs����~����
		// �󋵎���ł͊g�k�ƕ��s�ړ��̉e���������󂯂����W���~�����ꍇ�����邵
		// DirectX::XMMATRIX1��64kB��256*256*3�p�ӂ����12MB�ƂȂ�
	public:
		DirectX::XMMATRIX WorldMatrix;
		Interface::RelationOfCoord LocalReferenceToCore;
		Interface::RelationOfCoord WorldPos;
		Interface::RelationOfCoord NextTickWorldPos;
		bool NeedUpdate;
		void Init(Interface::EntityInitData* pInitData) {
			if (pInitData->IsCore) {
				WorldPos = LocalReferenceToCore * pInitData->Pos;
				WorldMatrix = WorldPos.GetMatrix();
			}
			else {
				LocalReferenceToCore = pInitData->Pos;
			}
		}
		WorldPosition(Json::Value fromLoad) {
			LocalReferenceToCore.Parallel = Interface::GetVectorFromJson(fromLoad.get("parallel", ""));
			LocalReferenceToCore.Ratio = fromLoad.get("width", "").asFloat();
			LocalReferenceToCore.Rotate = 0;
			NeedUpdate = true;
		}
	};
 	// TODO �^���Ɋւ��Ă͐���������
	struct LinearAcceralation {
	public:
		Interface::RelationOfCoord World;
		void Init(Interface::EntityInitData* pInitData) {

		}
		LinearAcceralation(Json::Value fromLoad) {
			World.Parallel = Interface::GetVectorFromJson(fromLoad.get("parallel", ""));
			World.Ratio = fromLoad.get("zoom", "").asFloat();
			World.Rotate = fromLoad.get("rotate", "").asFloat();
		}
	};
	struct Motion {
	public:
		Interface::RelationOfCoord WorldDelta;
		//�R�A�̉�]��g�k�͂��̃p�[�c�̑傫����X���ɉe����^���Ă���̂œ��l�ɕω��̕����A������ς��Ȃ��Ƃ��������Ȃ�
		void Init(Interface::EntityInitData* pInitData) {
			WorldDelta.Parallel = DirectX::XMVector4Transform(
					DirectX::XMVectorScale(WorldDelta.Parallel, pInitData->Pos.Ratio),
					DirectX::XMMatrixRotationZ(pInitData->Pos.Rotate));
		}
		Motion(Json::Value fromLoad) {
			if (fromLoad.isMember("parallel")) {
				WorldDelta.Parallel = Interface::GetVectorFromJson(fromLoad.get("parallel", ""));
			}
			else {
				WorldDelta.Parallel = { 0.0f,0.0f, 0.0f, 0.0f };
			}
			if (fromLoad.isMember("rotate")) {
				WorldDelta.Ratio = fromLoad.get("zoom", "").asFloat();
			}
			else {
				WorldDelta.Ratio = 1.0f;
			}
			if (fromLoad.isMember("ratate")) {
				WorldDelta.Rotate = fromLoad.get("rotate", "").asFloat();
			}
			else {
				WorldDelta.Rotate = 0.0f;
			}
		}
	};
	//�����ڂɊւ��鐫��
	struct BlockAppearance {
	public:
		Interface::RectAppId BufferDataIndex;
		DirectX::XMVECTOR TexIndex;
		DirectX::XMVECTOR TexHeadIndex;
		//�u���b�N�ɐF�������
		DirectX::XMVECTOR Stain;
		void Init(Interface::EntityInitData* pInitData) {
		}
		BlockAppearance(Json::Value fromLoad) {
			TexHeadIndex = DirectX::XMVECTOR
			{
				(float)BlockTextureArray.Append(fromLoad.get("tex1", "")),
				(float)BlockTextureArray.Append(fromLoad.get("tex2", "")),
				(float)BlockTextureArray.Append(fromLoad.get("tex3", "")),
				(float)BlockTextureArray.Append(fromLoad.get("mask", ""))
			};
			TexIndex = TexHeadIndex;
			Stain = { 0,0,0,0 };
		}
		void CopyToBuffer(Interface::BlockInstanceType* pDst,
			Component::WorldPosition& worldPosition) {
			pDst->Set(&worldPosition.WorldMatrix, &TexIndex);
		}
		void SetInstanceId(Interface::RectAppId newId) {
			BufferDataIndex = newId;
		}
	};
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
		void Load(Json::Value fromLoad,std::string layerName,int index) {
			TexHeadIndex[index] = (float)BallTextureArray.Append(fromLoad.get(layerName, "").get("texture", ""));
			TexIndex[index] = TexHeadIndex[index];
			Color0[index] = Interface::GetVectorFromJson(fromLoad.get(layerName, "").get("color0", ""));
			Color1[index] = Interface::GetVectorFromJson(fromLoad.get(layerName, "").get("color1", ""));
			Color2[index] = Interface::GetVectorFromJson(fromLoad.get(layerName, "").get("color2", ""));
		}
		BallAppearance(Json::Value fromLoad) {
			Load(fromLoad, "shadow", 0);
			Load(fromLoad, "base", 1);
			Load(fromLoad, "pattern", 2);
		}
		void CopyToBuffer(Interface::BallInstanceType* pDst[3],
			Component::WorldPosition& worldPosition) {
			//�e
			DirectX::XMMATRIX shadowWorld{
				{worldPosition.WorldPos.Ratio, 0.0f, 0.0f, 0.0f},
				{0.0f, worldPosition.WorldPos.Ratio, 0.0f, 0.0f},
				{0.0f, 0.0f, 0.0f, 0.0f},
				DirectX::XMVectorAdd({0.0f, worldPosition.WorldPos.Ratio * -0.1f, 0.0f, 0.0f},worldPosition.WorldPos.Parallel)
			};
			pDst[0]->Set(&shadowWorld,
				TexIndex[0],
				&Color0[0],
				&Color1[0],
				&Color2[0]);


			//�{��
			DirectX::XMMATRIX baseWorld{
				{worldPosition.WorldPos.Ratio, 0.0f, 0.0f, 0.0f },
				{0.0f, worldPosition.WorldPos.Ratio, 0.0f, 0.0f},
				{0.0f, 0.0f, 0.0f, 0.0f},
				worldPosition.WorldPos.Parallel
			};
			pDst[1]->Set(&baseWorld,
				TexIndex[1],
				&Color0[1],
				&Color1[1],
				&Color2[1]);

			//�͗l
			//��ɂ�����̂����Ɍ�����̂̍Č�
			DirectX::XMMATRIX patternWorld{
				0.0f, 0.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 0.0f, 0.0f,
					0.0f, worldPosition.WorldPos.Ratio * 0.2f, 0.0f, 0.0f
			};
			patternWorld = patternWorld + worldPosition.WorldMatrix;
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
		BulletAppearance(Json::Value fromLoad) {
			TexHeadIndex = (float)BulletTextureArray.Append(fromLoad.get("texture", ""));
			TexIndex = TexHeadIndex;
		}
		void CopyToBuffer(Interface::BulletInstanceType* pDst,
			Component::WorldPosition& worldPosition) {
			pDst->Set(&worldPosition.WorldMatrix,TexIndex);
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
		EffectAppearance(Json::Value fromLoad) {
			TexHeadIndex = (float)BulletTextureArray.Append(fromLoad.get("texture", ""));
			TexIndex = TexHeadIndex;
			Color0 = Interface::GetVectorFromJson(fromLoad.get("baseColor0", ""));
			Color1 = Interface::GetVectorFromJson(fromLoad.get("baseColor1", ""));
		}
		void CopyToBuffer(Interface::EffectInstanceType* pDst,
			Component::WorldPosition& worldPosition) {
			pDst->Set(&worldPosition.WorldMatrix,
				TexIndex,
				&Color0,
				&Color1);
		}
		void SetInstanceId(Interface::RectAppId newId) {
			BufferDataIndex = newId;
		}
	};
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
		CircleHitbox(Json::Value fromLoad) {
			DiameterCoef = fromLoad.get("diameterCoef","").asFloat();
			RelativeCenter = Interface::GetVectorFromJson(fromLoad.get("relativeCenter", ""));
		}
	};
	// �e�{�[���ɕt�����铖���蔻��/��炢����Ɋւ��鐫��
	struct BallHurtbox {
	public:
		// World�̔��a�ɑ΂��Ă̔��a�̔�
		float DiameterCoef;
		void Init(Interface::EntityInitData* pInitData) {

		}
		BallHurtbox(Json::Value fromLoad) {
			DiameterCoef = fromLoad.get("diameterCoef", "").asFloat();
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
		UnitOccupationbox(Json::Value fromLoad) {
			AlredayChecked = false;
		}
	};
	struct WallHurtbox {
	public:
		bool RefleshOccupation;
		void Init(Interface::EntityInitData* pInitData) {

		}
		WallHurtbox(Json::Value fromLoad) {
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
		CorpsData(Json::Value fromLoad) {

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
		float AttackAt;
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
			MoveTilePerTick = 0.06;
			SpeedBuff = pInitData->SpeedMultiply;
			BallCount = 0;
			DistanceThresholdMax = 0;
			DistanceThresholdMin = 0;
			FinalTarget.Pos = pInitData->Pos.Parallel;
			AreaBorderTarget.Pos = pInitData->Pos.Parallel;
			PreviousTickArea = -1;
			PreviousTargetPos = {0,0,0,1};
			Heading = { 0,0,0,0 };
			Fire = false;
			NextPosReloadTick = 0;

			MaxHP = MaxHP * pInitData->HealthMultiply;
		}
		UnitData(Json::Value fromLoad) {
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
		Attack(Json::Value fromLoad) {
			CoolTime = fromLoad.get("coolTime", "").asInt();
			NextAbleTick = 0;
			Bullet = Interface::EntNameHash[fromLoad.get("bullet", "").asString()];
			Count = fromLoad.get("count", "").asInt();
			headingError = fromLoad.get("headingError", "").asFloat();
			headingFork = fromLoad.get("headingFork", "").asFloat();
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
			AngleFromCore = atan2f(pInitData->Pos.Parallel.m128_f32[1], pInitData->Pos.Parallel.m128_f32[0]);
			OnCenter = pInitData->Pos.Parallel.m128_f32[0] == 0 && pInitData->Pos.Parallel.m128_f32[1] == 0;
			Attack = Attack * pInitData->DamageMultiply;
			CoreId = pInitData->CoreId;
		}
		BallData(Json::Value fromLoad) {
			Attack = fromLoad.get("attack", "").asFloat();
			MovePower = fromLoad.get("movePower", "").asFloat();
			RotatePower = fromLoad.get("rotatePower", "").asFloat();
			Weight = fromLoad.get("weight", "").asFloat();
		}
	};
	struct BulletData {
		bool InterfareToWall;
		bool InterfareToBall;
		Interface::HostilityTeam Team;
		void Init(Interface::EntityInitData* pInitData) {
			Team = pInitData->Team;
		}
		BulletData(Json::Value fromLoad) {
			InterfareToWall = fromLoad.get("interfareToWall", "").asBool();
			InterfareToBall = fromLoad.get("interfareToBall", "").asBool();
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
		BlockData(Json::Value fromLoad) {

		}
	};
	struct GiveDamage {
	public:
		Interface::Damage Damage;
		void Init(Interface::EntityInitData* pInitData) {

		}
		GiveDamage(Json::Value fromLoad) {
			Damage = Interface::Damage(fromLoad);
		}
	};
	struct DamagePool {
	public:
		Interface::Damage Damage;
		void Init(Interface::EntityInitData* pInitData) {

		}
		DamagePool(Json::Value fromLoad) {
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
		HitEffect(Json::Value fromLoad) {
			PrototypeEntity = Interface::EntNameHash[fromLoad.get("effectName", "").asString()];
			LeftRadian = fromLoad.get("leftRadian", "").asFloat();
			RightRadian = fromLoad.get("rightRadian", "").asFloat();
			EjectCount = fromLoad.get("count", "").asInt();
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
		Trajectory(Json::Value fromLoad) {
			PrototypeEntity = Interface::EntNameHash[fromLoad.get("effectName", "").asString()];
			LeftRadian = fromLoad.get("leftRadian", "").asFloat();
			RightRadian = fromLoad.get("rightRadian", "").asFloat();
			EjectCount = fromLoad.get("count", "").asInt();
			EjectStartTick = fromLoad.get("ejectStartTick", "").asInt();
			EjectEndTick = fromLoad.get("ejectEndTick", "").asInt();
			EjectInterval = fromLoad.get("ejectInterval", "").asInt();
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
		InvationObservance(Json::Value fromLoad) {
			Left = fromLoad.get("observeArea", "").get("left", "").asInt();
			Right = fromLoad.get("observeArea", "").get("right", "").asInt();
			Bottom = fromLoad.get("observeArea", "").get("bottom", "").asInt();
			Top = fromLoad.get("observeArea", "").get("top", "").asInt();
		}
	};
	struct UnitCountObservance {
		int Border;
		Interface::HostilityTeam Team;
		void Init(Interface::EntityInitData* pInitData) {

		}
		UnitCountObservance(Json::Value fromLoad) {
			Border = fromLoad.get("borderCount", "").asInt();
			Team = fromLoad.get("team", "").asInt();
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
		Spawn(Json::Value fromLoad) {
			Leader = Interface::UnitNameHash[fromLoad.get("leaderUnit", "").asString()];
			Member = Interface::UnitNameHash[fromLoad.get("memberUnit", "").asString()];
			CoreData = Interface::EntityInitData(fromLoad.get("initData", ""));
			CountLeft = fromLoad.get("count", "").asInt();
			TimeGapSince = fromLoad.get("timeGap", "").asInt();
			SpawnFlag = false;
			SpawnAreaLeft = fromLoad.get("spawnArea", "").get("left", "").asInt();
			SpawnAreaRight = fromLoad.get("spawnArea", "").get("right", "").asInt();
			SpawnAreaTop = fromLoad.get("spawnArea", "").get("top", "").asInt();
			SpawnAreaBottom = fromLoad.get("spawnArea", "").get("bottom", "").asInt();
		}
	};

	struct Test1
	{
		Test1() {

		}
		void Init(Interface::EntityInitData* pInitData) {

		}
		Test1(Json::Value fromLoad) {

		}
	};
	struct Test2
	{
		Test2() {

		}
		void Init(Interface::EntityInitData* pInitData) {

		}
		Test2(Json::Value fromLoad) {

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
		Test3(Json::Value fromLoad) {

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
		TestResult(Json::Value fromLoad) {
			UpdatedByTestA = false;
			UpdatedByTestB = false;
			UpdatedByTestC = false;
			UpdatedByTestD = false;
		}
	};
};