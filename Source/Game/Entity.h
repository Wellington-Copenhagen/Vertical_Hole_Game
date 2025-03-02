#pragma once
#include "../../Interfaces.h"
#include "Component.h"
#define DefineCompWrapper(CompName) ComponentWrapper<Component::CompName> CompName
//同じArchetypeを持つEntityの集合
class AllEntities;
struct UnitInfo;
class SameArchetype {
public:
	Interface::RawArchetype ThisArchetype;
	//新しいエンティティを作るときの付加情報はこの関数の引数にポインタを指定するくらいしか案はない(ないならnullptr)
	Interface::SameArchIndex Add(Interface::EntId nextId, Interface::EntityInitData* pInitData);
	//削除を一気にやらないと整合性の問題になりそう

	//削除した後穴埋めに使ったデータのIdを返す
	void Delete(AllEntities* pAllEntities, Interface::SameArchIndex toDelete);

	//削除対象
	SameArchetype(Interface::RawArchetype thisArchetype, bool IsPrototype = false);
	//これは読み込み時用
	void LoadToPrototype(Json::Value oneObject);
	void LoadToPrototypeAsTest() {
		Prototypes->ValidEntityCount = 4;
		Prototypes->Test3.Components.push_back(Component::Test3(0));
		Prototypes->Test3.Components.push_back(Component::Test3(1));
		Prototypes->Test3.Components.push_back(Component::Test3(2));
		Prototypes->Test3.Components.push_back(Component::Test3(3));
		Prototypes->Test3.Components.push_back(Component::Test3(4));
		Prototypes->Test3.Components.push_back(Component::Test3(5));
		Prototypes->Test3.Components.push_back(Component::Test3(6));
	}
	void LoadToEntitiesAsTest() {
		ValidEntityCount++;
		TestResult.Components.push_back(Component::TestResult());
		if (ThisArchetype.test(CompNames::Test1)) {
			Test1.Components.push_back(Component::Test1());
		}
		if (ThisArchetype.test(CompNames::Test2)) {
			Test2.Components.push_back(Component::Test2());
		}
	}

	SameArchetype* Prototypes;
	//前ティック終了時点で存在していたエンティティの数
	int ValidEntityCount;
	int RealEntityCount;

	int PrototypeCount;

	//全オブジェクトに固有で割り当てる
	//オブジェクト間の識別用
	std::vector<Interface::EntId> IdArray;
	//これより下が各Componentのデータ
	//共通するデータ
	DefineCompWrapper(HitFlag);
	DefineCompWrapper(KillFlag);
	DefineCompWrapper(GenerateFlag);
	DefineCompWrapper(MoveFlag);
	DefineCompWrapper(AppearanceChanged);
	DefineCompWrapper(PositionReference);
	//AIに関する性質
	DefineCompWrapper(AI);
	//当たり判定にかかわる性質
	DefineCompWrapper(CircleHitbox);
	DefineCompWrapper(BallHurtbox);
	DefineCompWrapper(UnitOccupationbox);
	DefineCompWrapper(WallHurtbox);
	DefineCompWrapper(GiveDamage);
	DefineCompWrapper(DamagePool);
	//内部にかかわる性質
	DefineCompWrapper(BallData);
	DefineCompWrapper(UnitData);
	DefineCompWrapper(BulletData);
	//他のオブジェクトの生成にかかわる性質
	DefineCompWrapper(HitEffect);
	DefineCompWrapper(Trajectory);
	//見た目にかかわる性質
	DefineCompWrapper(BlockAppearance);
	DefineCompWrapper(BallAppearance);
	DefineCompWrapper(BulletAppearance);
	//運動にかかわる性質
	DefineCompWrapper(Motion);
	DefineCompWrapper(LinearAcceralation);
	DefineCompWrapper(WorldPosition);
	//ミッション進行にかかわる性質
	DefineCompWrapper(InvationObservance);
	DefineCompWrapper(UnitCountObservance);
	DefineCompWrapper(Spawn);
	//テスト用
	DefineCompWrapper(Test1);
	DefineCompWrapper(Test2);
	DefineCompWrapper(TestResult);
	DefineCompWrapper(Test3);
};
class AllEntities {
public:
	//bitsetの引数はComponentの数だけ
	std::vector<Interface::RawArchetype> Archetypes;
	std::vector<SameArchetype> EntityArraies;
	//8MB
	std::vector<Interface::EntityPointer> INtoIndex;
	std::vector<UnitInfo> UnitInfos;
	std::vector<int> UnitCount;
	inline void SetAsDeleteFromEIN(Interface::EntId instanceEIN) {
		Interface::EntityPointer pointer = INtoIndex[instanceEIN];
		EntityArraies[pointer.Archetype].KillFlag.Components[pointer.Index].KillOnThisTick = true;
	}
	inline Interface::EntId AddFromEntPointer(Interface::EntityInitData* pInitData) {
		INtoIndex.push_back(Interface::EntityPointer(
			pInitData->Prototype.Archetype,
			EntityArraies[pInitData->Prototype.Archetype].Add(INtoIndex.size(),pInitData)));
		return INtoIndex.size() - 1;
	}
	AllEntities() {
		Archetypes = std::vector<Interface::RawArchetype>();
		EntityArraies = std::vector<SameArchetype>();
		INtoIndex = std::vector<Interface::EntityPointer>();
		UnitInfos = std::vector<UnitInfo>();
		UnitCount = std::vector<int>(TeamCount);
	}
	void LoadEntities(std::vector<std::string> fileNames);
	void LoadUnits(std::vector<std::string> filePathes);
	void LoadMission(std::string fileName);
	// ミッションの中で出てくるユニットの召喚用
	Interface::EntId AddUnitWithMagnification(int unitIndex,Interface::EntityInitData* pInitData, Interface::RelationOfCoord* CorePos);
	//namesにあるコンポーネントの名前を含むアーキタイプを生成する
	Interface::RawArchetype GetArchetypeFromCompNames(Json::Value::Members names) {
		Interface::RawArchetype output;
		output.reset();
		//全てのエンティティが持つコンポーネントを登録する
		for (int j = 0; j < CompNameArray.size(); j++) {
			if (j==CompNames::GenerateFlag || 
				j == CompNames::MoveFlag || 
				j == CompNames::KillFlag ||
				j == CompNames::HitFlag ||
				j == CompNames::AppearanceChanged || 
				j == CompNames::PositionReference) {
				output.set(j);
			}
		}
		//読み込んだエンティティ名リストにあるコンポーネントを登録する
		for (int i = 0; i < names.size(); i++) {
			for (int j = 0; j < CompNameArray.size(); j++) {
				if (Interface::SameString(names[i], CompNameArray[j])) {
					output.set(j);
				}
			}
		}
		return output;
	}



	//ここより下はテストで使うもの
	void SetCompToArchetypes(int entityArrayIndex, std::string sCompName) {
		int size = CompNameArray.size();
		for (int i = 0; i < size; i++) {
			if (Interface::SameString(sCompName, CompNameArray[i])) {
				Archetypes[entityArrayIndex].set(i);
			}
		}
		Archetypes[entityArrayIndex].set();
	}
	//entityArrayIndexのデータのアーキタイプにcompNameがあるという情報を足す
	void SetCompToArchetypes(int entityArrayIndex, CompNames compName) {
		Archetypes[entityArrayIndex].set(compName);
	}
	void LoadAsUpdateTest() {
		Archetypes.push_back(Interface::RawArchetype());
		SetCompToArchetypes(0, CompNames::Test1);
		SetCompToArchetypes(0, CompNames::TestResult);

		Archetypes.push_back(Interface::RawArchetype());
		SetCompToArchetypes(1, CompNames::Test2);
		SetCompToArchetypes(1, CompNames::TestResult);

		Archetypes.push_back(Interface::RawArchetype());
		SetCompToArchetypes(2, CompNames::Test1);
		SetCompToArchetypes(2, CompNames::Test2);
		SetCompToArchetypes(2, CompNames::TestResult);

		Archetypes.push_back(Interface::RawArchetype());
		SetCompToArchetypes(3, CompNames::TestResult);
		//RawArchetypeの設定→EntityArraiesの初期化の順
		EntityArraies.push_back(SameArchetype(Archetypes[0]));
		EntityArraies[0].LoadToEntitiesAsTest();

		EntityArraies.push_back(SameArchetype(Archetypes[1]));
		EntityArraies[1].LoadToEntitiesAsTest();

		EntityArraies.push_back(SameArchetype(Archetypes[2]));
		EntityArraies[2].LoadToEntitiesAsTest();

		EntityArraies.push_back(SameArchetype(Archetypes[3]));
		EntityArraies[3].LoadToEntitiesAsTest();
	}
	void LoadAsAddDeleteTest() {
		Archetypes.push_back(Interface::RawArchetype());
		SetCompToArchetypes(0, CompNames::Test3);

		EntityArraies.push_back(SameArchetype(Archetypes[0]));
		EntityArraies[0].LoadToPrototypeAsTest();

	}
};
struct UnitInfo {
	Interface::EntityPointer CorePrototype;
	Interface::EntityPointer PrototypePointer[7];
	Interface::RelationOfCoord RelativePosFromCore[7];
	DirectX::XMVECTOR BaseColor0[7];
	DirectX::XMVECTOR BaseColor1[7];
	float Ratio;
	UnitInfo() {
	}
	UnitInfo(Json::Value jsonOfThisUnit, AllEntities* pAllEntities) {
		CorePrototype = Interface::EntNameHash[jsonOfThisUnit.get("coreName", "").asString()];
		Ratio = jsonOfThisUnit.get("ratio", "").asFloat();
		for (int i = 0; i < 7; i++) {
			PrototypePointer[i] = Interface::EntNameHash[jsonOfThisUnit.get("balls", "")[i].get("ballName", "").asString()];
			Interface::RelationOfCoord toPush;
			//コアに対しての初期位置
			toPush.Parallel = DirectX::XMVECTOR{
				std::cosf(jsonOfThisUnit.get("balls","")[i].get("pos","").asInt() * PI / 3)/3,
				std::sinf(jsonOfThisUnit.get("balls","")[i].get("pos","").asInt() * PI / 3)/3,
				0,1
			};
			if (i == 0) {
				toPush.Parallel = { 0,0,0,1 };
			}
			toPush.Ratio = 1;
			toPush.Rotate = 0;
			RelativePosFromCore[i] = toPush;
			BaseColor0[i] = Interface::GetVectorFromJson(jsonOfThisUnit.get("balls", "")[i].get("baseColor0", ""));
			BaseColor1[i] = Interface::GetVectorFromJson(jsonOfThisUnit.get("balls", "")[i].get("baseColor1", ""));
		}
	}
};