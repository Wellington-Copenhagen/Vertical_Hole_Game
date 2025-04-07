#pragma once
#include "Interfaces.h"
#include "Source/DirectX/DirectX.h"

//�`��p�C�v���C���ɂ�����o�b�t�@�̐ݒ�
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
template<typename DataType>
class VertexBuffer {
public:
	int Slot;
	VertexBuffer(){}
	VertexBuffer(DataType* pData, int maxCount, int slot) {
		Slot = slot;
		D3D11_BUFFER_DESC bDesc = {};
		bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;	// �f�o�C�X�Ƀo�C���h����Ƃ��̎��(���_�o�b�t�@�A�C���f�b�N�X�o�b�t�@�A�萔�o�b�t�@�Ȃ�)
		bDesc.ByteWidth = sizeof(DataType) * maxCount;					// �쐬����o�b�t�@�̃o�C�g�T�C�Y
		//�N�����Ɋm�ۂ��Ďg���܂킷�ꍇ�����̃T�C�Y�����炩���ߌ��߂�K�v������A���ꂪ�`��\�ʂ̌��E�ƂȂ�
		bDesc.MiscFlags = 0;							// ���̑��̃t���O
		bDesc.StructureByteStride = 0;					// �\�����o�b�t�@�̏ꍇ�A���̍\���̂̃T�C�Y
		bDesc.Usage = D3D11_USAGE_DYNAMIC;				// �쐬����o�b�t�@�̎g�p�@
		bDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		// ��̎d�l��n���Ē��_�o�b�t�@������Ă��炤
		// �������f�o�C�X����ɂ��肢����
		// ���_�o�b�t�@�ɂ��Ă͍ŏ��̕ϐ��ȊO�͊֌W�Ȃ��炵���H
		D3D11_SUBRESOURCE_DATA initData = { pData, 0, 0 };	// �������ރf�[�^
		// ���_�o�b�t�@�̍쐬
		if (FAILED(D3D.m_device->CreateBuffer(&bDesc, &initData, &Buffer))) {
			throw("");
		}

		// ���_�o�b�t�@��`��Ŏg����悤�ɃZ�b�g����
		UINT stride = sizeof(DataType);
		UINT offset = 0;
		//���_�o�b�t�@�̂������Ԃ���(��1����)����(��2����)���g���̂�
		//vb�͔z��̐擪�̃A�h���X���w�����ƂɂȂ�
		//stride��offset���e���_�o�b�t�@���ƂɎw�肷�邱�ƂɂȂ�̂ł������z��
		D3D.m_deviceContext->IASetVertexBuffers(Slot, 1, Buffer.GetAddressOf(), &stride, &offset);
	}
	void UpdateAndSet(DataType* pData, int offset, int count) {
		D3D11_MAPPED_SUBRESOURCE mappedSubresource;
		//������GPU���璸�_�o�b�t�@�ւ̃A�N�Z�X���~�߁ACPU����̃A�N�Z�X���ł���悤�ɂ��Ă���
		if (FAILED(D3D.m_deviceContext->Map(Buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource))) {
			throw("");
		}
		//������CPU�ɂ�鏈���Ńo�b�t�@�̓��e��ς��Ă���
		memcpy(mappedSubresource.pData, pData, sizeof(DataType) * count);//��3�ϐ���ς��邱�Ƃŏ��������鐔��ς�����
		//������GPU�ɃA�N�Z�X�������Ă�
		D3D.m_deviceContext->Unmap(Buffer.Get(), 0);
		// ���_�o�b�t�@��`��Ŏg����悤�ɃZ�b�g����
		UINT stride = sizeof(DataType);
		UINT uOffset = (UINT)offset;
		//���_�o�b�t�@�̂������Ԃ���(��1����)����(��2����)���g���̂�
		//vb�͔z��̐擪�̃A�h���X���w�����ƂɂȂ�
		//stride��offset���e���_�o�b�t�@���ƂɎw�肷�邱�ƂɂȂ�̂ł������z��
		D3D.m_deviceContext->IASetVertexBuffers(Slot, 1, Buffer.GetAddressOf(), &stride, &uOffset);
	}
	ComPtr<ID3D11Buffer> Buffer;
};
/*
class ColorVarRectDrawCallBuffer {
public:
	ColorVarRectDrawCallBuffer();
	void UpdateAndSet();
	int UsedCount;
	ColorVarInterface::RectDrawCallType* DataList;
	ComPtr<ID3D11Buffer> Buffer;
};
class ColorVarRectInstanceBuffer {
public:
	ColorVarRectInstanceBuffer();
	void UpdateAndSet();
	int UsedCount;
	ColorVarInterface::RectInstanceType* DataList;
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
	Interface::ConstantType Data;
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
class AllBuffers : IAllGraphicBuffer {
public:
	RectDrawCallBuffer RectDCBuffer;
	RectInstanceBuffer RectIBuffer;
	AllBuffers() {
		RectDCBuffer = RectDrawCallBuffer();
		RectIBuffer = RectInstanceBuffer();
	}

	Interface::RectDrawCallType* GetNextRectDCPointer() override;
	Interface::RectInstanceType* GetNextRectIPointer() override;
};
*/