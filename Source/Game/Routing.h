#pragma once
#include "Source/DirectX/DirectX.h"
#include "Interfaces.h"
#include "framework.h"
#define MaxAreaCount 256
struct ConvexArea {
	int Top;
	int Bottom;
	int Left;
	int Right;
	// 対象エリアに向かいたいとき、最短となるのはどのコネクションを通る場合か
	std::vector<int> WhereToGo;
	ConvexArea(int bottom, int top, int left, int right) {
		WhereToGo = std::vector<int>();
		if (top <= bottom || right <= left) {
			throw("");
		}
		Top = top;
		Bottom = bottom;
		Left = left;
		Right = right;
	}
	inline float DistanceBetween(ConvexArea* pOther) {
		return abs((Left + Right) / 2.0f - (pOther->Left + pOther->Right) / 2.0f) +
			abs((Bottom + Top) / 2.0f - (pOther->Bottom + pOther->Top) / 2.0f);
	}
	bool IsNeighboring(ConvexArea* pOther) {
		return ((pOther->Left == Right ||pOther->Right == Left)
			&&
			pOther->Bottom<Top && Bottom < pOther->Top)
			||
			((pOther->Top == Bottom ||	pOther->Bottom == Top)
				&&
				pOther->Left < Right && Left < pOther->Right);
	}
	bool IsOverlapping(ConvexArea* pOther) {
		return ((Left > pOther->Left && Left < pOther->Right) ||
				(Right > pOther->Left && Right < pOther->Right)) &&
				((Top > pOther->Bottom && Top < pOther->Top) ||
				(Bottom > pOther->Bottom && Bottom < pOther->Top));
	}
};
struct AreaConnection {
	AreaConnection(){}
	AreaConnection(ConvexArea lower, ConvexArea higher) {
		Range = lower.DistanceBetween(&higher);
		if (lower.Bottom == higher.Top || lower.Top == higher.Bottom) {
			// 上下に並んでる状態
			Bottom = max(lower.Left, higher.Left) + 1;
			Top = min(lower.Right, higher.Right) - 1;
			IsHorizontal = true;
			if (lower.Bottom == higher.Top) {
				Pos = lower.Bottom - 0.5;
				IsMinOnLower = false;
			}
			else {
				Pos = lower.Top - 0.5;
				IsMinOnLower = true;
			}
		}
		if (lower.Left == higher.Right || lower.Right == higher.Left) {
			// 左右に並んでる状態
			Bottom = max(lower.Bottom, higher.Bottom) + 1;
			Top = min(lower.Top, higher.Top) - 1;
			IsHorizontal = false;
			if (lower.Left == higher.Right) {
				Pos = lower.Left - 0.5;
				IsMinOnLower = false;
			}
			else {
				Pos = lower.Right - 0.5;
				IsMinOnLower = true;
			}
		}
	}
	// 距離
	int Range;
	bool IsHorizontal;
	// エリアインデックスが小さいほうが座標が小さい側にある
	bool IsMinOnLower;
	// 境界の値が大きい端
	int Top;
	// 境界の値が小さい端
	int Bottom;
	// 境界がある位置
	float Pos;
	Interface::WayPoint GetWayPoint(int current,int goingTo,DirectX::XMVECTOR currentPos) {
		Interface::WayPoint output;
		if (IsHorizontal) {
			if ((current < goingTo) ^ IsMinOnLower) {
				output.Heading = {
					0,1,0,0
				};
			}
			else {
				output.Heading = {
					0,-1,0,0
				};
			}
			output.Pos = {
				max(min(currentPos.m128_f32[0],(float)Top),(float)Bottom),
				Pos,
				0,1
			};
		}
		else {
			if ((current < goingTo) ^ IsMinOnLower) {
				output.Heading = {
					1,0,0,0
				};
			}
			else {
				output.Heading = {
					-1,0,0,0
				};
			}
			output.Pos = {
				Pos,
				max(min(currentPos.m128_f32[0],(float)Top),(float)Bottom),
				0,1
			};
		}
		return output;
	}
};
class Routing {
public:
	std::vector<ConvexArea> AllArea;
	// コネクション
	// min(a,b) * MaxAreaCount + max(a,b)をキーにする
	std::unordered_map<int,AreaConnection> Connections;
	Routing() {
		AllArea = std::vector<ConvexArea>();
		Connections = std::unordered_map<int, AreaConnection>();
		
	}
	void AddArea(ConvexArea area);
	// howFar==1で次のウェイポイント
	Interface::WayPoint GetWayPoint(int current, int target, DirectX::XMVECTOR currentPos);
	void FormRouting();
};