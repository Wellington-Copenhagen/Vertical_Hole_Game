#include "Entity.h"
#define AddCompMacro(CompName) CompName.Add(Prototypes->CompName, prototypeIndex)
#define DeleteCompMacro(CompName) CompName.Delete(toDelete)
#define InitCompWrapper(CompName) CompName = ComponentWrapper<Component::CompName>(thisArchetype, CompNames::CompName)
#define InitPrototypeCompWrapper(CompName) Prototypes->CompName.Add(oneObject, CompNameArray[CompNames::CompName])

// coreWorldはcoreである場合はcoreの位置、coreでない場合はcoreとの位置関係
Interface::SameArchIndex SameArchetype::Add(Interface::SameArchIndex prototypeIndex, Interface::EntId nextId,
	Interface::CombatUnitInitData* pCombatUnitInit,Interface::RelationOfCoord* coreWorld,int initialMaskCoord,
	bool isCore,Interface::EntId positionReference) {
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
	AddCompMacro(CharacterHurtbox);
	AddCompMacro(WallHurtbox);
	AddCompMacro(GiveDamage);
	AddCompMacro(DamagePool);
	//内部にかかわる性質
	AddCompMacro(CharacterData);
	AddCompMacro(BulletData);
	//他のオブジェクトの生成にかかわる性質
	AddCompMacro(HitEffect);
	AddCompMacro(Trajectory);
	//見た目にかかわる性質
	AddCompMacro(BlockAppearance);
	AddCompMacro(CharacterAppearance);
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
	if (GenerateFlag.UsedFlag) {
		GenerateFlag.Components.back().Init();
	}
	if (KillFlag.UsedFlag) {
		KillFlag.Components.back().Init();
	}
	if (MoveFlag.UsedFlag) {
		MoveFlag.Components.back().Init();
	}
	if (AppearanceChanged.UsedFlag) {
		AppearanceChanged.Components.back().Init();
	}
	if (HitFlag.UsedFlag) {
		HitFlag.Components.back().Init();
	}
	if (CharacterData.UsedFlag&& pCombatUnitInit != nullptr) {
		CharacterData.Components.back().Init(pCombatUnitInit);
	}
	if (WorldPosition.UsedFlag && coreWorld != nullptr) {
		WorldPosition.Components.back().Init(coreWorld,isCore);
	}
	if (PositionReference.UsedFlag) {
		if (isCore) {
			PositionReference.Components.back().Init(isCore, IdArray.back());
		}
		else {
			PositionReference.Components.back().Init(isCore, positionReference);
		}
	}
	if (BlockAppearance.UsedFlag) {
		BlockAppearance.Components.back().Init(initialMaskCoord);
	}
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
	DeleteCompMacro(CharacterHurtbox);
	DeleteCompMacro(WallHurtbox);
	DeleteCompMacro(GiveDamage);
	DeleteCompMacro(DamagePool);
	//内部にかかわる性質
	DeleteCompMacro(CharacterData);
	DeleteCompMacro(BulletData);
	//他のオブジェクトの生成にかかわる性質
	DeleteCompMacro(HitEffect);
	DeleteCompMacro(Trajectory);
	//見た目にかかわる性質
	DeleteCompMacro(BlockAppearance);
	DeleteCompMacro(CharacterAppearance);
	DeleteCompMacro(BulletAppearance);
	//運動にかかわる性質
	DeleteCompMacro(Motion);
	DeleteCompMacro(LinearAcceralation);
	DeleteCompMacro(WorldPosition);
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
	InitCompWrapper(CharacterHurtbox);
	InitCompWrapper(WallHurtbox);
	InitCompWrapper(GiveDamage);
	InitCompWrapper(DamagePool);
	//内部にかかわる性質
	InitCompWrapper(CharacterData);
	InitCompWrapper(BulletData);
	//他のオブジェクトの生成にかかわる性質
	InitCompWrapper(HitEffect);
	InitCompWrapper(Trajectory);
	//見た目にかかわる性質
	InitCompWrapper(BlockAppearance);
	InitCompWrapper(CharacterAppearance);
	InitCompWrapper(BulletAppearance);
	//運動にかかわる性質
	InitCompWrapper(Motion);
	InitCompWrapper(LinearAcceralation);
	InitCompWrapper(WorldPosition);
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
	InitPrototypeCompWrapper(CharacterHurtbox);
	InitPrototypeCompWrapper(WallHurtbox);
	InitPrototypeCompWrapper(GiveDamage);
	InitPrototypeCompWrapper(DamagePool);
	//内部にかかわる性質
	InitPrototypeCompWrapper(CharacterData);
	InitPrototypeCompWrapper(BulletData);
	//他のオブジェクトの生成にかかわる性質
	InitPrototypeCompWrapper(HitEffect);
	InitPrototypeCompWrapper(Trajectory);
	//見た目にかかわる性質
	InitPrototypeCompWrapper(BlockAppearance);
	InitPrototypeCompWrapper(CharacterAppearance);
	InitPrototypeCompWrapper(BulletAppearance);
	//運動にかかわる性質
	InitPrototypeCompWrapper(Motion);
	InitPrototypeCompWrapper(LinearAcceralation);
	InitPrototypeCompWrapper(WorldPosition);
	//テスト用
	InitPrototypeCompWrapper(Test1);
	InitPrototypeCompWrapper(Test2);
	InitPrototypeCompWrapper(TestResult);
	InitPrototypeCompWrapper(Test3);
}
std::map<std::string, Interface::EntityPointer> AllEntities::LoadFromFile(std::vector<std::string> fileNames) {
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
			int a = Interface::EntNameHash.size();
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
	return Interface::EntNameHash;
}