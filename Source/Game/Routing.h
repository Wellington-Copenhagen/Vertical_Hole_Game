#pragma once
#include "Source/DirectX/DirectX.h"
#include <unordered_map>
class Rounting {
	std::list<ConvexArea> AllArea;
};
struct ConvexArea {
	int TopX;
	int BottomX;
	int TopY;
	int BottomY;
	std::vector<std::list<ConvexArea>::iterator> BorderingItr;
	std::vector<DirectX::XMFLOAT2> BorderingPos;
	std::list<ConvexArea>::iterator thisItr;
	void DeleteBordering(std::list<ConvexArea>::iterator itr);
	void CheckAndAddBordering(std::list<ConvexArea>::iterator);
	void TryMargeBordering();
};
struct Node {
	std::vector<std::list<Node>::iterator> ConnectingItr;
	std::vector<int> ConnectingRange;
	int TopX;
	int BottomX;
	int TopY;
	int BottomY;
};