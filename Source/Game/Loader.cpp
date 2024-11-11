#include "Loader.h"
#include "fstream"
#include "iostream"
#include "Source/DirectX/DirectX.h"

std::vector<std::vector<UnionColumnType>> Loader::LoadToTypeConvertedMatrix(std::string fileName) {
	std::ifstream file(fileName);
	std::string line;
	std::vector<std::vector<UnionColumnType>> output = std::vector<std::vector<UnionColumnType>>();
	std::vector<EnumColumnType> typeList = std::vector<EnumColumnType>();
	int lineCount = 0;
	while (std::getline(file, line)) {
		if (lineCount > 0) {
			std::vector<UnionColumnType> thisLine;
			std::string element = "";
			int columnCount = 0;
			for (int i = 0; i < line.length(); i++) {
				if (line[i] == ',' || i == line.length() - 1) {
					if (i == 1) {
						if (element[0] == 'F') {
							typeList.push_back(EnumColumnType::Float);
						}
						if (element[0] == 'I') {
							typeList.push_back(EnumColumnType::Int);
						}
						if (element[0] == 'S') {
							typeList.push_back(EnumColumnType::String);
						}
						if (element[0] == 'B') {
							typeList.push_back(EnumColumnType::Bool);
						}
						element = "";
						columnCount++;
					}
					else {
						UnionColumnType converted;
						if (typeList[columnCount] == EnumColumnType::Int) {
							converted.Int = std::stoi(element);
						}
						if (typeList[columnCount] == EnumColumnType::Float) {
							converted.Float = std::stof(element);
						}
						if (typeList[columnCount] == EnumColumnType::String) {
							converted.String = element;
						}
						if (typeList[columnCount] == EnumColumnType::Bool) {
							if (element[0] == 'T'|| element[0] == 't') {
								converted.Bool = true;
							}
							if (element[0] == 'F' || element[0] == 'f') {
								converted.Bool = false;
							}
						}
						thisLine.push_back(converted);
						element = "";
						columnCount++;
					}
				}
				else {
					element += line[i];
				}
			}
			output.push_back(thisLine);
			lineCount++;
		}
	}
	return output;
}
OneLayer::OneLayer() {
	Content = "";

}
std::string OneLayer::GetAsString() {
	return Content;
}
float OneLayer::GetAsFloat() {
	return std::stof(Content);
}
int OneLayer::GetAsInt() {
	return std::stoi(Content);
}
bool OneLayer::GetAsBool() {
	if (Content[0] == 'T' || Content[0] == 't') {
		return true;
	}
	else if (Content[0] == 'F' || Content[0] == 'f') {
		return false;
	}
	else {
		throw("");
	}
}
DirectX::XMVECTOR OneLayer::GetAsVector() {
	DirectX::XMVECTOR output = {Leaves["x"]->GetAsFloat(),Leaves["y"]->GetAsFloat(), Leaves["z"]->GetAsFloat(), Leaves["w"]->GetAsFloat() };
	return output;
}
AttackFuncAndArg OneLayer::GetAsAttack() {
	AttackFuncAndArg output;
	output.Func = Attack::AttackDictionary[Get("FuncName")->GetAsString()];
	output.Arg1 = Get("Arg1")->GetAsFloat();
	output.Arg2 = Get("Arg2")->GetAsFloat();
	return output;
}
OneLayer* OneLayer::Get(std::string address) {
	return Leaves[address];
}
void OneLayer::Load(int partStartIndex, int partEndIndex, int yourLayer) {
	int StartIndex = -1;
	for (int i = partStartIndex; i < partEndIndex; i++) {
		if (Elements[i].LayerLevel == yourLayer) {
			if (!Elements[i].LayerStart && !Elements[i].LayerEnd) {
				Content = Elements[i].Content;
			}
			if (Elements[i].LayerStart && !Elements[i].LayerEnd) {
				StartIndex = i;
			}
			if (!Elements[i].LayerStart && Elements[i].LayerEnd) {
				if (StartIndex == -1)throw("");
				Leaves[Elements[StartIndex].Content] = new OneLayer();
				Leaves[Elements[StartIndex].Content]->Load(StartIndex+1,i,yourLayer+1);
				leafNames.push_back(Elements[StartIndex].Content);
			}
		}
	}
}
void OneLayer::LoadFromFile(std::string filePath) {
	Elements = std::vector<Element>();
	std::ifstream file(filePath);
	std::string rawStr = "";
	std::string line;
	while (std::getline(file, line)) {
		rawStr = rawStr + line;
	}
	std::string separated = "";
	int currentLayerLevel = 0;
	for (int i = 0; i < rawStr.size(); i++) {
		if (rawStr[i] == ' '|| rawStr[i] == '\t' || rawStr[i] == '\n') {
			rawStr.erase(i);
			i--;
		}
		else if (rawStr[i] == '<') {
			Element toPush;
			if (separated[0] == '<') {
				throw("");
			}
			else {
				toPush.LayerStart = false;
				toPush.LayerEnd = false;
			}
			toPush.Content = separated;
			Elements.push_back(toPush);
			separated == "";
			separated = separated + rawStr[i];
		}
		else if (rawStr[i] == '>') {
			Element toPush;
			if (separated[0] == '<') {
				if (separated[0] == '/') {
					toPush.LayerStart = false;
					toPush.LayerEnd = true;
					currentLayerLevel--;
					toPush.LayerLevel = currentLayerLevel;
					separated.erase(separated.begin());
					separated.erase(separated.begin());
				}
				else {
					toPush.LayerStart = true;
					toPush.LayerEnd = false;
					toPush.LayerLevel = currentLayerLevel;
					currentLayerLevel++;
					separated.erase(separated.begin());
				}
			}
			else {
				throw("");
			}
			toPush.Content = separated;
			Elements.push_back(toPush);
			separated == "";
		}
		else {
			separated = separated + rawStr[i];
		}
	}
	auto itr = Elements.begin();
	while (itr != Elements.end()) {
		if (itr->Content.size() == 0) {
			itr = Elements.erase(itr);
		}
		else {
			itr++;
		}
	}
	Root = new OneLayer();
	Root->Load(0, Elements.size(), 0);
}