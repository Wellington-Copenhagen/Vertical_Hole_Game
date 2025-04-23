
#include "framework.h"
#include "Source/DirectX/DirectX.h"
#include "Buffers.h"
/*
StripDrawCallBuffer::StripDrawCallBuffer() {
	DataList = new StripDrawCallType[BLENGTHVERTEX];
	D3D11_BUFFER_DESC bDesc = {};
	bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;	// �f�o�C�X�Ƀo�C���h����Ƃ��̎��(���_�o�b�t�@�A�C���f�b�N�X�o�b�t�@�A�萔�o�b�t�@�Ȃ�)
	bDesc.ByteWidth = sizeof(StripDrawCallType)*BLENGTHVERTEX;					// �쐬����o�b�t�@�̃o�C�g�T�C�Y
	//�N�����Ɋm�ۂ��Ďg���܂킷�ꍇ�����̃T�C�Y�����炩���ߌ��߂�K�v������A���ꂪ�`��\�ʂ̌��E�ƂȂ�
	bDesc.MiscFlags = 0;							// ���̑��̃t���O
	bDesc.StructureByteStride = 0;					// �\�����o�b�t�@�̏ꍇ�A���̍\���̂̃T�C�Y
	bDesc.Usage = D3D11_USAGE_DYNAMIC;				// �쐬����o�b�t�@�̎g�p�@
	bDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	// ��̎d�l��n���Ē��_�o�b�t�@������Ă��炤
	// �������f�o�C�X����ɂ��肢����
	// ���_�o�b�t�@�ɂ��Ă͍ŏ��̕ϐ��ȊO�͊֌W�Ȃ��炵���H
	D3D11_SUBRESOURCE_DATA initData = { DataList, sizeof(StripDrawCallType)*UsedCount, 0 };	// �������ރf�[�^
	// ���_�o�b�t�@�̍쐬
	if (FAILED(D3D.m_device->CreateBuffer(&bDesc, &initData, &Buffer))) {
		return;
	}
	
	// ���_�o�b�t�@��`��Ŏg����悤�ɃZ�b�g����
	UINT stride = sizeof(StripDrawCallType);
	UINT offset = 0;
	//���_�o�b�t�@�̂������Ԃ���(��1����)����(��2����)���g���̂�
	//vb�͔z��̐擪�̃A�h���X���w�����ƂɂȂ�
	//stride��offset���e���_�o�b�t�@���ƂɎw�肷�邱�ƂɂȂ�̂ł������z��
	D3D.m_deviceContext->IASetDrawCallBuffers(0, 1, Buffer.GetAddressOf(), &stride, &offset);
}
void StripDrawCallBuffer::UpdateAndSet() {
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	//������GPU���璸�_�o�b�t�@�ւ̃A�N�Z�X���~�߁ACPU����̃A�N�Z�X���ł���悤�ɂ��Ă���
	if (FAILED(D3D.m_deviceContext->Map(Buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource))) {
		return;
	}
	//������CPU�ɂ�鏈���Ńo�b�t�@�̓��e��ς��Ă���
	memcpy(mappedSubresource.pData, DataList, sizeof(StripDrawCallType) * UsedCount);//��3�ϐ���ς��邱�Ƃŏ��������鐔��ς�����
	//������GPU�ɃA�N�Z�X�������Ă�
	D3D.m_deviceContext->Unmap(Buffer.Get(), 0);
	// ���_�o�b�t�@��`��Ŏg����悤�ɃZ�b�g����
	UINT stride = sizeof(StripDrawCallType);
	UINT offset = 0;
	//���_�o�b�t�@�̂������Ԃ���(��1����)����(��2����)���g���̂�
	//vb�͔z��̐擪�̃A�h���X���w�����ƂɂȂ�
	//stride��offset���e���_�o�b�t�@���ƂɎw�肷�邱�ƂɂȂ�̂ł������z��
	D3D.m_deviceContext->IASetDrawCallBuffers(0, 1, Buffer.GetAddressOf(), &stride, &offset);
}
*/



/*
ColorVarRectDrawCallBuffer::ColorVarRectDrawCallBuffer() {
	DataList = new ColorVarInterface::RectDrawCallType[BLENGTHVERTEX];
	D3D11_BUFFER_DESC bDesc = {};
	bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;	// �f�o�C�X�Ƀo�C���h����Ƃ��̎��(���_�o�b�t�@�A�C���f�b�N�X�o�b�t�@�A�萔�o�b�t�@�Ȃ�)
	bDesc.ByteWidth = sizeof(ColorVarInterface::RectDrawCallType) * BLENGTHVERTEX;					// �쐬����o�b�t�@�̃o�C�g�T�C�Y
	//�N�����Ɋm�ۂ��Ďg���܂킷�ꍇ�����̃T�C�Y�����炩���ߌ��߂�K�v������A���ꂪ�`��\�ʂ̌��E�ƂȂ�
	bDesc.MiscFlags = 0;							// ���̑��̃t���O
	bDesc.StructureByteStride = 0;					// �\�����o�b�t�@�̏ꍇ�A���̍\���̂̃T�C�Y
	bDesc.Usage = D3D11_USAGE_DYNAMIC;				// �쐬����o�b�t�@�̎g�p�@
	bDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	// ��̎d�l��n���Ē��_�o�b�t�@������Ă��炤
	// �������f�o�C�X����ɂ��肢����
	// ���_�o�b�t�@�ɂ��Ă͍ŏ��̕ϐ��ȊO�͊֌W�Ȃ��炵���H
	D3D11_SUBRESOURCE_DATA initData = { DataList, sizeof(ColorVarInterface::RectDrawCallType) * UsedCount, 0 };	// �������ރf�[�^
	// ���_�o�b�t�@�̍쐬
	if (FAILED(D3D.m_device->CreateBuffer(&bDesc, &initData, &Buffer))) {
		return;
	}

	// ���_�o�b�t�@��`��Ŏg����悤�ɃZ�b�g����
	UINT stride = sizeof(ColorVarInterface::RectDrawCallType);
	UINT offset = 0;
	//���_�o�b�t�@�̂������Ԃ���(��1����)����(��2����)���g���̂�
	//vb�͔z��̐擪�̃A�h���X���w�����ƂɂȂ�
	//stride��offset���e���_�o�b�t�@���ƂɎw�肷�邱�ƂɂȂ�̂ł������z��
	D3D.m_deviceContext->IASetDrawCallBuffers(0, 1, Buffer.GetAddressOf(), &stride, &offset);
}
void ColorVarRectDrawCallBuffer::UpdateAndSet() {
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	//������GPU���璸�_�o�b�t�@�ւ̃A�N�Z�X���~�߁ACPU����̃A�N�Z�X���ł���悤�ɂ��Ă���
	if (FAILED(D3D.m_deviceContext->Map(Buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource))) {
		return;
	}
	//������CPU�ɂ�鏈���Ńo�b�t�@�̓��e��ς��Ă���
	memcpy(mappedSubresource.pData, DataList, sizeof(ColorVarInterface::RectDrawCallType) * UsedCount);//��3�ϐ���ς��邱�Ƃŏ��������鐔��ς�����
	//������GPU�ɃA�N�Z�X�������Ă�
	D3D.m_deviceContext->Unmap(Buffer.Get(), 0);
	// ���_�o�b�t�@��`��Ŏg����悤�ɃZ�b�g����
	UINT stride = sizeof(ColorVarInterface::RectDrawCallType);
	UINT offset = 0;
	//���_�o�b�t�@�̂������Ԃ���(��1����)����(��2����)���g���̂�
	//vb�͔z��̐擪�̃A�h���X���w�����ƂɂȂ�
	//stride��offset���e���_�o�b�t�@���ƂɎw�肷�邱�ƂɂȂ�̂ł������z��
	D3D.m_deviceContext->IASetDrawCallBuffers(0, 1, Buffer.GetAddressOf(), &stride, &offset);
}
*/



/*
ColorVarRectInstanceBuffer::ColorVarRectInstanceBuffer() {
	DataList = new ColorVarInterface::RectInstanceType[BLENGTHINSTANCE];
	D3D11_BUFFER_DESC bDesc = {};
	bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;	// �f�o�C�X�Ƀo�C���h����Ƃ��̎��(���_�o�b�t�@�A�C���f�b�N�X�o�b�t�@�A�萔�o�b�t�@�Ȃ�)
	bDesc.ByteWidth = sizeof(ColorVarInterface::RectInstanceType) * BLENGTHINSTANCE;					// �쐬����o�b�t�@�̃o�C�g�T�C�Y
	//�N�����Ɋm�ۂ��Ďg���܂킷�ꍇ�����̃T�C�Y�����炩���ߌ��߂�K�v������A���ꂪ�`��\�ʂ̌��E�ƂȂ�
	bDesc.MiscFlags = 0;							// ���̑��̃t���O
	bDesc.StructureByteStride = 0;					// �\�����o�b�t�@�̏ꍇ�A���̍\���̂̃T�C�Y
	bDesc.Usage = D3D11_USAGE_DYNAMIC;				// �쐬����o�b�t�@�̎g�p�@
	bDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	// ��̎d�l��n���Ē��_�o�b�t�@������Ă��炤
	// �������f�o�C�X����ɂ��肢����
	// ���_�o�b�t�@�ɂ��Ă͍ŏ��̕ϐ��ȊO�͊֌W�Ȃ��炵���H
	D3D11_SUBRESOURCE_DATA initData = { DataList, sizeof(ColorVarInterface::RectInstanceType) * UsedCount, 0 };	// �������ރf�[�^
	// ���_�o�b�t�@�̍쐬
	if (FAILED(D3D.m_device->CreateBuffer(&bDesc, &initData, &Buffer))) {
		return;
	}

	// ���_�o�b�t�@��`��Ŏg����悤�ɃZ�b�g����
	UINT stride = sizeof(ColorVarInterface::RectInstanceType);
	UINT offset = 0;
	//���_�o�b�t�@�̂������Ԃ���(��1����)����(��2����)���g���̂�
	//vb�͔z��̐擪�̃A�h���X���w�����ƂɂȂ�
	//stride��offset���e���_�o�b�t�@���ƂɎw�肷�邱�ƂɂȂ�̂ł������z��
	D3D.m_deviceContext->IASetDrawCallBuffers(1, 1, Buffer.GetAddressOf(), &stride, &offset);
}
void ColorVarRectInstanceBuffer::UpdateAndSet() {
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	//������GPU���璸�_�o�b�t�@�ւ̃A�N�Z�X���~�߁ACPU����̃A�N�Z�X���ł���悤�ɂ��Ă���
	if (FAILED(D3D.m_deviceContext->Map(Buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource))) {
		return;
	}
	//������CPU�ɂ�鏈���Ńo�b�t�@�̓��e��ς��Ă���
	for (int i = 0; i < UsedCount; i++) {
		ColorVarInterface::RectInstanceType temp = DataList[i];
		int a = 1;
		a = a + 1;
	}
	memcpy(mappedSubresource.pData, DataList, sizeof(ColorVarInterface::RectInstanceType) * UsedCount);//��3�ϐ���ς��邱�Ƃŏ��������鐔��ς�����
	//������GPU�ɃA�N�Z�X�������Ă�
	D3D.m_deviceContext->Unmap(Buffer.Get(), 0);
	// ���_�o�b�t�@��`��Ŏg����悤�ɃZ�b�g����
	UINT stride = sizeof(ColorVarInterface::RectInstanceType);
	UINT offset = 0;
	//���_�o�b�t�@�̂������Ԃ���(��1����)����(��2����)���g���̂�
	//vb�͔z��̐擪�̃A�h���X���w�����ƂɂȂ�
	//stride��offset���e���_�o�b�t�@���ƂɎw�肷�邱�ƂɂȂ�̂ł������z��
	D3D.m_deviceContext->IASetDrawCallBuffers(1, 1, Buffer.GetAddressOf(), &stride, &offset);
}
*/




/*
IndexBuffer::IndexBuffer() {

}
void IndexBuffer::Initialize() {
	DataList = new WORD[BLENGTHINDEX];
	D3D11_BUFFER_DESC bDesc = {};
	bDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;	// �f�o�C�X�Ƀo�C���h����Ƃ��̎��(���_�o�b�t�@�A�C���f�b�N�X�o�b�t�@�A�萔�o�b�t�@�Ȃ�)
	bDesc.ByteWidth = sizeof(WORD) * BLENGTHINDEX;					// �쐬����o�b�t�@�̃o�C�g�T�C�Y
	//�N�����Ɋm�ۂ��Ďg���܂킷�ꍇ�����̃T�C�Y�����炩���ߌ��߂�K�v������A���ꂪ�`��\�ʂ̌��E�ƂȂ�
	bDesc.MiscFlags = 0;							// ���̑��̃t���O
	bDesc.StructureByteStride = 0;					// �\�����o�b�t�@�̏ꍇ�A���̍\���̂̃T�C�Y
	bDesc.Usage = D3D11_USAGE_DYNAMIC;				// �쐬����o�b�t�@�̎g�p�@
	bDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	// ��̎d�l��n���Ē��_�o�b�t�@������Ă��炤
	// �������f�o�C�X����ɂ��肢����
	// ���_�o�b�t�@�ɂ��Ă͍ŏ��̕ϐ��ȊO�͊֌W�Ȃ��炵���H
	D3D11_SUBRESOURCE_DATA initData = { DataList, sizeof(WORD) * UsedCount, 0 };	// �������ރf�[�^
	if (FAILED(D3D.m_device->CreateBuffer(&bDesc, &initData, &Buffer))) {
		return;
	}
	D3D.m_deviceContext->IASetIndexBuffer(Buffer.Get(), DXGI_FORMAT_R16_UINT, 0);
}
void IndexBuffer::UpdateAndSet() {
	D3D11_MAPPED_SUBRESOURCE mappedSubresource = { };
	//������GPU���璸�_�o�b�t�@�ւ̃A�N�Z�X���~�߁ACPU����̃A�N�Z�X���ł���悤�ɂ��Ă���
	if (FAILED(D3D.m_deviceContext->Map(Buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource))) {
		return;
	}
	//������CPU�ɂ�鏈���Ńo�b�t�@�̓��e��ς��Ă���
	memcpy(mappedSubresource.pData, DataList, sizeof(WORD) * UsedCount);//��3�ϐ���ς��邱�Ƃŏ��������鐔��ς�����
	//������GPU�ɃA�N�Z�X�������Ă�
	D3D.m_deviceContext->Unmap(Buffer.Get(), 0);
	D3D.m_deviceContext->IASetIndexBuffer(Buffer.Get(), DXGI_FORMAT_R16_UINT, 0);
}
*/
ConstantBuffer::ConstantBuffer() {
	D3D11_BUFFER_DESC bDesc = {};
	bDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;	// �f�o�C�X�Ƀo�C���h����Ƃ��̎��(���_�o�b�t�@�A�C���f�b�N�X�o�b�t�@�A�萔�o�b�t�@�Ȃ�)
	bDesc.ByteWidth = sizeof(Data);					// �쐬����o�b�t�@�̃o�C�g�T�C�Y
	//�N�����Ɋm�ۂ��Ďg���܂킷�ꍇ�����̃T�C�Y�����炩���ߌ��߂�K�v������A���ꂪ�`��\�ʂ̌��E�ƂȂ�
	bDesc.MiscFlags = 0;							// ���̑��̃t���O
	bDesc.StructureByteStride = 0;					// �\�����o�b�t�@�̏ꍇ�A���̍\���̂̃T�C�Y
	bDesc.Usage = D3D11_USAGE_DYNAMIC;				// �쐬����o�b�t�@�̎g�p�@
	bDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	// ��̎d�l��n���Ē��_�o�b�t�@������Ă��炤
	// �������f�o�C�X����ɂ��肢����
	// ���_�o�b�t�@�ɂ��Ă͍ŏ��̕ϐ��ȊO�͊֌W�Ȃ��炵���H
	D3D11_SUBRESOURCE_DATA initData = { &Data, sizeof(Data), 0 };	// �������ރf�[�^
	initData.pSysMem = &Data;
	initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;
	if (FAILED(D3D.m_device->CreateBuffer(&bDesc, &initData, &Buffer))) {
		throw("");
	}
	D3D.m_deviceContext->VSSetConstantBuffers(0, 1, Buffer.GetAddressOf());
}
void ConstantBuffer::UpdateAndSet(DirectX::XMMATRIX* pViewProjection, std::vector<DirectX::XMFLOAT4>* pBlackBoxMatrices) {
	if (pViewProjection != nullptr) {
		DirectX::XMStoreFloat4x4(&Data.ViewProjection, *pViewProjection);
	}
	if (pBlackBoxMatrices != nullptr) {
		memcpy_s(&Data.BlackBox, sizeof(DirectX::XMFLOAT4) * 2048, &(*pBlackBoxMatrices)[0], sizeof(DirectX::XMFLOAT4) * pBlackBoxMatrices->size());
	}
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	//������GPU���璸�_�o�b�t�@�ւ̃A�N�Z�X���~�߁ACPU����̃A�N�Z�X���ł���悤�ɂ��Ă���
	if (FAILED(D3D.m_deviceContext->Map(Buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource))) {
		throw("");
	}
	//������CPU�ɂ�鏈���Ńo�b�t�@�̓��e��ς��Ă���
	memcpy(mappedSubresource.pData, &Data , sizeof(Data));//��3�ϐ���ς��邱�Ƃŏ��������鐔��ς�����
	//������GPU�ɃA�N�Z�X�������Ă�
	D3D.m_deviceContext->Unmap(Buffer.Get(), 0);
	D3D.m_deviceContext->VSSetConstantBuffers(0, 1, Buffer.GetAddressOf());
}



/*
LightBuffer::LightBuffer() {
	DataList = new LightBufferData[BLENGTHLIGHT];
	D3D11_BUFFER_DESC bDesc = {};
	bDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;	// �f�o�C�X�Ƀo�C���h����Ƃ��̎��(���_�o�b�t�@�A�C���f�b�N�X�o�b�t�@�A�萔�o�b�t�@�Ȃ�)
	bDesc.ByteWidth = sizeof(LightBufferData) * BLENGTHLIGHT;					// �쐬����o�b�t�@�̃o�C�g�T�C�Y
	//�N�����Ɋm�ۂ��Ďg���܂킷�ꍇ�����̃T�C�Y�����炩���ߌ��߂�K�v������A���ꂪ�`��\�ʂ̌��E�ƂȂ�
	bDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;							// ���̑��̃t���O
	bDesc.StructureByteStride = sizeof(LightBufferData);					// �\�����o�b�t�@�̏ꍇ�A���̍\���̂̃T�C�Y
	bDesc.Usage = D3D11_USAGE_DYNAMIC;				// �쐬����o�b�t�@�̎g�p�@
	bDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA initData = { DataList, sizeof(LightBufferData) * UsedCount, 0 };	// �������ރf�[�^
	if (FAILED(D3D.m_device->CreateBuffer(&bDesc, &initData, &Buffer))) {
		return;
	}


	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = UsedCount;
	srvDesc.Buffer.ElementOffset = 0;
	srvDesc.Buffer.ElementWidth = sizeof(LightBufferData);
	if (FAILED(D3D.m_device->CreateShaderResourceView(Buffer.Get(), &srvDesc, SRV.GetAddressOf()))) {
		return;
	}
	
	D3D.m_deviceContext->VSSetShaderResources(0, 1, SRV.GetAddressOf());
}
void LightBuffer::UpdateAndSet() {
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	//������GPU���璸�_�o�b�t�@�ւ̃A�N�Z�X���~�߁ACPU����̃A�N�Z�X���ł���悤�ɂ��Ă���
	if (FAILED(D3D.m_deviceContext->Map(Buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource))) {
		return;
	}
	//������CPU�ɂ�鏈���Ńo�b�t�@�̓��e��ς��Ă���
	memcpy(mappedSubresource.pData, DataList, sizeof(LightBufferData) * UsedCount);//��3�ϐ���ς��邱�Ƃŏ��������鐔��ς�����
	//������GPU�ɃA�N�Z�X�������Ă�
	D3D.m_deviceContext->Unmap(Buffer.Get(), 0);
	D3D.m_deviceContext->VSSetShaderResources(0, 1, SRV.GetAddressOf());
}
*/