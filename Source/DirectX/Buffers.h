#pragma once
#include "framework.h"
#include "Source/DirectX/DirectX.h"

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
struct RectVertexType {
public:
	DirectX::XMFLOAT2 UV;
	DirectX::XMFLOAT3 Normal;
	DirectX::XMFLOAT3 Pos;
};
class RectVertexBuffer {
public:
	void UpdateAndSet();
	int UsedCount;
	RectVertexType* DataList;
	ComPtr<ID3D11Buffer> Buffer;
	static RectVertexBuffer& GetInstance() {
		if (pThis == nullptr) {
			pThis = new RectVertexBuffer();
		}
		return *pThis;
	}
private:
	RectVertexBuffer();
	static RectVertexBuffer* pThis;
};
#define rBuffer RectVertexBuffer::GetInstance()
struct PositionType {
public:
	DirectX::XMFLOAT4 Row1;
	DirectX::XMFLOAT4 Row2;
	DirectX::XMFLOAT4 Row3;
	DirectX::XMFLOAT4 Row4;
	void Set(DirectX::XMMATRIX input);
	void Set(DirectX::XMMATRIX* input);
};
class PositionBuffer {
public:
	PositionBuffer();
	void UpdateAndSet();
	int UsedCount;
	PositionType* DataList;
	ComPtr<ID3D11Buffer> Buffer;
	static PositionBuffer& GetInstance() {
		if (pThis == nullptr) {
			pThis = new PositionBuffer();
		}
		return *pThis;
	}
private:
	PositionBuffer();
	static PositionBuffer* pThis;
};
#define pBuffer PositionBuffer::GetInstance()
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
struct ConstantData
{
public:
	DirectX::XMFLOAT2 ViewProjection;
};
class ConstantBuffer {
public:
	ConstantBuffer();
	void UpdateAndSet();
	ConstantData Data;
	ComPtr<ID3D11Buffer> Buffer;
	static ConstantBuffer& GetInstance() {
		if (pThis == nullptr) {
			pThis = new ConstantBuffer();
		}
		return *pThis;
	}
private:
	ConstantBuffer();
	static ConstantBuffer* pThis;
};
#define cBuffer ConstantBuffer::GetInstance()
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