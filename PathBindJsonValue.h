#pragma once
#include "framework.h"
#include "DebugAssist.h"
class PathBindJsonValue {
public:
	Json::Value mValue;
	std::string Path;
	PathBindJsonValue(std::string filePath) {
		//マップデータの読み込み
		Json::Value root;
		std::ifstream file = std::ifstream(filePath);
		if (!file) {
			DebugLogOutput("Json Error:File not found:{}", filePath);
		}
		file >> mValue;
		file.close();
		Path = filePath;
	}
	PathBindJsonValue(Json::Value value ,std::string path) {
		mValue = value;
		Path = path;
	}
	PathBindJsonValue get(std::string key) {
		if (mValue.isMember(key)) {
			return PathBindJsonValue(mValue.get(key, ""), Path + "/" + key);
		}
		else {
			DebugLogOutput("Json Error:Missing key \"{}\" :{}", key, Path);
			throw("");
		}
	}
	std::string asString() {
		if (mValue.isString()) {
			return mValue.asString();
		}
		else {
			DebugLogOutput("Json Error: Not string :{}", Path);
			throw("");
		}
	}
	int asInt() {
		if (mValue.isInt()) {
			return mValue.asInt();
		}
		else {
			DebugLogOutput("Json Error: Not int :{}", Path);
			throw("");
		}
	}
	float asFloat() {
		if (mValue.isDouble()) {
			return mValue.asFloat();
		}
		else {
			DebugLogOutput("Json Error: Not float :{}", Path);
			throw("");
		}
	}
	bool asBool() {
		if (mValue.isBool()) {
			return mValue.asBool();
		}
		else {
			DebugLogOutput("Json Error: Not bool :{}", Path);
			throw("");
		}
	}
	DirectX::XMVECTOR asVector() {
		if (!mValue.isArray() || mValue.size() < 4) {
			DebugLogOutput("Json Error: Not vector :{}", Path);
			throw("");
		}
		DirectX::XMVECTOR output = {
			mValue[0].asFloat(),
			mValue[1].asFloat(),
			mValue[2].asFloat(),
			mValue[3].asFloat(),
		};
		return output;
	}
	std::string tryGetAsString(std::string key, std::string def) {
		if (mValue.isMember(key) && mValue.get(key,"").isString()) {
			return mValue.get(key, "").asString();
		}
		else {
			return def;
		}
	}
	int tryGetAsInt(std::string key, int def) {
		if (mValue.isMember(key) && mValue.get(key, "").isInt()) {
			return mValue.get(key, "").asInt();
		}
		else {
			return def;
		}
	}
	float tryGetAsFloat(std::string key, float def) {
		if (mValue.isMember(key) && mValue.get(key, "").isDouble()) {
			return mValue.get(key, "").asFloat();
		}
		else {
			return def;
		}
	}
	bool tryGetAsBool(std::string key, bool def) {
		if (mValue.isMember(key) && mValue.get(key, "").isBool()) {
			return mValue.get(key, "").asBool();
		}
		else {
			return def;
		}
	}
	DirectX::XMVECTOR tryGetAsVector(std::string key, DirectX::XMVECTOR def) {
		if (mValue.isMember(key) && mValue.get(key, "").isArray() && mValue.get(key, "").size() >= 4) {
			DirectX::XMVECTOR output = {
				mValue.get(key, "")[0].asFloat(),
				mValue.get(key, "")[1].asFloat(),
				mValue.get(key, "")[2].asFloat(),
				mValue.get(key, "")[3].asFloat(),
			};
			return output;
		}
		else {
			return def;
		}
	}
	int size() {
		return mValue.size();
	}
	std::vector<std::string> getMemberNames() {
		return mValue.getMemberNames();
	}
	bool isMember(std::string key) {
		return mValue.isMember(key);
	}
	PathBindJsonValue operator[](int index) {
		if (index < mValue.size()) {
			return PathBindJsonValue(mValue[index], Path + "/" + std::to_string(index));
		}
		else {
			DebugLogOutput("Json Error: Index out of range :{}", Path);
			throw("");
		}
	}
};