#pragma once
#include "framework.h"
#include "Source/DirectX/Buffers.h"
#include "Effect.h"
#include "Character.h"
#include "Source/DirectX/DrawPipe.h"
#include "Source/DirectX/Texture.h"
#include "Loader.h"
#include "Object.h"
//�L�����N�^�[ �I�u�W�F�N�g�͉��Ȃ��@�����ڂ̓G�t�F�N�g�̂��̂Ȃ���@�L�����N�^�[�̌����ڂ͉��Ȃ���ɒ��S�Œ�
// �L�����N�^�[�I�u�W�F�N�g�A�L�����N�^�[�����ڂȂ疳���HWorld
// �L�����N�^�[�I�u�W�F�N�g�A�L�����N�^�[�����ڂȂ�World��4��ڂ̂݉��Z��������
//�G�t�F�N�g �I�u�W�F�N�g�͉��@�����ڂ͉��Ȃ�
// �G�t�F�N�g�I�u�W�F�N�g�A�G�t�F�N�g�����ڂȂ�s�񓯎m�̐�
// 
//World�ɉ�]�������܂܂�Ă�Ȃ�|���Z������K�v������
//World�ɉ�]�������܂܂�ĂȂ��Ȃ�4�s�ڂ̑����Z�ŗǂ�
class AllGraphics {
	std::vector<std::list<RectGraphic>> Rects;
	std::vector<std::list<StripGraphic>> Strips;
	std::vector<RectCommonInfo> RectCI;
	std::vector<StripCommonInfo> StripCI;
	std::unordered_map<std::string, int> RectDictionary;
	std::unordered_map<std::string, int> StripDictionary;
	AllGraphics();
	void Draw();
	std::list<RectGraphic>::iterator AddRect(int index, std::list<Object>::iterator relatedTo);
	std::list<StripGraphic>::iterator AddStrip(int index, std::list<Object>::iterator relatedTo);
	void LoadRectDefaults(std::string filePath);
	void LoadStripDefaults(std::string filePath);
};
struct RectCommonInfo {
	Texture Tex;
	RectVertexType vDatas[4];
	bool OffsetZero;
	bool OffsetRotatable;
};
struct StripCommonInfo {
	Texture Tex;
};
class RectGraphic {
	friend AllGraphics;
private:
	std::list<Object>::iterator RelativeTo;
	bool IsWorldRollable;
	DirectX::XMMATRIX* World;
	DirectX::XMMATRIX Offset;
public:
	RectGraphic(std::list<Object>::iterator itr) {
		RelativeTo = itr;
		IsWorldRollable = itr->ShapeData.rectWorld.Rollable;
		World = &(itr->ShapeData.rectWorld.World);
	}
};
class StripGraphic {
	friend AllGraphics;
private:
	std::list<Object>::iterator RelativeTo;
	int* count;
	StripVertexType* vDatas;
public:
	StripGraphic(std::list<Object>::iterator itr) {
		RelativeTo = itr;
		vDatas = itr->ShapeData.stripVertexes.VDatas;
		count = &(itr->ShapeData.stripVertexes.VCount);
	}
};