#include "Entity.h"
extern GraphicalStringDraw globalStringDraw;
//地面と壁は別オブジェクトとする
void Entities::LoadMap(std::string mapFileName)
{
	//マップデータの読み込み
	Json::Value root;
	std::ifstream file = std::ifstream(mapFileName);
	file >> root;
	file.close();

	PlayerCorpsSpawnArea = {
		root.get("playerCorpsSpawnArea","").get("left","").asInt(),
		root.get("playerCorpsSpawnArea","").get("right","").asInt(),
		root.get("playerCorpsSpawnArea","").get("bottom","").asInt(),
		root.get("playerCorpsSpawnArea","").get("top","").asInt()
	};


	mRouting.AreaIndexMap = std::vector<int>(WorldWidth * WorldHeight,-1);
	mRouting.NearestWalkableArea = std::vector<std::tuple<int,int>>(WorldWidth * WorldHeight);
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
				mRouting.AreaIndexMap[y * WorldWidth + x] = roomIndex;
			}
		}
		roomIndex++;
	}
	mRouting.FormRouting();
	for (int x = 0; x < WorldWidth; x++) {
		for (int y = 0; y < WorldHeight; y++) {
			if (mRouting.AreaIndexMap[y * WorldWidth + x] != -1) {
				//エリア内の場合
				Interface::EntityInitData init;
				init.Pos.Identity();
				init.Pos.Parallel = { (float)x,(float)y,0.0f,1.0f };
				init.OnTop = false;
				init.Prototype = floorMaterial[mRouting.AreaIndexMap[y * WorldWidth + x]];
				EmplaceFromPrototypeEntity<false>(&init);
				mRouting.NearestWalkableArea[y * WorldWidth + x] = { x,y };
				if (x % 6 == 3 && y%6==3) {
					std::string content = std::to_string(mRouting.AreaIndexMap[y * WorldWidth + x]);
					globalStringDraw.SimpleAppend(content, 0, 0, 0, { (float)x,(float)y,0.0f,1.0f }, 0.5, 60000, StrDrawPos::AsCenter);
				}
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
							nearByArea = mRouting.AreaIndexMap[observeY * WorldWidth + observeX];
						}
						else if(mRouting.AreaIndexMap[observeY * WorldWidth + observeX]!=-1) {
							nearByArea = min(nearByArea, mRouting.AreaIndexMap[observeY * WorldWidth + observeX]);
						}
					}
				}
				// 最も近い歩ける場所を探す
				// エリア外にはみ出した時の誘導に使いたい
				std::tuple<int,int> nearest;
				float nearestDistance = 10000000;
				for (Json::Value area : root.get("areaList", "")) {
					int top = area.get("top", "").asInt();
					int bottom = area.get("bottom", "").asInt();
					int left = area.get("left", "").asInt();
					int right = area.get("right", "").asInt();
					float distance = 0;
					std::tuple<int, int> pos;
					if (bottom <= y && y < top) {
						std::get<1>(pos) = y;
					}
					else if(y<bottom){
						distance += powf(bottom - y, 2);
						std::get<1>(pos) = bottom;
					}
					else {
						distance += powf(y - (top - 1), 2);
						std::get<1>(pos) = top - 1;
					}
					if (left <= x && x < right) {
						std::get<0>(pos) = x;
					}
					else if (x < left) {
						distance += powf(left - x, 2);
						std::get<0>(pos) = left;
					}
					else {
						distance += powf(x - (right - 1), 2);
						std::get<0>(pos) = right - 1;
					}
					distance = sqrtf(distance);
					if (distance < nearestDistance) {
						nearestDistance = distance;
						nearest = pos;
					}
				}
				mRouting.NearestWalkableArea[y * WorldWidth + x] = nearest;
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
			}
			if (x % 6 == 0 && y % 6 == 0) {
				std::string content = "(" + std::to_string(std::get<0>(mRouting.NearestWalkableArea[y * WorldWidth + x])) + "," + std::to_string(std::get<1>(mRouting.NearestWalkableArea[y * WorldWidth + x])) + ")";
				globalStringDraw.SimpleAppend(content, 1, 1, 0, { (float)x,(float)y,0.0f,1.0f }, 0.5, 60000, StrDrawPos::AsCenter);
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
void Entities::LoadPlayerCorps(std::string fileName) {
	Json::Value root;
	std::ifstream file = std::ifstream(fileName);
	file >> root;
	file.close();
	int i = 0;
	for (Json::Value oneCorps : root.get("corps", "")) {
		float left = std::get<0>(PlayerCorpsSpawnArea);
		float right = std::get<1>(PlayerCorpsSpawnArea);
		float bottom = std::get<2>(PlayerCorpsSpawnArea);
		float top = std::get<3>(PlayerCorpsSpawnArea);
		// 部隊長のスポーン
		Interface::UnitIndex leader = Interface::UnitNameHash[oneCorps.get("leaderUnit","").asString()];
		float Xpos = Interface::UniformRandInt(left, right);
		float Ypos = Interface::UniformRandInt(bottom, top);
		Interface::RelationOfCoord pos = Interface::RelationOfCoord();
		pos.Parallel = {
			(float)Xpos,(float)Ypos,0,1
		};
		pos.Parallel = mRouting.NearestWalkablePosition(pos.Parallel);
		Interface::EntityInitData initData;
		initData.Team = 0;
		initData.DamageMultiply = 1;
		initData.HealthMultiply = 1;
		initData.SpeedMultiply = 1;
		initData.IsLeader = true;
		initData.LeaderId = EmplaceUnitWithInitData(leader, &initData, &pos);
		if (i == 0) {
			PlayingUnit = initData.LeaderId;
		}

		// 部下のスポーン
		initData.IsLeader = false;
		Interface::UnitIndex member = Interface::UnitNameHash[oneCorps.get("memberUnit", "").asString()];
		for (int i = 0; i < oneCorps.get("count", "").asInt();i++) {
			Xpos = Interface::UniformRandInt(left, right);
			Ypos = Interface::UniformRandInt(bottom, top);
			Interface::RelationOfCoord pos = Interface::RelationOfCoord();
			pos.Parallel = {
				(float)Xpos,(float)Ypos,0,1
			};
			pos.Parallel = mRouting.NearestWalkablePosition(pos.Parallel);
			EmplaceUnitWithInitData(member, &initData, &pos);
		}
		i++;
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
	for (Json::Value hostilityPair : root.get("hostility", "")) {
		int other = hostilityPair.get("other", "").asInt();
		int another = hostilityPair.get("another", "").asInt();
		Interface::HostilityTable[other * TeamCount + another] = true;
		Interface::HostilityTable[another * TeamCount + other] = true;
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
	EmplaceEntityIfNeeded<EffectAppearance, asPrototype>(newEntity, onePrototypeEntityData);
	EmplaceEntityIfNeeded<CircleHitbox,asPrototype>(newEntity, onePrototypeEntityData);
	EmplaceEntityIfNeeded<BallHurtbox,asPrototype>(newEntity, onePrototypeEntityData);
	EmplaceEntityIfNeeded<UnitOccupationbox,asPrototype>(newEntity, onePrototypeEntityData);
	EmplaceEntityIfNeeded<WallHurtbox,asPrototype>(newEntity, onePrototypeEntityData);
	EmplaceEntityIfNeeded<CorpsData, asPrototype>(newEntity, onePrototypeEntityData);
	EmplaceEntityIfNeeded<UnitData,asPrototype>(newEntity, onePrototypeEntityData);
	EmplaceEntityIfNeeded<BallData,asPrototype>(newEntity, onePrototypeEntityData);
	EmplaceEntityIfNeeded<Attack, asPrototype>(newEntity, onePrototypeEntityData);
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
	// 2^20個飛ぶ場合がある？
	// 再利用が行われたときの挙動のよう
	pInitData->thisEntities = newEntity;
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
	EmplaceEntityIfNeeded<EffectAppearance, asPrototype>(newEntity, pInitData);
	EmplaceEntityIfNeeded<CircleHitbox,asPrototype>(newEntity, pInitData);
	EmplaceEntityIfNeeded<BallHurtbox,asPrototype>(newEntity, pInitData);
	EmplaceEntityIfNeeded<UnitOccupationbox,asPrototype>(newEntity, pInitData);
	EmplaceEntityIfNeeded<WallHurtbox,asPrototype>(newEntity, pInitData);
	EmplaceEntityIfNeeded<CorpsData, asPrototype>(newEntity, pInitData);
	EmplaceEntityIfNeeded<UnitData,asPrototype>(newEntity, pInitData);
	EmplaceEntityIfNeeded<BallData,asPrototype>(newEntity, pInitData);
	EmplaceEntityIfNeeded<Attack, asPrototype>(newEntity, pInitData);
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
int Entities::GetShadowIndex(int x, int y, bool searchFloor) {
	int maskIndex = 0;
	for (int i = 0; i < 8; i++) {
		// 701
		// 6 2
		// 543
		// の順
		int searchX = x;
		int searchY = y;
		if (i >= 1 && i <= 3) {
			searchX++;
		}
		if (i >= 5) {
			searchX--;
		}
		if (i >= 7 || i <= 1) {
			searchY++;
		}
		if (i >= 3 && i <= 5) {
			searchY--;
		}
		if (searchX >= 0 && 
			searchX < WorldWidth && 
			searchY >= 0 && 
			searchY < WorldHeight && 
			(IsWallMap[searchY * WorldWidth + searchX]^searchFloor)) {
			maskIndex += pow(2, i);
		}
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
std::pair<entt::entity, float> Entities::GetNearestHostilingUnit(DirectX::XMVECTOR pos, Interface::HostilityTeam team) {
	entt::entity nearestEntity = entt::entity(0xFFFFFFFF);
	int nearestDistance = 100000;
	for (auto [entity, unitData, worldPosition] : Registry.view<
		Component::UnitData,
		Component::WorldPosition>().each()) {
		if (Interface::HostilityTable[TeamCount * team + unitData.Team]) {
			float length = DirectX::XMVector4Length(DirectX::XMVectorSubtract(pos, worldPosition.WorldPos.Parallel)).m128_f32[0];
			if (length < nearestDistance) {
				nearestDistance = length;
				nearestEntity = entity;
			}
		}
	}
	return { nearestEntity ,nearestDistance };
}