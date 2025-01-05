#pragma once
#include "framework.h"
#include "Source/DirectX/DirectX.h"
#include "Interfaces.h"

#define BLENGTHINSTANCE 1000
#define BLENGTHVERTEX 1000000
#define BLENGTHINDEX 2000000
#define BLENGTHLIGHT 100
//�����炭�������_�ɕʂ�UV�𓖂Ă邱�Ƃ͂ł��Ȃ�
//���_�o�b�t�@�ɂ͊e���_�̃��[���h���W��������������
//�C���f�b�N�X�o�b�t�@�͌Ăяo����
//�萔�o�b�t�@�ɂ̓��[���h�����e�̕ϊ����W������̈ʒu����������
//�\�����o�b�t�@�ɂ�UV���W����������(�������_�ł��ʂ�UV���W����������̂�)
//�v���~�e�B�u�g�|���W�[�͒P���ɎO�p�`�����x���`�悷����

//�����炭�����ǃC���X�^���X�`��͒��_�̐����������̂��������ɕ`��ł��Ȃ�
//�肭�炢�O�p�`���g���Έ�̃X�g���b�v�ł������̗��ꗣ��̐}�`��`��ł���I
//����͂܂�ŏ��ɍŏ��̒��_��1�񏑂����ݏI��������_�ōŌ�ɍŌ�̒��_������1�񏑂����߂΂悢

/*
struct StripVertexType {
public:
	DirectX::XMFLOAT3 Pos;
	DirectX::XMFLOAT2 UV;
};
class StripVertexBuffer {
public:
	void UpdateAndSet();
	int UsedCount;
	StripVertexType* DataList;
	ComPtr<ID3D11Buffer> Buffer;
	static StripVertexBuffer& GetInstance() {
		if (pThis == nullptr) {
			pThis = new StripVertexBuffer();
		}
		return *pThis;
	}
private:
	StripVertexBuffer();
	static StripVertexBuffer* pThis;
};
#define sBuffer StripVertexBuffer::GetInstance()
*/
class RectDrawCallBuffer{
public:
	RectDrawCallBuffer();
	void UpdateAndSet();
	int UsedCount;
	RectDrawCallType* DataList;
	ComPtr<ID3D11Buffer> Buffer;
};
class RectInstanceBuffer{
public:
	RectInstanceBuffer();
	void UpdateAndSet();
	int UsedCount;
	RectInstanceType* DataList;
	ComPtr<ID3D11Buffer> Buffer;
};
/*
class ColorVarRectDrawCallBuffer {
public:
	ColorVarRectDrawCallBuffer();
	void UpdateAndSet();
	int UsedCount;
	ColorVarRectDrawCallType* DataList;
	ComPtr<ID3D11Buffer> Buffer;
};
class ColorVarRectInstanceBuffer {
public:
	ColorVarRectInstanceBuffer();
	void UpdateAndSet();
	int UsedCount;
	ColorVarRectInstanceType* DataList;
	ComPtr<ID3D11Buffer> Buffer;
};
*/
/*
class IndexBuffer {
public:
	IndexBuffer();
	void Initialize();
	void UpdateAndSet();
	int UsedCount;
	WORD* DataList;
	ComPtr<ID3D11Buffer> Buffer;
};
*/
class ConstantBuffer {
public:
	ConstantBuffer();
	void UpdateAndSet();
	ConstantType Data;
	ComPtr<ID3D11Buffer> Buffer;
};
/*
struct LightBufferData {
public:
	DirectX::XMFLOAT4 LightPos;
	DirectX::XMFLOAT4 LightColor;
};
class LightBuffer {
public:
	void UpdateAndSet();
	int UsedCount;
	LightBufferData* DataList;
	ComPtr<ID3D11Buffer> Buffer;
	ComPtr<ID3D11ShaderResourceView> SRV;
	static LightBuffer& GetInstance() {
		if (pThis == nullptr) {
			pThis = new LightBuffer();
		}
		return *pThis;
	}
private:
	LightBuffer();
	static LightBuffer* pThis;
};
#define lBuffer LightBuffer::GetInstance()
*/
class AllBuffers : IAllGraphicBuffer {
public:
	RectDrawCallBuffer RectDCBuffer;
	RectInstanceBuffer RectIBuffer;
	AllBuffers() {
		RectDCBuffer = RectDrawCallBuffer();
		RectIBuffer = RectInstanceBuffer();
	}

	RectDrawCallType* GetNextRectDCPointer() override;
	RectInstanceType* GetNextRectIPointer() override;
};