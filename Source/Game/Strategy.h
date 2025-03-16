#pragma once
#include "Source/DirectX/DirectX.h"
#include "Interfaces.h"
#define CorpsPerTeam 8
// 部隊を対象とした戦術の決定
// ミッションファイルで指定されたように動く
// 取れる戦術をいくつかの選択肢として作っておき、強さを決めておく
// そのうえで難易度によって選ぶ確率を決めていく感じか
// 大戦略
// ミッションファイルで任意行動と指定されている場合、大戦術としてより奥地の
// 必要な情報として
// 敵の残存数
class StrategicDecisionMaker {

};
class Strategy {
	std::vector<entt::entity> AllCorps[TeamCount * CorpsPerTeam];
	std::vector<entt::entity> highDamageUnits;
	// 射程内に高火力エンティティのリストに入っている対象があったならそれを出す
	// ない場合はnullptr
	entt::entity* GetNearHighDamageUnit(DirectX::XMVECTOR position, float maximumDistance);
};