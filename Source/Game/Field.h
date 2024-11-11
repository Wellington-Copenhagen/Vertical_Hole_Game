#pragma once
#include "Source/DirectX/DirectX.h"
#include "Source/DirectX/Texture.h"
#include "Source/DirectX/Buffers.h"
#include "Shape.h"
class AllPlate {
private:
	Plate* plates;
	// �ړ��J�n���_�ňړ���v���[�g���L����
	// 
	// �����𒴂������_�ňړ����v���[�g�̐�L��������
	// �ړ����Ɉړ�������ύX�����ꍇ�͕ύX�悪��L�ς݂Ȃ�ω������Ȃ�
	// �ړ��悪��L�ς݂łȂ��Ȃ猳�̈ړ��\��v���[�g�̐�L���������A�V�����ړ���v���[�g���L����
	// �ړ������͐V�����ړ���v���[�g�̒��S�ɂȂ�悤�ɕ␳����
	bool* IsOccupied;// 
	int Width;
	int Height;
public:
	AllPlate(std::string fileName);
	void CheckCollision();
	void Draw();
};
//1��ނ̃e�N�X�`��������
class Plate {
public:
	Texture Texture;
	RectVertexType NormalUV;
	Rect* PosList;
	float Passability;
	int UsedCount;
	Plate(int usedCount);
	void SetToBufferData();
};