#pragma once
#include "../../Interfaces.h"
#include "../../Source/DirectX/Texture.h"


//Initに入れるのは各インスタンスが生成されるときに変更したいこと
//読み込み時に確定していることは書かなくてよい

//全エンティティで読み込みたいオブジェクトはAllEntities::GetArchetypeFromCompNamesで設定する
// Systemの最後の処理に初期化、削除用のものを用意する
// Initに関しては引数が異なるのでそれぞれSameArchetype::Addに書く(void*とか使えば何とかなるんかな？型チェックがされないのであんまやりたくないけど)
// これは同時に生成したり消したいので
// コンポーネントのデータ型、と簡単な管理を行う関数
extern SameFormatTextureArray<256> BlockTextureArray;
extern SameFormatTextureArray<256> BallTextureArray;
extern SameFormatTextureArray<256> BulletTextureArray;
namespace Component {
	struct KillFlag {
		bool KillOnThisTick;
		void Init(Interface::EntityInitData* pInitData) {
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
		//動いた場合描画データの更新が必要なので
		// 前ティックで実際に動いた
		bool Moved;
		// このティック終了時に動きたい
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
	// となるので変換したい座標を持つ方を前項とする必要がある

	//運動に関する性質
	//描画や衝突判定にも使用
	struct WorldPosition {
		// 相似拡縮、z軸を中心とした回転、x,y,z方向の平行移動を記録できる
		// 1,1,3の変数で表現できるのでいちいち行列を使うよりもその方がよさそうな気もする
		// 倍率を求めるには2x2成分取り出して行列式に通すとか必要だし
		// 一方で共通のインターフェイスとして行列も欲しい
		// 状況次第では拡縮と平行移動の影響だけを受けた座標が欲しい場合もあるし
		// DirectX::XMMATRIX1つが64kBで256*256*3個用意すると12MBとなる
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
		}
	};
 	// TODO 運動に関しては整理したい
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
		//コアの回転や拡縮はこのパーツの大きさや傾きに影響を与えているので同様に変化の方向、長さを変えないとおかしくなる
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
	//見た目に関する性質
	struct BlockAppearance {
	public:
		Interface::RectAppId BufferDataIndex;
		DirectX::XMVECTOR TexCoord12;
		DirectX::XMVECTOR TexCoord3M;
		//ブロックに色がつくやつ
		DirectX::XMVECTOR Stain;
		void Init(Interface::EntityInitData* pInitData) {
		}
		BlockAppearance(Json::Value fromLoad) {
			TexCoord12 = DirectX::XMVECTOR
			{
				fromLoad.get("tex1SamePageIndex", "").asFloat(),
				(float)BlockTextureArray.AppendFromFileName(fromLoad.get("tex1FilePath", "").asString()),
				fromLoad.get("tex2SamePageIndex", "").asFloat(),
				(float)BlockTextureArray.AppendFromFileName(fromLoad.get("tex2FilePath", "").asString()),
			};
			TexCoord3M = DirectX::XMVECTOR
			{
				fromLoad.get("tex3SamePageIndex", "").asFloat(),
				(float)BlockTextureArray.AppendFromFileName(fromLoad.get("tex3FilePath", "").asString()),
				fromLoad.get("maskSamePageIndex", "").asFloat(),
				(float)BlockTextureArray.AppendFromFileName(fromLoad.get("maskFilePath", "").asString()),
			};
			Stain = { 0,0,0,0 };
		}
	};
	struct BallAppearance {
	public:
		// 足元(落ち影)、本体色・陰、模様・ハイライト
		Interface::RectAppId BufferDataIndex;
		DirectX::XMVECTOR Color0[3];
		DirectX::XMVECTOR Color1[3];
		DirectX::XMVECTOR Color2[3];
		DirectX::XMVECTOR TexCoord[3];
		void Init(Interface::EntityInitData* pInitData) {
			// 1はBaseのインデックス
			Color0[1] = pInitData->BaseColor0;
			Color1[1] = pInitData->BaseColor1;
		}
		void Load(Json::Value fromLoad,std::string layerName,int index) {
			TexCoord[index] = DirectX::XMVECTOR
			{
				fromLoad.get(layerName,"").get("samePageIndex", "").asFloat(),
				(float)BallTextureArray.AppendFromFileName(fromLoad.get(layerName,"").get("filePath", "").asString()),
				0,0
			};
			Color0[index] = Interface::GetVectorFromJson(fromLoad.get(layerName, "").get("color0", ""));
			Color1[index] = Interface::GetVectorFromJson(fromLoad.get(layerName, "").get("color1", ""));
			Color2[index] = Interface::GetVectorFromJson(fromLoad.get(layerName, "").get("color2", ""));
		}
		BallAppearance(Json::Value fromLoad) {
			Load(fromLoad, "shadow", 0);
			Load(fromLoad, "base", 1);
			Load(fromLoad, "pattern", 2);
		}
	};
	struct BulletAppearance {
	public:
		Interface::RectAppId BufferDataIndex;
		DirectX::XMVECTOR Color0;
		DirectX::XMVECTOR Color1;
		DirectX::XMVECTOR TexCoord;
		void Init(Interface::EntityInitData* pInitData) {

		}
		BulletAppearance(Json::Value fromLoad) {
			TexCoord = DirectX::XMVECTOR
			{
				fromLoad.get("baseSamePageIndex", "").asFloat(),
				(float)BulletTextureArray.AppendFromFileName(fromLoad.get("baseFilePath", "").asString()),
				0,0
			};
			Color0 = Interface::GetVectorFromJson(fromLoad.get("baseColor0", ""));
			Color1 = Interface::GetVectorFromJson(fromLoad.get("baseColor1", ""));
		}
	};
	//当たり判定に関する性質
	struct CircleHitbox {
	public:
		DirectX::XMVECTOR RelativeCenter;
		DirectX::XMVECTOR AbsoluteCenter;
		float Radius;
		// Worldの半径に対しての半径の比
		float DiameterCoef;
		void Init(Interface::EntityInitData* pInitData) {

		}
		CircleHitbox(Json::Value fromLoad) {
			DiameterCoef = fromLoad.get("diameterCoef","").asFloat();
			RelativeCenter = {
				fromLoad.get("relativeCenter", "")[0].asFloat(),
				fromLoad.get("relativeCenter", "")[1].asFloat(),
				0,1
			};
		}
	};
	// 各ボールに付随する当たり判定/喰らい判定に関する性質
	struct BallHurtbox {
	public:
		// Worldの半径に対しての半径の比
		float DiameterCoef;
		void Init(Interface::EntityInitData* pInitData) {

		}
		BallHurtbox(Json::Value fromLoad) {
			DiameterCoef = fromLoad.get("diameterCoef", "").asFloat();
		}
	};
	// コアに付随する占有に関する性質
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
	//オブジェクト内部の性質

	//コアに関する性質
	//バフの管理
	struct CorpsData {
	public:

		// 現在の部隊の状態の情報
		Interface::HostilityTeam Team;
		entt::entity LeaderUnit;
		std::vector<std::tuple<entt::entity, float>> HighDamageUnit;
		std::vector<entt::entity> AllMemberUnit;

		// 構成するユニットの情報

		// 現在出ている命令について
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

		// 移動速度系
		float RadianPerTick;
		float MoveTilePerTick;
		float Weight;
		float SpeedBuff;

		// 構成するボールの情報
		entt::entity BallIds[7];
		int BallCount;

		// 行動の選択にかかわる情報
		float AttackRange;
		float AttackAzimath;


		// 選択している行動・これらを運動に反映させる
		entt::entity TargetEntity;
		float DistanceThresholdMin;
		float DistanceThresholdMax;
		int PreviousTickArea;
		// ↓
		Interface::WayPoint FinalTarget;
		DirectX::XMVECTOR PreviousTargetPos;
		bool TargetUpdated;
		// ↓
		bool HeadingFree;
		Interface::WayPoint AreaBorderTarget;
		int NextPosReloadTick;
		// Centerを中心としてLengthの範囲内の場所へ移動する
		// 定点位置、方向で停止する



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
		}
		UnitData(Json::Value fromLoad) {
			AttackRange = 20;
		}
	};
	struct BallData {
	public:
		entt::entity CoreId;
		float HP;
		float Attack;
		float MovePower;
		float RotatePower;
		float Weight;

		void Init(Interface::EntityInitData* pInitData){
			HP = HP * pInitData->HealthMultiply;
			Attack = Attack * pInitData->DamageMultiply;
			CoreId = pInitData->CoreId;
		}
		BallData(Json::Value fromLoad) {
			HP = fromLoad.get("HP","").asFloat();
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
	};
	//他のオブジェクトの生成にかかわる性質
	struct HitEffect {
	public:
		entt::entity PrototypeEntity;
		float LeftRadian;// 生成の幅
		float RightRadian;// 生成の幅
		int EjectCount;// 生成数

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
		float LeftRadian;// 生成の幅
		float RightRadian;// 生成の幅
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
		int TimeGapSince;// スポーン開始までの残りティック
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