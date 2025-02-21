#include "HitBox.h"


void Hurtboxes::DeleteCharacter(int x, int y, Interface::EntId id){
	int pos = y * Width + x;
	if (OccupyingCharacterCount[pos] < 0) {
		return;
	}
	for (int i = 0; i < 4; i++) {
		if (OccupyingCharacters[pos * 4 + i] == id) {
			OccupyingCharacters[pos * 4 + i] = OccupyingCharacters[pos * 4 + OccupyingCharacterCount[pos]-1];
			OccupyingCharacterCount[pos]--;
			break;
		}
	}
	if (OccupyingCharacterCount[pos] == 0) {
		Interface::OccupiedMap[pos] = false;
	}
}
//衝突時の運動として貫通、消滅、阻害の3つのパターンが考えられる
bool Hurtboxes::CheckCircleCollid(DirectX::XMVECTOR center, float radius, Interface::Damage damage, Interface::Damage* gotDamage, bool checkOnlyOnce,int team,bool collidToWall,bool collidToCharacter,bool collidToAllTeamCharacter,int CoreId){
	//判定対象が含むエリア
	int left = (int)roundf(center.m128_f32[0] - radius);
	int right = (int)roundf(center.m128_f32[0] + radius);
	int top = (int)roundf(center.m128_f32[1] + radius);
	int bottom = (int)roundf(center.m128_f32[1] - radius);
	//衝突したかどうかの変数
	bool collided = false;
	if (collidToWall) {
		for (int x = left; x <= right; x++) {
			for (int y = bottom; y <= top; y++) {
				int pos = y * Width + x;
				//壁との衝突　壁はそこにあるなら衝突している
				if (WallIsThere[pos]) {
					if (checkOnlyOnce) {
						return true;
					}
					collided = true;
				}
			}
		}
	}
	//すでにチェックしたキャラクターのリスト
	std::vector<Interface::EntId> checkedCharacter = std::vector<Interface::EntId>();
	if (collidToCharacter) {
		for (int x = left; x <= right; x++) {
			for (int y = bottom; y <= top; y++) {
				int pos = y * Width + x;
				//キャラクターとの衝突
				for (int i = 0; i < OccupyingCharacterCount[pos]; i++) {
					//すでにチェックしたか調べる
					auto index = std::find(checkedCharacter.begin(), checkedCharacter.end(), OccupyingCharacters[pos * 4 + i]);
					if (index != checkedCharacter.end()) {
						continue;
					}
					//チェック済みに含める
					checkedCharacter.push_back(OccupyingCharacters[pos * 4 + i]);
					Interface::EntityPointer pointer = pAllEntities->INtoIndex[OccupyingCharacters[pos * 4 + i]];
					//敵味方の判定
					bool hostiling = Interface::HostilityTable[team * 8 + pAllEntities->EntityArraies[pointer.Archetype].CharacterData.Components[pointer.Index].Team];
					//敵対しているか全チームと衝突するかつ同じCoreに繋がってないなら
					if ((hostiling || collidToAllTeamCharacter) && 
						(CoreId==-1 || CoreId != pAllEntities->EntityArraies[pointer.Archetype].PositionReference.Components[pointer.Index].ReferenceTo)) {
						//距離の算出
						float length = DirectX::XMVector2Length(DirectX::XMVectorSubtract(center,
							pAllEntities->EntityArraies[pointer.Archetype].
							CharacterHurtbox.Components[pointer.Index].Center)).m128_f32[0];
						if (length < radius + pAllEntities->EntityArraies[pointer.Archetype].CharacterHurtbox.Components[pointer.Index].Radius) {
							//衝突していてなおかつ敵対しているならダメージを与える
							if (hostiling) {
								pAllEntities->EntityArraies[pointer.Archetype].DamagePool.Components[pointer.Index].Damage += damage;
								if (gotDamage != nullptr) {
									(*gotDamage) += pAllEntities->EntityArraies[pointer.Archetype].DamagePool.Components[pointer.Index].Damage;
								}
							}
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
bool Hurtboxes::LimitateInOneBlock(DirectX::XMVECTOR* center, DirectX::XMVECTOR* moveTo, DirectX::XMVECTOR* velocity, float* limitation, float radius, std::vector<Interface::EntId>* pCheckedCharacter, int x, int y) {
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
	for (int i = 0; i < OccupyingCharacterCount[pos]; i++) {
		auto index = std::find(pCheckedCharacter->begin(), pCheckedCharacter->end(), OccupyingCharacters[pos * 4 + i]);
		if (index != pCheckedCharacter->end()) {
			continue;
		}
		Interface::EntityPointer characterPointer = pAllEntities->INtoIndex[OccupyingCharacters[pos * 4 + i]];
		DirectX::XMVECTOR S_E = DirectX::XMVectorSubtract(*center, *moveTo);
		DirectX::XMVECTOR E_C = DirectX::XMVectorSubtract(*moveTo,
			pAllEntities->EntityArraies[characterPointer.Archetype].CharacterHurtbox.Components[characterPointer.Index].Center);
		float a = DirectX::XMVector2Dot(S_E, S_E).m128_f32[0];
		float b = DirectX::XMVector2Dot(S_E, E_C).m128_f32[0];
		float c = DirectX::XMVector2Dot(E_C, E_C).m128_f32[0] - std::powf(
			radius + pAllEntities->EntityArraies[characterPointer.Archetype].CharacterHurtbox.Components[characterPointer.Index].Radius, 2);

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
	std::vector<Interface::EntId> CheckedCharacter = std::vector<Interface::EntId>();
	float limitation = 1;
	if (toRight) {
		if (toTop) {
			for (int x = centerX; x <= moveToX; x++) {
				for (int y = centerY; y <= moveToY; y++) {
					if (LimitateInOneBlock(center, &moveTo, velocity, &limitation, radius, &CheckedCharacter, x, y)) {
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
					if (LimitateInOneBlock(center, &moveTo, velocity, &limitation, radius, &CheckedCharacter, x, y)) {
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
					if (LimitateInOneBlock(center, &moveTo, velocity, &limitation, radius, &CheckedCharacter, x, y)) {
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
					if (LimitateInOneBlock(center, &moveTo, velocity, &limitation, radius, &CheckedCharacter, x, y)) {
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