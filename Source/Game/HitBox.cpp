#include "HitBox.h"


void Hurtboxes::DeleteUnit(int x, int y, Interface::EntId id){
	int pos = y * WorldWidth + x;
	if (OccupyingUnitCount[pos] < 0) {
		return;
	}
	for (int i = 0; i < 4; i++) {
		if (OccupyingUnits[pos * 4 + i] == id) {
			OccupyingUnits[pos * 4 + i] = OccupyingUnits[pos * 4 + OccupyingUnitCount[pos]-1];
			OccupyingUnitCount[pos]--;
			break;
		}
	}
	if (OccupyingUnitCount[pos] == 0) {
		OccupiedMap[pos] = false;
	}
}
bool Hurtboxes::CheckUnitCollid(Interface::EntId coreId) {
	Interface::EntityPointer core = pAllEntities->INtoIndex[coreId];
	pAllEntities->EntityArraies[core.Archetype].HitFlag.Components[core.Index].IsHit = 
		CheckCircleCollid(pAllEntities->EntityArraies[core.Archetype].WorldPosition.Components[core.Index].NextTickWorldPos.Parallel,
		pAllEntities->EntityArraies[core.Archetype].WorldPosition.Components[core.Index].WorldPos.Ratio * 
		pAllEntities->EntityArraies[core.Archetype].BallHurtbox.Components[core.Index].DiameterCoef,
		&pAllEntities->EntityArraies[core.Archetype].GiveDamage.Components[core.Index].Damage,
		false, pAllEntities->EntityArraies[core.Archetype].UnitData.Components[core.Index].Team);
	for (int i = 0; i < 6; i++) {
		if (pAllEntities->EntityArraies[core.Archetype].UnitData.Components[core.Index].IsBallExist[i]) {
			Interface::EntityPointer ball = pAllEntities->INtoIndex[
				pAllEntities->EntityArraies[core.Archetype].UnitData.Components[core.Index].BallIds[i]];
			//このコアに属するボールに対して喰らい処理
			pAllEntities->EntityArraies[ball.Archetype].HitFlag.Components[ball.Index].IsHit =
			CheckCircleCollid(pAllEntities->EntityArraies[ball.Archetype].WorldPosition.Components[ball.Index].NextTickWorldPos.Parallel,
				pAllEntities->EntityArraies[ball.Archetype].WorldPosition.Components[ball.Index].WorldPos.Ratio *
				pAllEntities->EntityArraies[ball.Archetype].BallHurtbox.Components[ball.Index].DiameterCoef,
				&pAllEntities->EntityArraies[ball.Archetype].GiveDamage.Components[ball.Index].Damage,
				false, pAllEntities->EntityArraies[core.Archetype].UnitData.Components[core.Index].Team);
		}
	}
	//移動阻害の判定
	return CheckCircleInterfare(pAllEntities->EntityArraies[core.Archetype].WorldPosition.Components[core.Index].NextTickWorldPos.Parallel,
		pAllEntities->EntityArraies[core.Archetype].WorldPosition.Components[core.Index].WorldPos.Ratio, true, true);
}
//衝突時の運動として貫通、消滅、阻害の3つのパターンが考えられる
bool Hurtboxes::CheckCircleCollid(DirectX::XMVECTOR center, float radius, Interface::Damage* giveDamage
	, bool checkOnlyOnce, int team)
{
	//判定対象が含むエリア
	int left = (int)roundf(center.m128_f32[0] - radius * 0.5);
	int right = (int)roundf(center.m128_f32[0] + radius * 0.5);
	int top = (int)roundf(center.m128_f32[1] + radius * 0.5);
	int bottom = (int)roundf(center.m128_f32[1] - radius * 0.5);
	//衝突したかどうかの変数
	bool collided = false;
	for (int x = left; x <= right; x++) {
		for (int y = bottom; y <= top; y++) {
			int pos = y * WorldWidth + x;
			//壁との衝突　壁はそこにあるなら衝突している
			if (WallIsThere[pos]) {
				Interface::EntityPointer pointer = pAllEntities->INtoIndex[OccupyingWalls[pos]];
				pAllEntities->EntityArraies[pointer.Archetype].DamagePool.Components[pointer.Index].Damage += *giveDamage;
				if (checkOnlyOnce) {
					return true;
				}
				collided = true;
			}
		}
	}
	//すでにチェックしたキャラクターのリスト
	std::vector<Interface::EntId> checkedUnit = std::vector<Interface::EntId>();
	for (int x = left; x <= right; x++) {
		for (int y = bottom; y <= top; y++) {
			int pos = y * WorldWidth + x;
			//キャラクターとの衝突
			for (int i = 0; i < OccupyingUnitCount[pos]; i++) {
				for (int i = 0; i < 7; i++) {
					Interface::EntityPointer corePointer = pAllEntities->INtoIndex[OccupyingUnits[pos * 4 + i]];
					Interface::EntityPointer pointer = pAllEntities->INtoIndex[OccupyingUnits[pos * 4 + i]];
					//その位置にエンティティがない場合はスキップ
					if (!pAllEntities->EntityArraies[pointer.Archetype].UnitData.Components[pointer.Index].IsBallExist[i]) {
						continue;
					}
					if (i != 6) {
						pointer = pAllEntities->INtoIndex[pAllEntities->EntityArraies[pointer.Archetype].UnitData.Components[pointer.Index].BallIds[i]];
					}
					//すでにチェックしたか調べる
					auto index = std::find(checkedUnit.begin(), checkedUnit.end(), OccupyingUnits[pos * 4 + i]);
					if (index != checkedUnit.end()) {
						continue;
					}
					//チェック済みに含める
					checkedUnit.push_back(OccupyingUnits[pos * 4 + i]);
					//敵味方の判定
					bool hostiling = Interface::HostilityTable[team * 8 + pAllEntities->EntityArraies[corePointer.Archetype].UnitData.Components[corePointer.Index].Team];
					//敵対しているなら
					if (hostiling) {
						//距離の算出
						float length = 0;
						if (pAllEntities->EntityArraies[corePointer.Archetype].UnitOccupationbox.Components[corePointer.Index].AlredayChecked) {
							float length = DirectX::XMVector2Length(DirectX::XMVectorSubtract(center,
								pAllEntities->EntityArraies[pointer.Archetype].
								WorldPosition.Components[pointer.Index].NextTickWorldPos.Parallel)).m128_f32[0];
						}
						else {
							float length = DirectX::XMVector2Length(DirectX::XMVectorSubtract(center,
								pAllEntities->EntityArraies[pointer.Archetype].
								WorldPosition.Components[pointer.Index].WorldPos.Parallel)).m128_f32[0];
						}
						if (length < radius / 6 + pAllEntities->EntityArraies[pointer.Archetype].BallHurtbox.Components[pointer.Index].DiameterCoef*
							pAllEntities->EntityArraies[pointer.Archetype].WorldPosition.Components[pointer.Index].WorldPos.Ratio / 6) {
							//衝突していてなおかつ敵対しているならダメージを与える
							pAllEntities->EntityArraies[pointer.Archetype].DamagePool.Components[pointer.Index].Damage += *giveDamage;
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
bool Hurtboxes::CheckCircleInterfare(DirectX::XMVECTOR center, float radius, bool collidToWall,	bool collidToBall) {
	//判定対象が含むエリア
	int left = (int)roundf(center.m128_f32[0] - radius * 0.5);
	int right = (int)roundf(center.m128_f32[0] + radius * 0.5);
	int top = (int)roundf(center.m128_f32[1] + radius * 0.5);
	int bottom = (int)roundf(center.m128_f32[1] - radius * 0.5);
	//衝突したかどうかの変数
	bool collided = false;
	if (collidToWall) {
		for (int x = left; x <= right; x++) {
			for (int y = bottom; y <= top; y++) {
				int pos = y * WorldWidth + x;
				//壁との衝突　壁はそこにあるなら衝突している
				if (WallIsThere[pos]) {
					return true;
				}
			}
		}
	}
	//すでにチェックしたキャラクターのリスト
	std::vector<Interface::EntId> checkedUnit = std::vector<Interface::EntId>();
	if (collidToBall) {
		for (int x = left; x <= right; x++) {
			for (int y = bottom; y <= top; y++) {
				int pos = y * WorldWidth + x;
				//キャラクターとの衝突
				for (int i = 0; i < OccupyingUnitCount[pos]; i++) {
					//すでにチェックしたか調べる
					auto index = std::find(checkedUnit.begin(), checkedUnit.end(), OccupyingUnits[pos * 4 + i]);
					if (index != checkedUnit.end()) {
						continue;
					}
					//チェック済みに含める
					checkedUnit.push_back(OccupyingUnits[pos * 4 + i]);
					Interface::EntityPointer pointer = pAllEntities->INtoIndex[OccupyingUnits[pos * 4 + i]];
					//距離の算出
					float length = 0;
					if (pAllEntities->EntityArraies[pointer.Archetype].UnitOccupationbox.Components[pointer.Index].AlredayChecked) {
						length = DirectX::XMVector2Length(DirectX::XMVectorSubtract(center,
							pAllEntities->EntityArraies[pointer.Archetype].
							WorldPosition.Components[pointer.Index].NextTickWorldPos.Parallel)).m128_f32[0];
					}
					else {
						length = DirectX::XMVector2Length(DirectX::XMVectorSubtract(center,
							pAllEntities->EntityArraies[pointer.Archetype].
							WorldPosition.Components[pointer.Index].WorldPos.Parallel)).m128_f32[0];
					}
					if (length < radius*0.5 + pAllEntities->EntityArraies[pointer.Archetype].WorldPosition.Components[pointer.Index].WorldPos.Ratio*0.5) {
						return true;
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
	if (posX - left < 0 || posX + right >= Width || posY - bottom < 0 || posY + top >= Height) {
		return false;
	}
	for (int x = posX - left; x <= posX + right; x++) {
		for (int y = posY - bottom; y <= posY + top; y++) {
			if (OccupiedMap[y * WorldWidth + x]) {
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
	PassableMap[currentIndex] = output;
	return output;
}
DirectX::XMVECTOR Hurtboxes::NextWayPoint(DirectX::XMVECTOR current, DirectX::XMVECTOR target) {

}