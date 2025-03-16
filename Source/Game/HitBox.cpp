#include "HitBox.h"

extern int Tick;

void Hurtboxes::DeleteUnit(int x, int y, entt::entity entity,Interface::HostilityTeam team){
	int pos = (y / UnitSearchDivision) * (WorldWidth / UnitSearchDivision) + (x / UnitSearchDivision);
	UnitMapForSearch[pos * TeamCount + team] = entt::entity(0xFFFFFFFF);


	pos = y * WorldWidth + x;
	if (OccupyingUnitCount[pos] < 0) {
		OccupyingUnitCount[pos] == 0;
		return;
	}
	for (int i = 0; i < UnitCountPerCell; i++) {
		if (OccupyingUnits[pos * UnitCountPerCell + i] == entity) {
			OccupyingUnits[pos * UnitCountPerCell + i] = OccupyingUnits[pos * UnitCountPerCell + OccupyingUnitCount[pos]-1];
			OccupyingUnitCount[pos]--;
			break;
		}
	}
	if (OccupyingUnitCount[pos] == 0) {
		OccupiedMap[pos] = false;
	}
}
entt::entity Hurtboxes::GetNearestUnit(int unitX, int unitY,Interface::HostilityTeam unitTeam, float maximumLength) {
	unitX = unitX / 4;
	unitY = unitY / 4;
	std::vector<int> objectiveTeam = std::vector<int>();
	for (int team = 0; team < 8; team++) {
		if (Interface::HostilityTable[unitTeam]) {
			objectiveTeam.push_back(team);
		}
	}
	int size = IndexLengthFromCenter.size();
	for (int i = 0; i < size; i++) {
		if (std::get<2>(IndexLengthFromCenter[i]) > maximumLength) {
			break;
		}
		for (int team : objectiveTeam) {
			int relX = std::get<0>(IndexLengthFromCenter[i]);
			int relY = std::get<1>(IndexLengthFromCenter[i]);
			for (int j = 0; j < 4; j++) {
				int x = max(0, min((WorldWidth / UnitSearchDivision), unitX + relX * ((j / 2) * 2 - 1)));
				int y = max(0, min((WorldHeight / UnitSearchDivision), unitY + relY * ((j % 2) * 2 - 1)));
				if (UnitMapForSearch[(y * WorldWidth / UnitSearchDivision + x) * TeamCount + team] != entt::entity(0xFFFFFFFF)) {
					return UnitMapForSearch[(y * WorldWidth / UnitSearchDivision + x) * TeamCount + team];
				}
			}
		}
	}
	return entt::entity(0xFFFFFFFF);
}
bool Hurtboxes::CheckUnitCollid(entt::entity coreEntity) {
	pMainRegistry->get<Component::HitFlag>(coreEntity).IsHit =
		CheckCircleCollid(pMainRegistry->get<Component::WorldPosition>(coreEntity).NextTickWorldPos.Parallel,
			pMainRegistry->get<Component::WorldPosition>(coreEntity).WorldPos.Ratio * 3,
			nullptr, false, pMainRegistry->get<Component::UnitData>(coreEntity).Team);
	for (int i = 0; i < 7; i++) {
		entt::entity ball = pMainRegistry->get<Component::UnitData>(coreEntity).BallIds[i];
		//このコアに属するボールに対して喰らい処理
		pMainRegistry->get<Component::HitFlag>(ball).IsHit =
			CheckCircleCollid(pMainRegistry->get<Component::WorldPosition>(ball).NextTickWorldPos.Parallel,
				pMainRegistry->get<Component::WorldPosition>(coreEntity).WorldPos.Ratio *
				pMainRegistry->get<Component::BallHurtbox>(ball).DiameterCoef,
				&pMainRegistry->get<Component::GiveDamage>(ball).Damage,
				false, pMainRegistry->get<Component::UnitData>(coreEntity).Team);
	}
	//移動阻害の判定
	return CheckCircleInterfare(coreEntity);
}
//衝突時の運動として貫通、消滅、阻害の3つのパターンが考えられる
bool Hurtboxes::CheckCircleCollid(DirectX::XMVECTOR center, float radius, Interface::Damage* giveDamage
	, bool checkOnlyOnce, int team)
{
	//判定対象が含むエリア
	int left = max(0,min(WorldWidth,(int)roundf(center.m128_f32[0] - radius * 0.5)));
	int right = max(0, min(WorldWidth, (int)roundf(center.m128_f32[0] + radius * 0.5)));
	int top = max(0, min(WorldHeight, (int)roundf(center.m128_f32[1] + radius * 0.5)));
	int bottom = max(0, min(WorldHeight, (int)roundf(center.m128_f32[1] - radius * 0.5)));
	//衝突したかどうかの変数
	bool collided = false;
	for (int x = left; x <= right; x++) {
		for (int y = bottom; y <= top; y++) {
			int pos = y * WorldWidth + x;
			//壁との衝突　壁はそこにあるなら衝突している
			if (WallIsThere[pos]) {
				entt::entity wall = OccupyingWalls[pos];
				Component::DamagePool* pWallDamagePool = pMainRegistry->try_get<Component::DamagePool>(wall);
				if (pWallDamagePool != nullptr) {
					pWallDamagePool->Damage += *giveDamage;
				}
				if (checkOnlyOnce) {
					return true;
				}
				collided = true;
			}
		}
	}
	//すでにチェックしたキャラクターのリスト
	std::vector<entt::entity> checkedUnit = std::vector<entt::entity>();
	for (int x = left; x <= right; x++) {
		for (int y = bottom; y <= top; y++) {
			int pos = y * WorldWidth + x;
			//キャラクターとの衝突
			for (int i = 0; i < OccupyingUnitCount[pos]; i++) {
				entt::entity core = OccupyingUnits[pos * UnitCountPerCell + i];
				//すでにチェックしたか調べる
				auto index = std::find(checkedUnit.begin(), checkedUnit.end(), core);
				if (index != checkedUnit.end()) {
					continue;
				}
				//チェック済みに含める
				checkedUnit.push_back(core);
				if (pMainRegistry->get<Component::UnitOccupationbox>(core).AlredayChecked) {
					continue;
				}
				for (int j = 0; j < 7; j++) {
					entt::entity ball = pMainRegistry->get<Component::UnitData>(core).BallIds[j];
					//敵味方の判定
					bool hostiling = Interface::HostilityTable[team * 8 + pMainRegistry->get<Component::UnitData>(core).Team];
					//敵対しているなら
					if (hostiling) {
						//距離の算出
						float length = DirectX::XMVector2Length(DirectX::XMVectorSubtract(center,
							pMainRegistry->get<Component::WorldPosition>(ball)
							.NextTickWorldPos.Parallel)).m128_f32[0];
						if (length < radius / 2 + pMainRegistry->get<Component::BallHurtbox>(ball).DiameterCoef*
							pMainRegistry->get<Component::WorldPosition>(core).WorldPos.Ratio / 2) {
							//衝突していてなおかつ敵対しているならダメージを与える
							pMainRegistry->get<Component::DamagePool>(ball).Damage += *giveDamage;
							if (checkOnlyOnce) {
								return true;
							}
							collided = true;
						}
					}
				}
			}
		}
	}
	return collided;
}
bool Hurtboxes::CheckCircleInterfare(entt::entity thisUnitEntity) {
	//判定対象が含むエリア
	DirectX::XMVECTOR center = pMainRegistry->get<Component::WorldPosition>(thisUnitEntity).NextTickWorldPos.Parallel;
	float radius = pMainRegistry->get<Component::WorldPosition>(thisUnitEntity).NextTickWorldPos.Ratio * 0.5;
	int left = max(0, min(WorldWidth, (int)roundf(center.m128_f32[0] - radius)));
	int right = max(0, min(WorldWidth, (int)roundf(center.m128_f32[0] + radius)));
	int top = max(0, min(WorldHeight, (int)roundf(center.m128_f32[1] + radius)));
	int bottom = max(0, min(WorldHeight, (int)roundf(center.m128_f32[1] - radius)));
	int centerX = (int)roundf(center.m128_f32[0]);
	int centerY = (int)roundf(center.m128_f32[1]);
	//衝突したかどうかの変数
	bool collided = false;
	// 中心がある行列を基準に衝突判定後に力が発生する向きを決める
	if (true) {
		for (int x = left; x <= right; x++) {
			int pos = centerY * WorldWidth + x;
			if (WallIsThere[pos]) {
				DirectX::XMVECTOR thisToOther;
				if (center.m128_f32[0] > x) {
					thisToOther = { -1 * (0.5f + radius - (center.m128_f32[0] - x)),0,0,0 };
				}
				else {
					thisToOther = { 0.5f + radius - (x - center.m128_f32[0]),0,0,0 };
				}
				pMainRegistry->get<Component::Motion>(thisUnitEntity).WorldDelta.Parallel = DirectX::XMVectorAdd(pMainRegistry->get<Component::Motion>(thisUnitEntity).WorldDelta.Parallel,
					DirectX::XMVectorScale(thisToOther, -1));
				collided = true;
				break;
			}
		}
		for (int y = bottom; y <= top; y++) {
			int pos = y * WorldWidth + centerX;
			if (WallIsThere[pos]) {
				DirectX::XMVECTOR thisToOther;
				if (center.m128_f32[1] > y) {
					thisToOther = { 0,-1 * (0.5f + radius - (center.m128_f32[1] - y)),0,0 };
				}
				else {
					thisToOther = { 0,0.5f + radius - (y - center.m128_f32[1]),0,0 };
				}
				pMainRegistry->get<Component::Motion>(thisUnitEntity).WorldDelta.Parallel = DirectX::XMVectorAdd(pMainRegistry->get<Component::Motion>(thisUnitEntity).WorldDelta.Parallel,
					DirectX::XMVectorScale(thisToOther, -1));
				collided = true;
				break;
			}
		}
		if (!collided) {
			for (int x = left; x <= right; x++) {
				for (int y = bottom; y <= top; y++) {
					int pos = y * WorldWidth + x;
					//壁との衝突　壁はそこにあるなら衝突している
					if (WallIsThere[pos]) {
						{
							DirectX::XMVECTOR thisToOther;
							if (center.m128_f32[1] > y) {
								thisToOther = { 0,-1 * (0.5f + radius - (center.m128_f32[1] - y)),0,0 };
							}
							else {
								thisToOther = { 0,0.5f + radius - (y - center.m128_f32[1]),0,0 };
							}
							pMainRegistry->get<Component::Motion>(thisUnitEntity).WorldDelta.Parallel = DirectX::XMVectorAdd(pMainRegistry->get<Component::Motion>(thisUnitEntity).WorldDelta.Parallel,
								DirectX::XMVectorScale(thisToOther, -1));
						}
						{
							DirectX::XMVECTOR thisToOther;
							if (center.m128_f32[0] > x) {
								thisToOther = { -1 * (0.5f + radius - (center.m128_f32[0] - x)),0,0,0 };
							}
							else {
								thisToOther = { 0.5f + radius - (x - center.m128_f32[0]),0,0,0 };
							}
							pMainRegistry->get<Component::Motion>(thisUnitEntity).WorldDelta.Parallel = DirectX::XMVectorAdd(pMainRegistry->get<Component::Motion>(thisUnitEntity).WorldDelta.Parallel,
								DirectX::XMVectorScale(thisToOther, -1));
						}
						collided = true;
						break;
					}
				}
				if (collided)
				{
					break;
				}
			}
		}

	}
	//すでにチェックしたキャラクターのリスト
	std::vector<entt::entity> checkedUnit = std::vector<entt::entity>();
	if (true) {
		for (int x = left; x <= right; x++) {
			for (int y = bottom; y <= top; y++) {
				int pos = y * WorldWidth + x;
				//キャラクターとの衝突
				for (int i = 0; i < OccupyingUnitCount[pos]; i++) {
					entt::entity core = OccupyingUnits[pos * UnitCountPerCell + i];
					//すでにチェックしたか調べる
					auto index = std::find(checkedUnit.begin(), checkedUnit.end(), core);
					if (index != checkedUnit.end()) {
						continue;
					}
					//チェック済みに含める
					checkedUnit.push_back(core);
					if (pMainRegistry->get<Component::UnitOccupationbox>(core).AlredayChecked) {
						continue;
					}
					if (core == thisUnitEntity) {
						continue;
					}
					if (Tick%5 > 1) {
						continue;
					}
					//距離の算出
					float length = DirectX::XMVector2Length(DirectX::XMVectorSubtract(center,
						pMainRegistry->get<Component::WorldPosition>(core).NextTickWorldPos.Parallel)).m128_f32[0];
					float minimumDist = radius + pMainRegistry->get<Component::WorldPosition>(core).WorldPos.Ratio * 0.5;
					float overlapRate = 1 * (minimumDist-length) / minimumDist;
					if (overlapRate>0) {
						//重なってる場合
						DirectX::XMVECTOR thisToOther = DirectX::XMVectorSubtract(pMainRegistry->get<Component::WorldPosition>(core).NextTickWorldPos.Parallel,
							pMainRegistry->get<Component::WorldPosition>(thisUnitEntity).NextTickWorldPos.Parallel);
						pMainRegistry->get<Component::Motion>(thisUnitEntity).WorldDelta.Parallel = DirectX::XMVectorAdd(pMainRegistry->get<Component::Motion>(thisUnitEntity).WorldDelta.Parallel,
							DirectX::XMVectorScale(thisToOther, -1 * overlapRate));
						pMainRegistry->get<Component::Motion>(core).WorldDelta.Parallel = DirectX::XMVectorAdd(pMainRegistry->get<Component::Motion>(core).WorldDelta.Parallel,
							DirectX::XMVectorScale(thisToOther,overlapRate));
						collided = true;
					}
				}
			}
		}
	}
	return collided;

}
/*
bool Hurtboxes::LimitateInOneBlock(DirectX::XMVECTOR* center, DirectX::XMVECTOR* moveTo, DirectX::XMVECTOR* velocity, float* limitation, float radius, std::vector<Interface::EntId>* pCheckedBall, int x, int y) {
	int pos = y * Width + x;
	bool limitated = false;
	//壁との衝突
	if (WallIsThere[pos]) {
		Interface::EntityPointer wallPointer = pAllEntities->INtoIndex[OccupyingWalls[pos]];
		DirectX::XMVECTOR E_S = DirectX::XMVectorSubtract(*moveTo, *center);
		DirectX::XMVECTOR Clow_S = DirectX::XMVectorSubtract(
			DirectX::XMVectorAdd(pAllEntities->EntityArraies[wallPointer.Archetype].WorldPosition.Components[wallPointer.Index].WorldPos.Parallel,
				{ -1 * (radius + 0.5f),-1 * (radius + 0.5f) ,0.0f,0.0f }), *center);
		DirectX::XMVECTOR Chigh_S = DirectX::XMVectorSubtract(
			DirectX::XMVectorAdd(pAllEntities->EntityArraies[wallPointer.Archetype].WorldPosition.Components[wallPointer.Index].WorldPos.Parallel,
				{ radius + 0.5f, radius + 0.5f, 0.0f, 0.0f }), *center);
		float tLowX = Clow_S.m128_f32[0] / E_S.m128_f32[0];
		float tHighX = Chigh_S.m128_f32[0] / E_S.m128_f32[0];
		float tLowY = Clow_S.m128_f32[1] / E_S.m128_f32[1];
		float tHighY = Chigh_S.m128_f32[1] / E_S.m128_f32[1];
		float alpha = max(min(tLowX, tHighX), min(tLowY, tHighY));
		float beta = min(max(tLowX, tHighX), max(tLowY, tHighY));
		if (alpha + beta >= 0 && alpha >= beta && alpha < *limitation) {
			*limitation = alpha;
			limitated = true;
		}
	}
	//キャラクターとの衝突
	for (int i = 0; i < OccupyingBallCount[pos]; i++) {
		auto index = std::find(pCheckedBall->begin(), pCheckedBall->end(), OccupyingBalls[pos * 4 + i]);
		if (index != pCheckedBall->end()) {
			continue;
		}
		Interface::EntityPointer ballPointer = pAllEntities->INtoIndex[OccupyingBalls[pos * 4 + i]];
		DirectX::XMVECTOR S_E = DirectX::XMVectorSubtract(*center, *moveTo);
		DirectX::XMVECTOR E_C = DirectX::XMVectorSubtract(*moveTo,
			pAllEntities->EntityArraies[ballPointer.Archetype].BallHurtbox.Components[ballPointer.Index].Center);
		float a = DirectX::XMVector2Dot(S_E, S_E).m128_f32[0];
		float b = DirectX::XMVector2Dot(S_E, E_C).m128_f32[0];
		float c = DirectX::XMVector2Dot(E_C, E_C).m128_f32[0] - std::powf(
			radius + pAllEntities->EntityArraies[ballPointer.Archetype].BallHurtbox.Components[ballPointer.Index].Radius, 2);

		if (b * b - 4 * a * c >= 0) {
			float t = (-1 * b - std::sqrt(b * b - 4 * a * c)) / (2 * a);
			if (t <= 1 && t >= 0 && t < *limitation) {
				*limitation = t;
				limitated = true;
			}
		}
	}
	return limitated;
}
void Hurtboxes::Limitate(DirectX::XMVECTOR* center, float radius, DirectX::XMVECTOR* velocity, Interface::EntId id){
	DirectX::XMVECTOR moveTo = DirectX::XMVectorAdd(*center, *velocity);
	bool toRight = velocity->m128_f32[0] > 0;
	bool toTop = velocity->m128_f32[1] > 0;
	int centerX = (int)roundf(center->m128_f32[0] + radius * toRight ? -1 : 1);
	int moveToX = (int)roundf(moveTo.m128_f32[0] + radius * toRight ? 1 : -1);
	int centerY = (int)roundf(center->m128_f32[1] + radius * toTop ? -1 : 1);
	int moveToY = (int)roundf(moveTo.m128_f32[1] + radius * toTop ? 1 : -1);
	std::vector<Interface::EntId> CheckedBall = std::vector<Interface::EntId>();
	float limitation = 1;
	if (toRight) {
		if (toTop) {
			for (int x = centerX; x <= moveToX; x++) {
				for (int y = centerY; y <= moveToY; y++) {
					if (LimitateInOneBlock(center, &moveTo, velocity, &limitation, radius, &CheckedBall, x, y)) {
						moveTo = DirectX::XMVectorAdd(*center, DirectX::XMVectorScale(*velocity, limitation));
						moveToX = (int)roundf(moveTo.m128_f32[0] + radius * toRight ? 1 : -1);
						moveToY = (int)roundf(moveTo.m128_f32[1] + radius * toTop ? 1 : -1);
					}
				}
			}
		}
		else {
			for (int x = centerX; x <= moveToX; x++) {
				for (int y = centerY; y >= moveToY; y--) {
					if (LimitateInOneBlock(center, &moveTo, velocity, &limitation, radius, &CheckedBall, x, y)) {
						moveTo = DirectX::XMVectorAdd(*center, DirectX::XMVectorScale(*velocity, limitation));
						moveToX = (int)roundf(moveTo.m128_f32[0] + radius * toRight ? 1 : -1);
						moveToY = (int)roundf(moveTo.m128_f32[1] + radius * toTop ? 1 : -1);
					}
				}
			}
		}
	}
	else {
		if (toTop) {
			for (int x = centerX; x >= moveToX; x--) {
				for (int y = centerY; y <= moveToY; y++) {
					if (LimitateInOneBlock(center, &moveTo, velocity, &limitation, radius, &CheckedBall, x, y)) {
						moveTo = DirectX::XMVectorAdd(*center, DirectX::XMVectorScale(*velocity, limitation));
						moveToX = (int)roundf(moveTo.m128_f32[0] + radius * toRight ? 1 : -1);
						moveToY = (int)roundf(moveTo.m128_f32[1] + radius * toTop ? 1 : -1);
					}
				}
			}
		}
		else {
			for (int x = centerX; x >= moveToX; x--) {
				for (int y = centerY; y >= moveToY; y--) {
					if (LimitateInOneBlock(center, &moveTo, velocity, &limitation, radius, &CheckedBall, x, y)) {
						moveTo = DirectX::XMVectorAdd(*center, DirectX::XMVectorScale(*velocity, limitation));
						moveToX = (int)roundf(moveTo.m128_f32[0] + radius * toRight ? 1 : -1);
						moveToY = (int)roundf(moveTo.m128_f32[1] + radius * toTop ? 1 : -1);
					}
				}
			}
		}
	}
	if (limitation < 1) {
		*velocity = { 0.0f,0.0f, 0.0f, 0.0f };
	}
}
*/
bool Hurtboxes::IsAbleToSpawn(float left, float right, float top, float bottom, float posX, float posY) {
	if (posX - left < 0 || posX + right >= WorldWidth || posY - bottom < 0 || posY + top >= WorldHeight) {
		return false;
	}
	for (int x = posX - left; x <= posX + right; x++) {
		for (int y = posY - bottom; y <= posY + top; y++) {
			if (WallIsThere[y * WorldWidth + x]) {
				return false;
			}
		}
	}
	return true;
}
void Hurtboxes::SetOccupation(float left, float right, float top, float bottom, float posX, float posY) {
	for (int x = posX - left; x <= posX + right; x++) {
		for (int y = posY - bottom; y <= posY + top; y++) {
			OccupiedMap[y * WorldWidth + x] = true;
		}
	}
}

void Hurtboxes::UpdatePassableMap() {
	CheckSetPassable(256, 0);
}
bool Hurtboxes::CheckSetPassable(int currentIndex, int currentSize) {
	bool output = true;
	if (currentSize != 1) {
		output &= CheckSetPassable(currentIndex * 4 + 1, currentSize / 2);
		output &= CheckSetPassable(currentIndex * 4 + 2, currentSize / 2);
		output &= CheckSetPassable(currentIndex * 4 + 3, currentSize / 2);
		output &= CheckSetPassable(currentIndex * 4 + 4, currentSize / 2);
	}
	else {
		output &= OccupiedMap[currentIndex * 4 + 1 - 21845];
		output &= OccupiedMap[currentIndex * 4 + 2 - 21845];
		output &= OccupiedMap[currentIndex * 4 + 3 - 21845];
		output &= OccupiedMap[currentIndex * 4 + 4 - 21845];
	}
	return output;
}
DirectX::XMVECTOR Hurtboxes::NextWayPoint(DirectX::XMVECTOR current, DirectX::XMVECTOR target) {
	return {0,0,0,1};
}
bool Hurtboxes::IsWayClear(DirectX::XMVECTOR* end1, DirectX::XMVECTOR* end2) {
	float slope = (end2->m128_f32[1] - end1->m128_f32[1]) / (end2->m128_f32[0] - end1->m128_f32[0]);
	float intercept = end2->m128_f32[1] - slope * end2->m128_f32[0];
	if (abs(slope) < 1) {
		int right = (int)roundf(max(end1->m128_f32[0], end2->m128_f32[0]));
		int left = (int)roundf(min(end1->m128_f32[0], end2->m128_f32[0]));
		for (int x = left; x <= right; x++) {
			int y = (int)roundf(intercept + slope * (float)x);
			if (WallIsThere[y * WorldWidth + x]) {
				return false;
			}
		}
	}
	else {
		slope = 1/slope;
		float intercept = end2->m128_f32[0] - slope * end2->m128_f32[1];
		int top = (int)roundf(max(end1->m128_f32[1], end2->m128_f32[1]));
		int bottom = (int)roundf(min(end1->m128_f32[1], end2->m128_f32[1]));
		for (int y = bottom; y <= top; y++) {
			int x = (int)roundf(intercept + slope * (float)y);
			if (WallIsThere[y * WorldWidth + x]) {
				return false;
			}
		}
	}
	return true;
}