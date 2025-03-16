#include "Routing.h"

void Routing::AddArea(ConvexArea area) {
	for (int i = 0; i < AllArea.size(); i++) {
		if (area.IsOverlapping(&AllArea[i])) {
			// エリアの重なりは認められない
			throw("");
		}
	}
	int newIndex = AllArea.size();
	AllArea.push_back(area);
	for (int i = 0; i < AllArea.size(); i++) {
		if (area.IsNeighboring(&AllArea[i])) {
			// エリアが接している場合
			Connections.emplace(min(newIndex, i) * MaxAreaCount + max(newIndex, i),AreaConnection(area, AllArea[i]));
		}
	}

}
Interface::WayPoint Routing::GetWayPoint(int current, int target, DirectX::XMVECTOR currentPos) {
	int nextArea = AllArea[current].WhereToGo[target];
	return Connections[min(current, nextArea) * MaxAreaCount + max(current, nextArea)].GetWayPoint(current,target,currentPos);
}
void Routing::FormRouting() {
	for (int i = 0; i < AllArea.size(); i++) {
		AllArea[i].WhereToGo.clear();
		for (int j = 0; j < AllArea.size(); j++) {
			AllArea[i].WhereToGo.push_back(-1);
		}
	}
	// 出発地点が1つ目のインデックス
	// 目標地点が2つ目のインデックス
	int areaCount = AllArea.size();
	std::vector<std::vector<float>> distance;
	std::vector<std::vector<bool>> confirmed;
	std::vector<std::vector<int>> wayFrom;
	// goalからstartに向かうときにstartの次に向かうべき場所
	for (int goal = 0; goal < areaCount; goal++) {
		distance.push_back(std::vector<float>());
		confirmed.push_back(std::vector<bool>());
		wayFrom.push_back(std::vector<int>());
		for (int start = 0; start < areaCount; start++) {
			if (goal == start) {
				confirmed[goal].push_back(true);
				distance[goal].push_back(0);
				wayFrom[goal].push_back(goal);
			}
			else if (Connections.contains(min(goal, start) * MaxAreaCount + max(goal, start))) {
				confirmed[goal].push_back(false);
				distance[goal].push_back(Connections[min(goal, start) * MaxAreaCount + max(goal, start)].Range);
				wayFrom[goal].push_back(goal);
			}
			else {
				confirmed[goal].push_back(false);
				distance[goal].push_back(1000000);
				wayFrom[goal].push_back(-1);
			}
		}
	}
	for (int goal = 0; goal < areaCount; goal++) {
		while (true) {
			bool finished = true;
			float shortestValue = 1000000;
			float shortestIndex = -1;
			for (int start = 0; start < areaCount; start++) {
				if (!confirmed[goal][start]) {
					finished = false;
					if (distance[goal][start] < shortestValue) {
						shortestIndex = start;
						shortestValue = distance[goal][start];
					}
				}
			}
			if (finished) {
				break;
			}
			if (shortestIndex == -1) {
				// たぶん全体が1つになってない
				throw("");
			}
			confirmed[goal][shortestIndex] = true;
			for (int start = 0; start < areaCount; start++) {
				if (shortestValue + distance[shortestIndex][start] < distance[goal][start] && 
					Connections.contains(min(shortestIndex, start) * MaxAreaCount + max(shortestIndex, start))) {
					distance[goal][start] = shortestValue + distance[shortestIndex][start];
					wayFrom[goal][start] = shortestIndex;
				}
			}
		}
	}
	for (int goal = 0; goal < areaCount; goal++) {
		for (int start = 0; start < areaCount; start++) {
			AllArea[start].WhereToGo[goal] = wayFrom[goal][start];
		}
	}
}