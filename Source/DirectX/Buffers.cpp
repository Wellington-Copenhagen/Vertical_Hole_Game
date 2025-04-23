
#include "framework.h"
#include "Source/DirectX/DirectX.h"
#include "Buffers.h"
/*
StripDrawCallBuffer::StripDrawCallBuffer() {
	DataList = new StripDrawCallType[BLENGTHVERTEX];
	D3D11_BUFFER_DESC bDesc = {};
	bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;	// デバイスにバインドするときの種類(頂点バッファ、インデックスバッファ、定数バッファなど)
	bDesc.ByteWidth = sizeof(StripDrawCallType)*BLENGTHVERTEX;					// 作成するバッファのバイトサイズ
	//起動時に確保して使いまわす場合ここのサイズをあらかじめ決める必要があり、それが描画可能量の限界となる
	bDesc.MiscFlags = 0;							// その他のフラグ
	bDesc.StructureByteStride = 0;					// 構造化バッファの場合、その構造体のサイズ
	bDesc.Usage = D3D11_USAGE_DYNAMIC;				// 作成するバッファの使用法
	bDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	// 上の仕様を渡して頂点バッファを作ってもらう
	// もちろんデバイスさんにお願いする
	// 頂点バッファについては最初の変数以外は関係ないらしい？
	D3D11_SUBRESOURCE_DATA initData = { DataList, sizeof(StripDrawCallType)*UsedCount, 0 };	// 書き込むデータ
	// 頂点バッファの作成
	if (FAILED(D3D.m_device->CreateBuffer(&bDesc, &initData, &Buffer))) {
		return;
	}
	
	// 頂点バッファを描画で使えるようにセットする
	UINT stride = sizeof(StripDrawCallType);
	UINT offset = 0;
	//頂点バッファのうち何番から(第1引数)何個を(第2引数)を使うのか
	//vbは配列の先頭のアドレスを指すことになる
	//strideとoffsetも各頂点バッファごとに指定することになるのでこれらも配列
	D3D.m_deviceContext->IASetDrawCallBuffers(0, 1, Buffer.GetAddressOf(), &stride, &offset);
}
void StripDrawCallBuffer::UpdateAndSet() {
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	//ここでGPUから頂点バッファへのアクセスを止め、CPUからのアクセスができるようにしている
	if (FAILED(D3D.m_deviceContext->Map(Buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource))) {
		return;
	}
	//ここでCPUによる処理でバッファの内容を変えている
	memcpy(mappedSubresource.pData, DataList, sizeof(StripDrawCallType) * UsedCount);//第3変数を変えることで書き換える数を変えられる
	//ここでGPUにアクセスを許可してる
	D3D.m_deviceContext->Unmap(Buffer.Get(), 0);
	// 頂点バッファを描画で使えるようにセットする
	UINT stride = sizeof(StripDrawCallType);
	UINT offset = 0;
	//頂点バッファのうち何番から(第1引数)何個を(第2引数)を使うのか
	//vbは配列の先頭のアドレスを指すことになる
	//strideとoffsetも各頂点バッファごとに指定することになるのでこれらも配列
	D3D.m_deviceContext->IASetDrawCallBuffers(0, 1, Buffer.GetAddressOf(), &stride, &offset);
}
*/



/*
ColorVarRectDrawCallBuffer::ColorVarRectDrawCallBuffer() {
	DataList = new ColorVarInterface::RectDrawCallType[BLENGTHVERTEX];
	D3D11_BUFFER_DESC bDesc = {};
	bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;	// デバイスにバインドするときの種類(頂点バッファ、インデックスバッファ、定数バッファなど)
	bDesc.ByteWidth = sizeof(ColorVarInterface::RectDrawCallType) * BLENGTHVERTEX;					// 作成するバッファのバイトサイズ
	//起動時に確保して使いまわす場合ここのサイズをあらかじめ決める必要があり、それが描画可能量の限界となる
	bDesc.MiscFlags = 0;							// その他のフラグ
	bDesc.StructureByteStride = 0;					// 構造化バッファの場合、その構造体のサイズ
	bDesc.Usage = D3D11_USAGE_DYNAMIC;				// 作成するバッファの使用法
	bDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	// 上の仕様を渡して頂点バッファを作ってもらう
	// もちろんデバイスさんにお願いする
	// 頂点バッファについては最初の変数以外は関係ないらしい？
	D3D11_SUBRESOURCE_DATA initData = { DataList, sizeof(ColorVarInterface::RectDrawCallType) * UsedCount, 0 };	// 書き込むデータ
	// 頂点バッファの作成
	if (FAILED(D3D.m_device->CreateBuffer(&bDesc, &initData, &Buffer))) {
		return;
	}

	// 頂点バッファを描画で使えるようにセットする
	UINT stride = sizeof(ColorVarInterface::RectDrawCallType);
	UINT offset = 0;
	//頂点バッファのうち何番から(第1引数)何個を(第2引数)を使うのか
	//vbは配列の先頭のアドレスを指すことになる
	//strideとoffsetも各頂点バッファごとに指定することになるのでこれらも配列
	D3D.m_deviceContext->IASetDrawCallBuffers(0, 1, Buffer.GetAddressOf(), &stride, &offset);
}
void ColorVarRectDrawCallBuffer::UpdateAndSet() {
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	//ここでGPUから頂点バッファへのアクセスを止め、CPUからのアクセスができるようにしている
	if (FAILED(D3D.m_deviceContext->Map(Buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource))) {
		return;
	}
	//ここでCPUによる処理でバッファの内容を変えている
	memcpy(mappedSubresource.pData, DataList, sizeof(ColorVarInterface::RectDrawCallType) * UsedCount);//第3変数を変えることで書き換える数を変えられる
	//ここでGPUにアクセスを許可してる
	D3D.m_deviceContext->Unmap(Buffer.Get(), 0);
	// 頂点バッファを描画で使えるようにセットする
	UINT stride = sizeof(ColorVarInterface::RectDrawCallType);
	UINT offset = 0;
	//頂点バッファのうち何番から(第1引数)何個を(第2引数)を使うのか
	//vbは配列の先頭のアドレスを指すことになる
	//strideとoffsetも各頂点バッファごとに指定することになるのでこれらも配列
	D3D.m_deviceContext->IASetDrawCallBuffers(0, 1, Buffer.GetAddressOf(), &stride, &offset);
}
*/



/*
ColorVarRectInstanceBuffer::ColorVarRectInstanceBuffer() {
	DataList = new ColorVarInterface::RectInstanceType[BLENGTHINSTANCE];
	D3D11_BUFFER_DESC bDesc = {};
	bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;	// デバイスにバインドするときの種類(頂点バッファ、インデックスバッファ、定数バッファなど)
	bDesc.ByteWidth = sizeof(ColorVarInterface::RectInstanceType) * BLENGTHINSTANCE;					// 作成するバッファのバイトサイズ
	//起動時に確保して使いまわす場合ここのサイズをあらかじめ決める必要があり、それが描画可能量の限界となる
	bDesc.MiscFlags = 0;							// その他のフラグ
	bDesc.StructureByteStride = 0;					// 構造化バッファの場合、その構造体のサイズ
	bDesc.Usage = D3D11_USAGE_DYNAMIC;				// 作成するバッファの使用法
	bDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	// 上の仕様を渡して頂点バッファを作ってもらう
	// もちろんデバイスさんにお願いする
	// 頂点バッファについては最初の変数以外は関係ないらしい？
	D3D11_SUBRESOURCE_DATA initData = { DataList, sizeof(ColorVarInterface::RectInstanceType) * UsedCount, 0 };	// 書き込むデータ
	// 頂点バッファの作成
	if (FAILED(D3D.m_device->CreateBuffer(&bDesc, &initData, &Buffer))) {
		return;
	}

	// 頂点バッファを描画で使えるようにセットする
	UINT stride = sizeof(ColorVarInterface::RectInstanceType);
	UINT offset = 0;
	//頂点バッファのうち何番から(第1引数)何個を(第2引数)を使うのか
	//vbは配列の先頭のアドレスを指すことになる
	//strideとoffsetも各頂点バッファごとに指定することになるのでこれらも配列
	D3D.m_deviceContext->IASetDrawCallBuffers(1, 1, Buffer.GetAddressOf(), &stride, &offset);
}
void ColorVarRectInstanceBuffer::UpdateAndSet() {
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	//ここでGPUから頂点バッファへのアクセスを止め、CPUからのアクセスができるようにしている
	if (FAILED(D3D.m_deviceContext->Map(Buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource))) {
		return;
	}
	//ここでCPUによる処理でバッファの内容を変えている
	for (int i = 0; i < UsedCount; i++) {
		ColorVarInterface::RectInstanceType temp = DataList[i];
		int a = 1;
		a = a + 1;
	}
	memcpy(mappedSubresource.pData, DataList, sizeof(ColorVarInterface::RectInstanceType) * UsedCount);//第3変数を変えることで書き換える数を変えられる
	//ここでGPUにアクセスを許可してる
	D3D.m_deviceContext->Unmap(Buffer.Get(), 0);
	// 頂点バッファを描画で使えるようにセットする
	UINT stride = sizeof(ColorVarInterface::RectInstanceType);
	UINT offset = 0;
	//頂点バッファのうち何番から(第1引数)何個を(第2引数)を使うのか
	//vbは配列の先頭のアドレスを指すことになる
	//strideとoffsetも各頂点バッファごとに指定することになるのでこれらも配列
	D3D.m_deviceContext->IASetDrawCallBuffers(1, 1, Buffer.GetAddressOf(), &stride, &offset);
}
*/




/*
IndexBuffer::IndexBuffer() {

}
void IndexBuffer::Initialize() {
	DataList = new WORD[BLENGTHINDEX];
	D3D11_BUFFER_DESC bDesc = {};
	bDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;	// デバイスにバインドするときの種類(頂点バッファ、インデックスバッファ、定数バッファなど)
	bDesc.ByteWidth = sizeof(WORD) * BLENGTHINDEX;					// 作成するバッファのバイトサイズ
	//起動時に確保して使いまわす場合ここのサイズをあらかじめ決める必要があり、それが描画可能量の限界となる
	bDesc.MiscFlags = 0;							// その他のフラグ
	bDesc.StructureByteStride = 0;					// 構造化バッファの場合、その構造体のサイズ
	bDesc.Usage = D3D11_USAGE_DYNAMIC;				// 作成するバッファの使用法
	bDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	// 上の仕様を渡して頂点バッファを作ってもらう
	// もちろんデバイスさんにお願いする
	// 頂点バッファについては最初の変数以外は関係ないらしい？
	D3D11_SUBRESOURCE_DATA initData = { DataList, sizeof(WORD) * UsedCount, 0 };	// 書き込むデータ
	if (FAILED(D3D.m_device->CreateBuffer(&bDesc, &initData, &Buffer))) {
		return;
	}
	D3D.m_deviceContext->IASetIndexBuffer(Buffer.Get(), DXGI_FORMAT_R16_UINT, 0);
}
void IndexBuffer::UpdateAndSet() {
	D3D11_MAPPED_SUBRESOURCE mappedSubresource = { };
	//ここでGPUから頂点バッファへのアクセスを止め、CPUからのアクセスができるようにしている
	if (FAILED(D3D.m_deviceContext->Map(Buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource))) {
		return;
	}
	//ここでCPUによる処理でバッファの内容を変えている
	memcpy(mappedSubresource.pData, DataList, sizeof(WORD) * UsedCount);//第3変数を変えることで書き換える数を変えられる
	//ここでGPUにアクセスを許可してる
	D3D.m_deviceContext->Unmap(Buffer.Get(), 0);
	D3D.m_deviceContext->IASetIndexBuffer(Buffer.Get(), DXGI_FORMAT_R16_UINT, 0);
}
*/
ConstantBuffer::ConstantBuffer() {
	D3D11_BUFFER_DESC bDesc = {};
	bDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;	// デバイスにバインドするときの種類(頂点バッファ、インデックスバッファ、定数バッファなど)
	bDesc.ByteWidth = sizeof(Data);					// 作成するバッファのバイトサイズ
	//起動時に確保して使いまわす場合ここのサイズをあらかじめ決める必要があり、それが描画可能量の限界となる
	bDesc.MiscFlags = 0;							// その他のフラグ
	bDesc.StructureByteStride = 0;					// 構造化バッファの場合、その構造体のサイズ
	bDesc.Usage = D3D11_USAGE_DYNAMIC;				// 作成するバッファの使用法
	bDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	// 上の仕様を渡して頂点バッファを作ってもらう
	// もちろんデバイスさんにお願いする
	// 頂点バッファについては最初の変数以外は関係ないらしい？
	D3D11_SUBRESOURCE_DATA initData = { &Data, sizeof(Data), 0 };	// 書き込むデータ
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
	//ここでGPUから頂点バッファへのアクセスを止め、CPUからのアクセスができるようにしている
	if (FAILED(D3D.m_deviceContext->Map(Buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource))) {
		throw("");
	}
	//ここでCPUによる処理でバッファの内容を変えている
	memcpy(mappedSubresource.pData, &Data , sizeof(Data));//第3変数を変えることで書き換える数を変えられる
	//ここでGPUにアクセスを許可してる
	D3D.m_deviceContext->Unmap(Buffer.Get(), 0);
	D3D.m_deviceContext->VSSetConstantBuffers(0, 1, Buffer.GetAddressOf());
}



/*
LightBuffer::LightBuffer() {
	DataList = new LightBufferData[BLENGTHLIGHT];
	D3D11_BUFFER_DESC bDesc = {};
	bDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;	// デバイスにバインドするときの種類(頂点バッファ、インデックスバッファ、定数バッファなど)
	bDesc.ByteWidth = sizeof(LightBufferData) * BLENGTHLIGHT;					// 作成するバッファのバイトサイズ
	//起動時に確保して使いまわす場合ここのサイズをあらかじめ決める必要があり、それが描画可能量の限界となる
	bDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;							// その他のフラグ
	bDesc.StructureByteStride = sizeof(LightBufferData);					// 構造化バッファの場合、その構造体のサイズ
	bDesc.Usage = D3D11_USAGE_DYNAMIC;				// 作成するバッファの使用法
	bDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA initData = { DataList, sizeof(LightBufferData) * UsedCount, 0 };	// 書き込むデータ
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
	//ここでGPUから頂点バッファへのアクセスを止め、CPUからのアクセスができるようにしている
	if (FAILED(D3D.m_deviceContext->Map(Buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource))) {
		return;
	}
	//ここでCPUによる処理でバッファの内容を変えている
	memcpy(mappedSubresource.pData, DataList, sizeof(LightBufferData) * UsedCount);//第3変数を変えることで書き換える数を変えられる
	//ここでGPUにアクセスを許可してる
	D3D.m_deviceContext->Unmap(Buffer.Get(), 0);
	D3D.m_deviceContext->VSSetShaderResources(0, 1, SRV.GetAddressOf());
}
*/