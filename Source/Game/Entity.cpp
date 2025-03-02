#include "Entity.h"
#define AddCompMacro(CompName) CompName.Add(Prototypes->CompName, pInitData)
#define DeleteCompMacro(CompName) CompName.Delete(toDelete)
#define InitCompWrapper(CompName) CompName = ComponentWrapper<Component::CompName>(thisArchetype, CompNames::CompName)
#define InitPrototypeCompWrapper(CompName) Prototypes->CompName.Add(oneObject, CompNameArray[CompNames::CompName])

// coreWorldはcoreである場合はcoreの位置、coreでない場合はcoreとの位置関係
Interface::SameArchIndex SameArchetype::Add(Interface::EntId nextId, Interface::EntityInitData* pInitData) {
	IdArray.push_back(nextId);

	// Componentの種類が増えたらここを増やす必要がある
	AddCompMacro(HitFlag);
	AddCompMacro(KillFlag);
	AddCompMacro(GenerateFlag);
	AddCompMacro(MoveFlag);
	AddCompMacro(AppearanceChanged);
	AddCompMacro(PositionReference);
	//AIに関する性質
	AddCompMacro(AI);
	//当たり判定にかかわる性質
	AddCompMacro(CircleHitbox);
	AddCompMacro(BallHurtbox);
	AddCompMacro(UnitOccupationbox);
	AddCompMacro(WallHurtbox);
	AddCompMacro(GiveDamage);
	AddCompMacro(DamagePool);
	//内部にかかわる性質
	AddCompMacro(BallData);
	AddCompMacro(UnitData);
	AddCompMacro(BulletData);
	//他のオブジェクトの生成にかかわる性質
	AddCompMacro(HitEffect);
	AddCompMacro(Trajectory);
	//見た目にかかわる性質
	AddCompMacro(BlockAppearance);
	AddCompMacro(BallAppearance);
	AddCompMacro(BulletAppearance);
	//運動にかかわる性質
	AddCompMacro(Motion);
	AddCompMacro(LinearAcceralation);
	AddCompMacro(WorldPosition);
	//テスト用
	AddCompMacro(Test1);
	AddCompMacro(Test2);
	AddCompMacro(TestResult);
	AddCompMacro(Test3);
	//Motion.Components.back().Init();
	//Rotate.Components.back().Init();
	return IdArray.size() - 1;
}
//toDeleteにあるデータを削除して間を埋めるために移動させたEntityのEntIdを返す
//ただし、最後のEntityを削除した場合は-1を返す
void SameArchetype::Delete(AllEntities* pAllEntities,Interface::SameArchIndex toDelete){
	//AllEntitiesのINtoIndexを更新する処理
	pAllEntities->INtoIndex[IdArray.back()].Index = toDelete;

	//IdArrayのデータを消す処理
	IdArray[toDelete] = IdArray.back();
	IdArray.pop_back();

	// Componentの種類が増えたらここを増やす必要がある
	DeleteCompMacro(HitFlag);
	DeleteCompMacro(KillFlag);
	DeleteCompMacro(GenerateFlag);
	DeleteCompMacro(MoveFlag);
	DeleteCompMacro(AppearanceChanged);
	DeleteCompMacro(PositionReference);
	//AIに関する性質
	DeleteCompMacro(AI);
	//当たり判定にかかわる性質
	DeleteCompMacro(CircleHitbox);
	DeleteCompMacro(BallHurtbox);
	DeleteCompMacro(UnitOccupationbox);
	DeleteCompMacro(WallHurtbox);
	DeleteCompMacro(GiveDamage);
	DeleteCompMacro(DamagePool);
	//内部にかかわる性質
	DeleteCompMacro(BallData);
	DeleteCompMacro(UnitData);
	DeleteCompMacro(BulletData);
	//他のオブジェクトの生成にかかわる性質
	DeleteCompMacro(HitEffect);
	DeleteCompMacro(Trajectory);
	//見た目にかかわる性質
	DeleteCompMacro(BlockAppearance);
	DeleteCompMacro(BallAppearance);
	DeleteCompMacro(BulletAppearance);
	//運動にかかわる性質
	DeleteCompMacro(Motion);
	DeleteCompMacro(LinearAcceralation);
	DeleteCompMacro(WorldPosition);
	//ミッション進行にかかわる性質
	DeleteCompMacro(InvationObservance);
	DeleteCompMacro(UnitCountObservance);
	DeleteCompMacro(Spawn);
	//テスト用
	DeleteCompMacro(Test1);
	DeleteCompMacro(Test2);
	DeleteCompMacro(TestResult);
	DeleteCompMacro(Test3);
}
SameArchetype::SameArchetype(Interface::RawArchetype thisArchetype, bool IsPrototype) {
	PrototypeCount = 0;
	ThisArchetype = thisArchetype;
	if (!IsPrototype) {
		Prototypes = new SameArchetype(thisArchetype, true);
	}
	IdArray = std::vector<Interface::EntId>();
	ValidEntityCount = 0;
	RealEntityCount = 0;

	//共通するデータ
	InitCompWrapper(HitFlag);
	InitCompWrapper(KillFlag);
	InitCompWrapper(GenerateFlag);
	InitCompWrapper(MoveFlag);
	InitCompWrapper(AppearanceChanged);
	InitCompWrapper(PositionReference);
	//AIに関する性質
	InitCompWrapper(AI);
	//当たり判定にかかわる性質
	InitCompWrapper(CircleHitbox);
	InitCompWrapper(BallHurtbox);
	InitCompWrapper(UnitOccupationbox);
	InitCompWrapper(WallHurtbox);
	InitCompWrapper(GiveDamage);
	InitCompWrapper(DamagePool);
	//内部にかかわる性質
	InitCompWrapper(BallData);
	InitCompWrapper(UnitData);
	InitCompWrapper(BulletData);
	//他のオブジェクトの生成にかかわる性質
	InitCompWrapper(HitEffect);
	InitCompWrapper(Trajectory);
	//見た目にかかわる性質
	InitCompWrapper(BlockAppearance);
	InitCompWrapper(BallAppearance);
	InitCompWrapper(BulletAppearance);
	//運動にかかわる性質
	InitCompWrapper(Motion);
	InitCompWrapper(LinearAcceralation);
	InitCompWrapper(WorldPosition);
	//ミッション進行にかかわる性質
	InitCompWrapper(InvationObservance);
	InitCompWrapper(UnitCountObservance);
	InitCompWrapper(Spawn);
	//テスト用
	InitCompWrapper(Test1);
	InitCompWrapper(Test2);
	InitCompWrapper(TestResult);
	InitCompWrapper(Test3);
}
void SameArchetype::LoadToPrototype(Json::Value oneObject) {
	//共通するデータ
	InitPrototypeCompWrapper(HitFlag);
	InitPrototypeCompWrapper(KillFlag);
	InitPrototypeCompWrapper(GenerateFlag);
	InitPrototypeCompWrapper(MoveFlag);
	InitPrototypeCompWrapper(AppearanceChanged);
	InitPrototypeCompWrapper(PositionReference);
	//AIに関する性質
	InitPrototypeCompWrapper(AI);
	//当たり判定にかかわる性質
	InitPrototypeCompWrapper(CircleHitbox);
	InitPrototypeCompWrapper(BallHurtbox);
	InitPrototypeCompWrapper(UnitOccupationbox);
	InitPrototypeCompWrapper(WallHurtbox);

	InitPrototypeCompWrapper(GiveDamage);
	InitPrototypeCompWrapper(DamagePool);
	//内部にかかわる性質
	InitPrototypeCompWrapper(BallData);
	InitPrototypeCompWrapper(UnitData);
	InitPrototypeCompWrapper(BulletData);
	//他のオブジェクトの生成にかかわる性質
	InitPrototypeCompWrapper(HitEffect);
	InitPrototypeCompWrapper(Trajectory);
	//見た目にかかわる性質
	InitPrototypeCompWrapper(BlockAppearance);
	InitPrototypeCompWrapper(BallAppearance);
	InitPrototypeCompWrapper(BulletAppearance);
	//運動にかかわる性質
	InitPrototypeCompWrapper(Motion);
	InitPrototypeCompWrapper(LinearAcceralation);
	InitPrototypeCompWrapper(WorldPosition);
	//ミッション進行にかかわる性質
	InitPrototypeCompWrapper(InvationObservance);
	InitPrototypeCompWrapper(UnitCountObservance);
	InitPrototypeCompWrapper(Spawn);
	//テスト用
	InitPrototypeCompWrapper(Test1);
	InitPrototypeCompWrapper(Test2);
	InitPrototypeCompWrapper(TestResult);
	InitPrototypeCompWrapper(Test3);
}
void AllEntities::LoadEntities(std::vector<std::string> fileNames) {
	//まず全エンティティの名前とプロトタイプのEntPointerを紐づける
	for (int i = 0; i < fileNames.size(); i++) {
		Json::Value root;
		std::ifstream file = std::ifstream(fileNames[i]);
		file >> root;
		file.close();
		//エンティティ名の一覧を取得
		Json::Value::Members entNames = root.getMemberNames();
		for (int j = 0; j < entNames.size(); j++) {
			Interface::EntityPointer prototypePointer;
			//コンポーネント名の一覧を得てアーキタイプに変換し、既に登録済みのアーキタイプか調べる
			Json::Value::Members compNames = root.get(entNames[j], "").getMemberNames();
			Interface::RawArchetype rawArchetype = GetArchetypeFromCompNames(compNames);
			auto itr = std::find(Archetypes.begin(), Archetypes.end(), rawArchetype);

			if (itr == Archetypes.end()) {
				//未登録
				Archetypes.push_back(rawArchetype);
				EntityArraies.push_back(SameArchetype(rawArchetype, false));
				prototypePointer.Archetype = Archetypes.size() - 1;
				prototypePointer.Index = 0;
				EntityArraies.back().PrototypeCount++;
			}
			else {
				//登録済み
				prototypePointer.Archetype = std::distance(Archetypes.begin(), itr);
				prototypePointer.Index = EntityArraies[prototypePointer.Archetype].PrototypeCount;
				EntityArraies[prototypePointer.Archetype].PrototypeCount++;
			}
			std::string name = entNames[j];
			bool inserted = Interface::EntNameHash.emplace(std::pair<std::string, Interface::EntityPointer>(name, prototypePointer)).second;
		}
	}
	//次に実際のデータを読み込んでいく
	for (int i = 0; i < fileNames.size(); i++) {
		Json::Value root;
		std::ifstream file = std::ifstream(fileNames[i]);
		file >> root;
		file.close();
		//エンティティ名の一覧を取得
		Json::Value::Members entNames = root.getMemberNames();
		for (int j = 0; j < entNames.size(); j++) {
			Interface::EntityPointer prototypePointer = Interface::EntNameHash[entNames[j]];
			EntityArraies[prototypePointer.Archetype].LoadToPrototype(root.get(entNames[j], ""));
		}
	}
}
void AllEntities::LoadUnits(std::vector<std::string> filePathes) {
	for (int i = 0; i < filePathes.size(); i++) {
		Json::Value root;
		std::ifstream file = std::ifstream(filePathes[i]);
		file >> root;
		file.close();
		auto unitNames = root.getMemberNames();
		for (int i = 0; i < unitNames.size(); i++) {
			UnitInfos.push_back(UnitInfo(root.get(unitNames[i], ""),this));
			Interface::UnitNameHash.emplace(std::pair<std::string, int>(unitNames[i], i));
		}
	}
}
Interface::EntId AllEntities::AddUnitWithMagnification(int unitIndex, Interface::EntityInitData* pInitData, Interface::RelationOfCoord* CorePos) {
	Interface::EntId core;
	pInitData->Pos = *CorePos;
	pInitData->Prototype = UnitInfos[unitIndex].CorePrototype;

	pInitData->IsCore = true;
	// 初期のサイズの指定
	pInitData->Ratio = UnitInfos[unitIndex].Ratio;

	core = AddFromEntPointer(pInitData);
	pInitData->CoreId = core;
	pInitData->IsCore = false;
	//1つのユニットには7このボールを置く
	for (int i = 0; i < 7; i++) {
		pInitData->BaseColor0 = UnitInfos[unitIndex].BaseColor0[i];
		pInitData->BaseColor1 = UnitInfos[unitIndex].BaseColor1[i];
		pInitData->Pos = UnitInfos[unitIndex].RelativePosFromCore[i];
		pInitData->Prototype = UnitInfos[unitIndex].PrototypePointer[i];
		AddFromEntPointer(pInitData);
	}
	return core;
}
void AllEntities::LoadMission(std::string fileName) {
	Json::Value root;
	std::ifstream file = std::ifstream(fileName);
	file >> root;
	file.close();
	Json::Value EnemySpawnCondition = root.get("enemySpawnCondition", "");
	// エリア侵入によってスポーン
	{
		Interface::RawArchetype rawArchetype;
		rawArchetype.reset();
		rawArchetype.set(CompNames::Spawn);
		rawArchetype.set(CompNames::InvationObservance);
		rawArchetype.set(CompNames::GenerateFlag);
		rawArchetype.set(CompNames::KillFlag);
		Archetypes.push_back(rawArchetype);
		EntityArraies.push_back(SameArchetype(rawArchetype, true));
		for (int i = 0; i < EnemySpawnCondition.size(); i++) {
			if (Interface::SameString(EnemySpawnCondition[i].get("conditionType", "").asString(), "areaInvation")) {
				EntityArraies.back().Spawn.Add(root.get("enemySpawnCondition","")[i], "");
				EntityArraies.back().InvationObservance.Add(root.get("enemySpawnCondition", "")[i], "");
			}
		}
	}
	// ユニット残数によってスポーン
	{
		Interface::RawArchetype rawArchetype;
		rawArchetype.reset();
		rawArchetype.set(CompNames::Spawn);
		rawArchetype.set(CompNames::UnitCountObservance);
		rawArchetype.set(CompNames::GenerateFlag);
		rawArchetype.set(CompNames::KillFlag);
		Archetypes.push_back(rawArchetype);
		EntityArraies.push_back(SameArchetype(rawArchetype, true));
		for (int i = 0; i < EnemySpawnCondition.size(); i++) {
			if (Interface::SameString(EnemySpawnCondition[i].get("conditionType", "").asString(), "unitCount")) {
				EntityArraies.back().Spawn.Add(root.get("enemySpawnCondition", "")[i], "");
				EntityArraies.back().UnitCountObservance.Add(root.get("enemySpawnCondition", "")[i], "");
				EntityArraies.back().GenerateFlag.Add(root.get("enemySpawnCondition", "")[i], "");
				EntityArraies.back().KillFlag.Add(root.get("enemySpawnCondition", "")[i], "");
				EntityArraies.back().ValidEntityCount++;
			}
		}
	}
}