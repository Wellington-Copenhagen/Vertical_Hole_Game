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
enum CompNames {
	HitFlag,
	MoveFlag,
	KillFlag,
	GenerateFlag,
	AppearanceChanged,
	PositionReference,

	AI,

	WorldPosition,
	LinearAcceralation,
	Motion,

	BlockAppearance,
	CharacterAppearance,
	BulletAppearance,

	CircleHitbox,
	CharacterHurtbox,
	WallHurtbox,

	CharacterData,
	BulletData,
	GiveDamage,
	DamagePool,

	HitEffect,
	Trajectory,

	Test1,
	Test2,
	TestResult,
	Test3,
};
static std::vector<std::string> CompNameArray = {
	"HitFlag",
	"MoveFlag",
	"KillFlag",
	"GenerateFlag",
	"AppearanceChanged",
	"PositionReference",

	"AI",

	"WorldPosition",
	"LinearAcceralation",
	"Motion",

	"BlockAppearance",
	"CharacterAppearance",
	"BulletAppearance",

	"CircleHitbox",
	"CharacterHurtbox",
	"WallHurtbox",

	"CharacterData",
	"BulletData",
	"GiveDamage",
	"DamagePool",

	"HitEffect",
	"Trajectory",

	"Test1",
	"Test2",
	"TestResult",
	"Test3",
};
template<typename CompType>
class ComponentWrapper {
public:
	ComponentWrapper() {
		Components = std::vector<CompType>();
	}
	ComponentWrapper(Interface::RawArchetype thisArchetype, CompNames compOfThis) {
		UsedFlag = thisArchetype.test(compOfThis);
		Components = std::vector<CompType>();
	}
	bool UsedFlag;
	std::vector<CompType> Components;
	inline void Delete(Interface::SameArchIndex deleteIndex) {
		if (UsedFlag) {
			Components[deleteIndex] = Components.back();
			Components.pop_back();
		}
	}
	//実行中にプロトタイプをコピーする用
	inline void Add(ComponentWrapper<CompType> prototype, Interface::SameArchIndex prototypeIndex) {
		if (UsedFlag) {
			Components.push_back(CompType(prototype.Components[prototypeIndex]));
		}
	}
	//プロトタイプを読み込むよう
	inline void Add(Json::Value fromLoad, std::string compTypeName) {
		if (UsedFlag) {
			Components.push_back(CompType(fromLoad, compTypeName));
		}
	}
};
namespace Component {
	struct KillFlag {
		bool KillOnThisTick;
		void Init() {
			KillOnThisTick = false;
		}
		KillFlag(Json::Value fromLoad, std::string compTypeName) {
			KillOnThisTick = false;
		}
	};
	struct GenerateFlag {
		bool GeneratedOnThisTick;
		void Init() {
			GeneratedOnThisTick = true;
		}
		GenerateFlag(Json::Value fromLoad, std::string compTypeName) {
			GeneratedOnThisTick = true;
		}
	};
	struct MoveFlag {
		//動いた場合描画データの更新が必要なので
		// 前ティックで実際に動いた
		bool Moved;
		// このティック終了時に動きたい
		bool TryMove;
		void Init() {
			Moved = true;
			TryMove = false;
		}
		MoveFlag(Json::Value fromLoad, std::string compTypeName) {
			Moved = true;
			TryMove = false;
		}
	};
	struct AppearanceChanged {
	public:
		bool Changed;
		void Init() {
			Changed = false;
		}
		AppearanceChanged(Json::Value fromLoad, std::string compTypeName) {
			Changed = false;
		}
	};
	struct HitFlag {
	public:
		bool IsHit;
		void Init() {
			IsHit = false;
		}
		HitFlag(Json::Value fromLoad, std::string compTypeName) {
			IsHit = false;
		}
	};
	struct PositionReference {
	public:
		Interface::EntId ReferenceTo;
		bool IsCore;
		void Init(bool isCore,Interface::EntId referenceTo) {
			IsCore = isCore;
			ReferenceTo = referenceTo;
		}
		PositionReference(Json::Value fromLoad, std::string compTypeName) {
			IsCore = true;
			ReferenceTo = 0;
		}
	};
	//AIに関する性質
	struct AI {
	public:
		bool Move;
		bool Attack;
		float AttackDist;
		Interface::IntXY From;
		Interface::IntXY To;
		float Progress;
		AI(Json::Value fromLoad, std::string compTypeName) {

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
		void Init(Interface::RelationOfCoord* CoreReferenceToWorld,bool isCore) {
			if (isCore) {
				WorldPos = LocalReferenceToCore * (*CoreReferenceToWorld);
				WorldMatrix = WorldPos.GetMatrix();
			}
			else {
				LocalReferenceToCore = *CoreReferenceToWorld;
			}
		}
		WorldPosition(Json::Value fromLoad, std::string compTypeName) {
			LocalReferenceToCore.Parallel = Interface::GetVectorFromJson(fromLoad.get(compTypeName, "").get("parallel", ""));
			LocalReferenceToCore.Ratio = fromLoad.get(compTypeName, "").get("width", "").asFloat();
			LocalReferenceToCore.Rotate = 0;
		}
	};
 	// TODO 運動に関しては整理したい
	struct LinearAcceralation {
	public:
		Interface::RelationOfCoord World;
		LinearAcceralation(Json::Value fromLoad, std::string compTypeName) {
			World.Parallel = Interface::GetVectorFromJson(fromLoad.get(compTypeName, "").get("parallel", ""));
			World.Ratio = fromLoad.get(compTypeName, "").get("zoom", "").asFloat();
			World.Rotate = fromLoad.get(compTypeName, "").get("rotate", "").asFloat();
		}
	};
	struct Motion {
	public:
		Interface::RelationOfCoord WorldDelta;
		//コアの回転や拡縮はこのパーツの大きさや傾きに影響を与えているので同様に変化の方向、長さを変えないとおかしくなる
		void Init(Interface::RelationOfCoord CoreReferenceToWorld) {
			WorldDelta.Parallel = DirectX::XMVector4Transform(
					DirectX::XMVectorScale(WorldDelta.Parallel, CoreReferenceToWorld.Ratio),
					DirectX::XMMatrixRotationZ(CoreReferenceToWorld.Rotate));
		}
		Motion(Json::Value fromLoad, std::string compTypeName) {
			if (fromLoad.get(compTypeName, "").isMember("parallel")) {
				WorldDelta.Parallel = Interface::GetVectorFromJson(fromLoad.get(compTypeName, "").get("parallel", ""));
			}
			else {
				WorldDelta.Parallel = { 0.0f,0.0f, 0.0f, 0.0f };
			}
			if (fromLoad.get(compTypeName, "").isMember("rotate")) {
				WorldDelta.Ratio = fromLoad.get(compTypeName, "").get("zoom", "").asFloat();
			}
			else {
				WorldDelta.Ratio = 1.0f;
			}
			if (fromLoad.get(compTypeName, "").isMember("ratate")) {
				WorldDelta.Rotate = fromLoad.get(compTypeName, "").get("rotate", "").asFloat();
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
		void Init(int initialMaskIndex) {
			TexCoord3M = DirectX::XMVECTOR
			{
				TexCoord3M.m128_f32[0],
				TexCoord3M.m128_f32[1],
				(float)initialMaskIndex,
				TexCoord3M.m128_f32[3],
			};
		}
		void Delete() {

		}
		BlockAppearance(Json::Value fromLoad, std::string compTypeName) {
			TexCoord12 = DirectX::XMVECTOR
			{
				fromLoad.get(compTypeName,"").get("tex1SamePageIndex", "").asFloat(),
				(float)SameFormatTextureArray::Append(fromLoad.get(compTypeName,"").get("tex1FilePath", "").asString()),
				fromLoad.get(compTypeName,"").get("tex2SamePageIndex", "").asFloat(),
				(float)SameFormatTextureArray::Append(fromLoad.get(compTypeName,"").get("tex2FilePath", "").asString()),
			};
			TexCoord3M = DirectX::XMVECTOR
			{
				fromLoad.get(compTypeName,"").get("tex3SamePageIndex", "").asFloat(),
				(float)SameFormatTextureArray::Append(fromLoad.get(compTypeName,"").get("tex3FilePath", "").asString()),
				fromLoad.get(compTypeName,"").get("maskSamePageIndex", "").asFloat(),
				(float)SameFormatTextureArray::Append(fromLoad.get(compTypeName,"").get("maskFilePath", "").asString()),
			};
			Stain = { 0,0,0,0 };
		}
	};
	struct CharacterAppearance {
	public:
		// 足元(落ち影)、本体色・陰、模様・ハイライト
		Interface::RectAppId BufferDataIndex;
		DirectX::XMVECTOR Color0[3];
		DirectX::XMVECTOR Color1[3];
		DirectX::XMVECTOR Color2[3];
		DirectX::XMVECTOR TexCoord[3];
		void Delete() {

		}
		void Load(Json::Value fromLoad,std::string compTypeName,std::string layerName,int index) {
			TexCoord[index] = DirectX::XMVECTOR
			{
				fromLoad.get(compTypeName,"").get(layerName,"").get("samePageIndex", "").asFloat(),
				(float)SameFormatTextureArray::Append(fromLoad.get(compTypeName,"").get(layerName,"").get("filePath", "").asString()),
				0,0
			};
			Color0[index] = Interface::GetVectorFromJson(fromLoad.get(compTypeName, "").get(layerName, "").get("color0", ""));
			Color1[index] = Interface::GetVectorFromJson(fromLoad.get(compTypeName, "").get(layerName, "").get("color1", ""));
			Color2[index] = Interface::GetVectorFromJson(fromLoad.get(compTypeName, "").get(layerName, "").get("color2", ""));
		}
		CharacterAppearance(Json::Value fromLoad, std::string compTypeName) {
			Load(fromLoad, compTypeName, "shadow", 0);
			Load(fromLoad, compTypeName, "base", 1);
			Load(fromLoad, compTypeName, "pattern", 2);
		}
	};
	struct BulletAppearance {
	public:
		Interface::RectAppId BufferDataIndex;
		DirectX::XMVECTOR Color0;
		DirectX::XMVECTOR Color1;
		DirectX::XMVECTOR TexCoord;
		void Delete() {

		}
		BulletAppearance(Json::Value fromLoad, std::string compTypeName) {
			TexCoord = DirectX::XMVECTOR
			{
				fromLoad.get(compTypeName,"").get("baseSamePageIndex", "").asFloat(),
				(float)SameFormatTextureArray::Append(fromLoad.get(compTypeName,"").get("baseFilePath", "").asString()),
				0,0
			};
			Color0 = Interface::GetVectorFromJson(fromLoad.get(compTypeName, "").get("baseColor0", ""));
			Color1 = Interface::GetVectorFromJson(fromLoad.get(compTypeName, "").get("baseColor1", ""));
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
		CircleHitbox(Json::Value fromLoad, std::string compTypeName) {
			DiameterCoef = fromLoad.get(compTypeName, "").get("diameterCoef","").asFloat();
			RelativeCenter = {
				fromLoad.get(compTypeName, "").get("relativeCenter", "")[0].asFloat(),
				fromLoad.get(compTypeName, "").get("relativeCenter", "")[1].asFloat(),
				0,1
			};
		}
	};
	struct CharacterHurtbox {
	public:
		DirectX::XMVECTOR Center;
		float Radius;
		// Worldの半径に対しての半径の比
		float DiameterCoef;

		int OccupingRectTop;
		int OccupingRectBottom;
		int OccupingRectLeft;
		int OccupingRectRight;
		void Delete() {

		}
		CharacterHurtbox(Json::Value fromLoad, std::string compTypeName) {
			DiameterCoef = fromLoad.get(compTypeName, "").get("diameterCoef", "").asFloat();
		}
	};
	struct WallHurtbox {
	public:
		bool RefleshOccupation;
		Interface::IntXY Pos;
		Interface::EntityPointer WrittenPointer;
		void Delete() {

		}
		WallHurtbox(Json::Value fromLoad, std::string compTypeName) {
			RefleshOccupation = true;
		}
	};
	//オブジェクト内部の性質
	struct CharacterData {
	public:
		float HP;
		float Attack;
		float TilePerTick;
		Interface::HostilityTeam Team;
		bool InterfareToWall;
		void Init(Interface::CombatUnitInitData* pCombatUnitInit){
			HP = HP * pCombatUnitInit->HealthMultiply;
			Attack = Attack * pCombatUnitInit->DamageMultiply;
			TilePerTick = TilePerTick * pCombatUnitInit->SpeedMultiply;
			Team = pCombatUnitInit->Team;
		}
		CharacterData(Json::Value fromLoad, std::string compTypeName) {
			HP = fromLoad.get(compTypeName,"").get("HP","").asFloat();
			Attack = fromLoad.get(compTypeName,"").get("attack", "").asFloat();
			TilePerTick = fromLoad.get(compTypeName,"").get("speed", "").asFloat();
			Team = 0;
			InterfareToWall = fromLoad.get(compTypeName, "").get("interfareToWall", "").asBool();
		}
	};
	struct BulletData {
		bool InterfareToWall;
		bool InterfareToCharacter;
		Interface::HostilityTeam Team;
		BulletData(Json::Value fromLoad, std::string compTypeName) {
			InterfareToWall = fromLoad.get(compTypeName,"").get("interfareToWall", "").asBool();
			InterfareToCharacter = fromLoad.get(compTypeName, "").get("interfareToCharacter", "").asBool();
		}
	};
	struct BigCharacter {
		int Diameter;//幅から1引いたもの
		BigCharacter(BigCharacter* prototype) {
			Diameter = prototype->Diameter;
		}
		
	};
	struct GiveDamage {
	public:
		Interface::Damage Damage;
		GiveDamage(Json::Value fromLoad, std::string compTypeName) {
			Damage = Interface::Damage(fromLoad.get(compTypeName,""));
		}
	};
	struct DamagePool {
	public:
		Interface::Damage Damage;
		DamagePool(Json::Value fromLoad, std::string compTypeName) {
			Damage = Interface::Damage(fromLoad.get(compTypeName, ""));
		}
	};
	//他のオブジェクトの生成にかかわる性質
	struct HitEffect {
	public:
		Interface::EntityPointer PrototypeEntP;
		float LeftRadian;// 生成の幅
		float RightRadian;// 生成の幅
		UINT16 EjectCount;// 生成数

		HitEffect(Json::Value fromLoad, std::string compTypeName) {
			PrototypeEntP = Interface::EntNameHash[fromLoad.get(compTypeName,"").get("effectName", "").asString()];
			LeftRadian = fromLoad.get(compTypeName,"").get("leftRadian", "").asFloat();
			RightRadian = fromLoad.get(compTypeName,"").get("rightRadian", "").asFloat();
			EjectCount = fromLoad.get(compTypeName,"").get("count", "").asFloat();
		}
	};
	struct Trajectory {
	public:
		Interface::EntityPointer PrototypeEntP;
		UINT16 EjectStartTick;
		UINT16 EjectEndTick;
		UINT16 EjectInterval;
		float LeftRadian;// 生成の幅
		float RightRadian;// 生成の幅
		UINT16 EjectCount;
		Trajectory(Json::Value fromLoad, std::string compTypeName) {
			PrototypeEntP = Interface::EntNameHash[fromLoad.get(compTypeName,"").get("effectName", "").asString()];
			LeftRadian = fromLoad.get(compTypeName,"").get("leftRadian", "").asFloat();
			RightRadian = fromLoad.get(compTypeName,"").get("rightRadian", "").asFloat();
			EjectCount = fromLoad.get(compTypeName,"").get("count", "").asFloat();
			EjectStartTick = fromLoad.get(compTypeName,"").get("ejectStartTick", "").asFloat();
			EjectEndTick = fromLoad.get(compTypeName,"").get("ejectEndTick", "").asFloat();
			EjectInterval = fromLoad.get(compTypeName,"").get("ejectInterval", "").asFloat();
		}
	};

	struct Test1
	{
		Test1() {

		}
		Test1(Json::Value fromLoad, std::string compTypeName) {

		}
	};
	struct Test2
	{
		Test2() {

		}
		Test2(Json::Value fromLoad, std::string compTypeName) {

		}
	};
	struct Test3
	{
		int Num;
		Test3(int num) {
			Num = num;
		}
		Test3(Json::Value fromLoad, std::string compTypeName) {

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
		TestResult(Json::Value fromLoad, std::string compTypeName) {

		}
	};
};