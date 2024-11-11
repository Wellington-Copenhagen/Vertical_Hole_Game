#pragma once
#include "framework.h"
#include "Effect.h"

union UnionColumnType {
	int Int;
	float Float;
	std::string String;
	bool Bool;
	UnionColumnType() {};
	~UnionColumnType() {};
};
enum EnumColumnType {
	Int = 0,
	Float = 1,
	String = 2,
	Bool = 3
};
class Loader {
public:
	static std::vector<std::vector<UnionColumnType>> LoadToTypeConvertedMatrix(std::string fileName);
};
struct Element {
public:
	std::string Content;
	int LayerLevel;
	bool LayerStart;
	bool LayerEnd;
};
struct OneLayer {
public:
	OneLayer();
	static std::vector<Element> Elements;
	static OneLayer* Root;
	std::unordered_map<std::string, OneLayer*> Leaves;
	std::vector<std::string> leafNames;
	std::string Content;
	std::string GetAsString();
	float GetAsFloat();
	int GetAsInt();
	bool GetAsBool();
	DirectX::XMVECTOR GetAsVector();
	AttackFuncAndArg GetAsAttack();
	void Load(int partStartIndex,int partEndIndex,int yourLayer);
	static void LoadFromFile(std::string filePath);
	OneLayer* Get(std::string address);
	std::vector<std::string> GetAllLeafName();
};