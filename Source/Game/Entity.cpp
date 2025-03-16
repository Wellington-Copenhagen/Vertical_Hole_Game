#include "Entity.h"
extern GraphicalStringDraw<65536, 2048, 32> globalStringDraw;
//地面と壁は別オブジェクトとする
void Entities::LoadMap(std::string mapFileName)
{
	//マップデータの読み込み
	Json::Value root;
	std::ifstream file = std::ifstream(mapFileName);
	file >> root;
	file.close();
	AreaIndexMap = std::vector<int>(WorldWidth * WorldHeight);
	for (int i = 0; i < WorldWidth * WorldHeight; i++) {
		AreaIndexMap[i] = -1;
	}
	IsWallMap.reset();
	ShouldUpdateShadow.set();
	// 各部屋に対応した壁と床の素材
	std::vector<entt::entity> wallMaterial = std::vector<entt::entity>();
	std::vector<entt::entity> floorMaterial = std::vector<entt::entity>();
	int roomIndex = 0;
	for (Json::Value area : root.get("areaList", "")) {
		wallMaterial.push_back(Interface::EntNameHash[area.get("wallEntity", "").asString()]);
		floorMaterial.push_back(Interface::EntNameHash[area.get("floorEntity", "").asString()]);
		int top = area.get("top", "").asInt();
		int bottom = area.get("bottom", "").asInt();
		int left = area.get("left", "").asInt();
		int right = area.get("right", "").asInt();
		mRouting.AddArea(ConvexArea(bottom, top, left, right));
		for (int x = left; x < right; x++) {
			for (int y = bottom; y < top; y++) {
				AreaIndexMap[y * WorldWidth + x] = roomIndex;
			}
		}
		roomIndex++;
	}
	mRouting.FormRouting();
	for (int x = 0; x < WorldWidth; x++) {
		for (int y = 0; y < WorldHeight; y++) {
			if (AreaIndexMap[y * WorldWidth + x] != -1) {
				//エリア内の場合
				Interface::EntityInitData init;
				init.Pos.Identity();
				init.Pos.Parallel = { (float)x,(float)y,0.0f,1.0f };
				init.OnTop = false;
				init.Prototype = floorMaterial[AreaIndexMap[y * WorldWidth + x]];
				EmplaceFromPrototypeEntity<false>(&init);
				DirectX::XMVECTOR pos = { x,y,0,1 };
				DirectX::XMVECTOR color = { 0,0,0,1 };
				std::string content = std::to_string(AreaIndexMap[y * WorldWidth + x]);
				globalStringDraw.Append(content, &color, &pos, 0.5, 60000, StrDrawPos::AsCenter);
			}
			else {
				int nearByArea = -1;
				//エリア外の場合
				// 対角含めた隣のうち最もインデックスが小さいエリアを探す
				for (int i = 0; i < 9; i++) {
					int observeX = x + i / 3 - 1;
					int observeY = y + i % 3 - 1;
					if (observeX >= 0 && observeX < WorldWidth && observeY >= 0 && observeY < WorldHeight) {
						if (nearByArea == -1) {
							nearByArea = AreaIndexMap[observeY * WorldWidth + observeX];
						}
						else if(AreaIndexMap[observeY * WorldWidth + observeX]!=-1) {
							nearByArea = min(nearByArea, AreaIndexMap[observeY * WorldWidth + observeX]);
						}
					}
				}
				if (nearByArea != -1) {
					IsWallMap[y * WorldWidth + x] = true;
					Interface::EntityInitData init;
					init.Pos.Identity();
					init.Pos.Parallel = { (float)x,(float)y,0.0f,1.0f };
					//床
					init.OnTop = false;
					init.Prototype = floorMaterial[nearByArea];
					EmplaceFromPrototypeEntity<false>(&init);
					//壁
					init.OnTop = true;
					init.Prototype = wallMaterial[nearByArea];
					EmplaceFromPrototypeEntity<false>(&init);
				}
				//AreaIndexMap[y * WorldWidth + x] = nearByArea;
			}
		}
	}
}
void Entities::LoadEntities(std::vector<std::string> fileNames) {
	for (std::string fileName : fileNames) {
		Json::Value root;
		std::ifstream file = std::ifstream(fileName);
		file >> root;
		file.close();
		std::vector<std::string> entNames = root.getMemberNames();
		for (std::string entName : entNames) {
			Interface::EntNameHash[entName] = EmplaceFromJson<true>(root.get(entName, ""));
		}
	}
}
void Entities::LoadUnits(std::vector<std::string> fileNames) {
	for (std::string fileName : fileNames) {
		Json::Value root;
		std::ifstream file = std::ifstream(fileName);
		file >> root;
		file.close();
		std::vector<std::string> entNames = root.getMemberNames();
		for (std::string entName : entNames) {
			UnitInfos.push_back(Interface::UnitInfo(root.get(entName, ""), &Registry));
			Interface::UnitNameHash[entName] = UnitInfos.size() - 1;
		}
	}
}
void Entities::LoadMission(std::string missionFileName) {
	Json::Value root;
	std::ifstream file = std::ifstream(missionFileName);
	file >> root;
	file.close();
	LoadMap(root.get("mapFilePath", "").asString());
	for (Json::Value spawnCondition : root.get("spawnCondition", "")) {
		EmplaceFromJson<false>(spawnCondition);
	}
}
template<typename typeComp, bool asPrototype>
void Entities::EmplaceEntityIfNeeded(entt::entity entity, Json::Value onePrototypeEntityData) {
	if (asPrototype) {
		auto names = onePrototypeEntityData.getMemberNames();
		for (auto name : names) {
			std::string className = typeid(typeComp).name();
			className = className.substr(18);
			if (Interface::SameString(name, className)) {
				PrototypeRegistry.emplace<typeComp>(entity, typeComp(onePrototypeEntityData.get(name, "")));
			}
		}
	}
	else {
		auto names = onePrototypeEntityData.getMemberNames();
		for (auto name : names) {
			std::string className = typeid(typeComp).name();
			className = className.substr(18);
			if (Interface::SameString(name, className)) {
				Registry.emplace<typeComp>(entity, typeComp(onePrototypeEntityData.get(name, "")));
			}
		}
	}
}
template<typename typeComp, bool asPrototype>
void Entities::EmplaceEntityIfNeeded(entt::entity entity, Interface::EntityInitData* pInitData) {

	if (asPrototype) {
		if (PrototypeRegistry.any_of<typeComp>(pInitData->Prototype)) {
			PrototypeRegistry.emplace<typeComp>(entity, typeComp(PrototypeRegistry.get<typeComp>(pInitData->Prototype)));
			PrototypeRegistry.get<typeComp>(entity).Init(pInitData);
		}
	}
	else {
		if (PrototypeRegistry.any_of<typeComp>(pInitData->Prototype)) {
			Registry.emplace<typeComp>(entity, typeComp(PrototypeRegistry.get<typeComp>(pInitData->Prototype)));
			Registry.get<typeComp>(entity).Init(pInitData);
		}
	}
}
template<typename typeComp,bool asPrototype>
void Entities::EmplaceEntity(entt::entity entity, Interface::EntityInitData* pInitData) {
	if (asPrototype) {
		PrototypeRegistry.emplace<typeComp>(entity, typeComp());
		PrototypeRegistry.get<typeComp>(entity).Init(pInitData);
	}
	else {
		Registry.emplace<typeComp>(entity, typeComp());
		Registry.get<typeComp>(entity).Init(pInitData);
	}
}
using namespace Component;
template<bool asPrototype>
entt::entity Entities::EmplaceFromJson(Json::Value onePrototypeEntityData) {
	entt::entity newEntity;
	if (asPrototype) {
		newEntity = PrototypeRegistry.create();
	}
	else {
		newEntity = Registry.create();
	}
	// プロトタイプでは常に存在するコンポーネントは作らない
	EmplaceEntityIfNeeded<WorldPosition,asPrototype>(newEntity, onePrototypeEntityData);
	EmplaceEntityIfNeeded<LinearAcceralation,asPrototype>(newEntity, onePrototypeEntityData);
	EmplaceEntityIfNeeded<Motion,asPrototype>(newEntity, onePrototypeEntityData);
	EmplaceEntityIfNeeded<BlockAppearance,asPrototype>(newEntity, onePrototypeEntityData);
	EmplaceEntityIfNeeded<BallAppearance,asPrototype>(newEntity, onePrototypeEntityData);
	EmplaceEntityIfNeeded<BulletAppearance,asPrototype>(newEntity, onePrototypeEntityData);
	EmplaceEntityIfNeeded<CircleHitbox,asPrototype>(newEntity, onePrototypeEntityData);
	EmplaceEntityIfNeeded<BallHurtbox,asPrototype>(newEntity, onePrototypeEntityData);
	EmplaceEntityIfNeeded<UnitOccupationbox,asPrototype>(newEntity, onePrototypeEntityData);
	EmplaceEntityIfNeeded<WallHurtbox,asPrototype>(newEntity, onePrototypeEntityData);
	EmplaceEntityIfNeeded<UnitData,asPrototype>(newEntity, onePrototypeEntityData);
	EmplaceEntityIfNeeded<BallData,asPrototype>(newEntity, onePrototypeEntityData);
	EmplaceEntityIfNeeded<BulletData,asPrototype>(newEntity, onePrototypeEntityData);
	EmplaceEntityIfNeeded<BlockData,asPrototype>(newEntity, onePrototypeEntityData);
	EmplaceEntityIfNeeded<GiveDamage,asPrototype>(newEntity, onePrototypeEntityData);
	EmplaceEntityIfNeeded<DamagePool,asPrototype>(newEntity, onePrototypeEntityData);
	EmplaceEntityIfNeeded<HitEffect,asPrototype>(newEntity, onePrototypeEntityData);
	EmplaceEntityIfNeeded<Trajectory,asPrototype>(newEntity, onePrototypeEntityData);
	EmplaceEntityIfNeeded<InvationObservance,asPrototype>(newEntity, onePrototypeEntityData);
	EmplaceEntityIfNeeded<UnitCountObservance,asPrototype>(newEntity, onePrototypeEntityData);
	EmplaceEntityIfNeeded<Spawn,asPrototype>(newEntity, onePrototypeEntityData);
	return newEntity;
}
template<bool asPrototype>
entt::entity Entities::EmplaceFromPrototypeEntity(Interface::EntityInitData* pInitData) {
	entt::entity newEntity;
	if (asPrototype) {
		newEntity = PrototypeRegistry.create();
	}
	else {
		newEntity = Registry.create();
	}

	EmplaceEntity<KillFlag,asPrototype>(newEntity, pInitData);
	EmplaceEntity<GenerateFlag,asPrototype>(newEntity, pInitData);
	EmplaceEntity<MoveFlag,asPrototype>(newEntity, pInitData);
	EmplaceEntity<HitFlag,asPrototype>(newEntity, pInitData);
	EmplaceEntity<AppearanceChanged,asPrototype>(newEntity, pInitData);
	EmplaceEntity<PositionReference,asPrototype>(newEntity, pInitData);


	EmplaceEntityIfNeeded<WorldPosition,asPrototype>(newEntity, pInitData);
	EmplaceEntityIfNeeded<LinearAcceralation,asPrototype>(newEntity, pInitData);
	EmplaceEntityIfNeeded<Motion,asPrototype>(newEntity, pInitData);
	EmplaceEntityIfNeeded<BlockAppearance,asPrototype>(newEntity, pInitData);
	EmplaceEntityIfNeeded<BallAppearance,asPrototype>(newEntity, pInitData);
	EmplaceEntityIfNeeded<BulletAppearance,asPrototype>(newEntity, pInitData);
	EmplaceEntityIfNeeded<CircleHitbox,asPrototype>(newEntity, pInitData);
	EmplaceEntityIfNeeded<BallHurtbox,asPrototype>(newEntity, pInitData);
	EmplaceEntityIfNeeded<UnitOccupationbox,asPrototype>(newEntity, pInitData);
	EmplaceEntityIfNeeded<WallHurtbox,asPrototype>(newEntity, pInitData);
	EmplaceEntityIfNeeded<UnitData,asPrototype>(newEntity, pInitData);
	EmplaceEntityIfNeeded<BallData,asPrototype>(newEntity, pInitData);
	EmplaceEntityIfNeeded<BulletData,asPrototype>(newEntity, pInitData);
	EmplaceEntityIfNeeded<BlockData,asPrototype>(newEntity, pInitData);
	EmplaceEntityIfNeeded<GiveDamage,asPrototype>(newEntity, pInitData);
	EmplaceEntityIfNeeded<DamagePool,asPrototype>(newEntity, pInitData);
	EmplaceEntityIfNeeded<HitEffect,asPrototype>(newEntity, pInitData);
	EmplaceEntityIfNeeded<Trajectory,asPrototype>(newEntity, pInitData);
	EmplaceEntityIfNeeded<InvationObservance,asPrototype>(newEntity, pInitData);
	EmplaceEntityIfNeeded<UnitCountObservance,asPrototype>(newEntity, pInitData);
	EmplaceEntityIfNeeded<Spawn,asPrototype>(newEntity, pInitData);
	return newEntity;
}
entt::entity Entities::EmplaceUnitWithInitData(int unitIndex, Interface::EntityInitData* pInitData, Interface::RelationOfCoord* pCorePos) {
	entt::entity core;
	pInitData->Pos = *pCorePos;
	pInitData->Prototype = UnitInfos[unitIndex].CorePrototype;

	pInitData->IsCore = true;
	// 初期のサイズの指定
	pInitData->Ratio = UnitInfos[unitIndex].Ratio;

	core = EmplaceFromPrototypeEntity<false>(pInitData);
	pInitData->CoreId = core;
	pInitData->IsCore = false;
	//1つのユニットには7このボールを置く
	for (int i = 0; i < 7; i++) {
		pInitData->BaseColor0 = UnitInfos[unitIndex].BaseColor0[i];
		pInitData->BaseColor1 = UnitInfos[unitIndex].BaseColor1[i];
		pInitData->Pos = UnitInfos[unitIndex].RelativePosFromCore[i];
		pInitData->Prototype = UnitInfos[unitIndex].Prototypes[i];
		EmplaceFromPrototypeEntity<false>(pInitData);
	}
	return core;
}
int Entities::GetFloorShadow(int x, int y) {
	int maskIndex = 0;
	if (x != 0 && IsWallMap[y * WorldWidth + x - 1]) {
		maskIndex += 1;
	}
	if (x != WorldWidth - 1 && IsWallMap[y * WorldWidth + x + 1]) {
		maskIndex += 2;
	}
	if (y != 0 && IsWallMap[(y - 1) * WorldWidth + x]) {
		maskIndex += 4;
	}
	if (y != WorldHeight - 1 && IsWallMap[(y + 1) * WorldWidth + x]) {
		maskIndex += 8;
	}
	return maskIndex;
}
int Entities::GetWallShadow(int x,int y) {
	int maskIndex = 0;
	if (x != 0 && !IsWallMap[y * WorldWidth + x - 1]) {
		maskIndex += 1;
	}
	if (x != WorldWidth - 1 && !IsWallMap[y * WorldWidth + x + 1]) {
		maskIndex += 2;
	}
	if (y != 0 && !IsWallMap[(y - 1) * WorldWidth + x]) {
		maskIndex += 4;
	}
	return maskIndex;
}
void Entities::DeleteWall(int x, int y) {
	IsWallMap[y * WorldWidth + x] = false;
	if (x > 0) {
		ShouldUpdateShadow[y * WorldWidth + (x - 1)] = true;
	}
	if (x < WorldWidth - 1) {
		ShouldUpdateShadow[y * WorldWidth + (x + 1)] = true;
	}
	if (y > 0) {
		ShouldUpdateShadow[(y - 1) * WorldWidth + x] = true;
	}
	if (y > WorldHeight - 1) {
		ShouldUpdateShadow[(y + 1) * WorldWidth + x] = true;
	}
}
int Entities::GetAreaIndex(DirectX::XMVECTOR pos) {
	int x = (int)roundf(pos.m128_f32[0]);
	int y = (int)roundf(pos.m128_f32[1]);
	return AreaIndexMap[y * WorldWidth + x];
}