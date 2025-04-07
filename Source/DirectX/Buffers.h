#pragma once
#include "Interfaces.h"
#include "Source/DirectX/DirectX.h"

//描画パイプラインにおけるバッファの設定
//おそらく同じ頂点に別のUVを当てることはできない
//頂点バッファには各頂点のワールド座標だけを持たせる
//インデックスバッファは呼び出し順
//定数バッファにはワールド→投影の変換座標や光源の位置を持たせる
//構造化バッファにはUV座標を持たせる(同じ頂点でも別のUV座標を持ちうるので)
//プリミティブトポロジーは単純に三角形を何度も描画するやつで

//おそらくだけどインスタンス描画は頂点の数が同じものしか同時に描画できない
//手くらい三角形を使えば一つのストリップでいくつもの離れ離れの図形を描画できる！
//これはつまり最初に最初の頂点を1回書き込み終わった時点で最後に最後の頂点をもう1回書き込めばよい

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
		bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;	// デバイスにバインドするときの種類(頂点バッファ、インデックスバッファ、定数バッファなど)
		bDesc.ByteWidth = sizeof(DataType) * maxCount;					// 作成するバッファのバイトサイズ
		//起動時に確保して使いまわす場合ここのサイズをあらかじめ決める必要があり、それが描画可能量の限界となる
		bDesc.MiscFlags = 0;							// その他のフラグ
		bDesc.StructureByteStride = 0;					// 構造化バッファの場合、その構造体のサイズ
		bDesc.Usage = D3D11_USAGE_DYNAMIC;				// 作成するバッファの使用法
		bDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		// 上の仕様を渡して頂点バッファを作ってもらう
		// もちろんデバイスさんにお願いする
		// 頂点バッファについては最初の変数以外は関係ないらしい？
		D3D11_SUBRESOURCE_DATA initData = { pData, 0, 0 };	// 書き込むデータ
		// 頂点バッファの作成
		if (FAILED(D3D.m_device->CreateBuffer(&bDesc, &initData, &Buffer))) {
			throw("");
		}

		// 頂点バッファを描画で使えるようにセットする
		UINT stride = sizeof(DataType);
		UINT offset = 0;
		//頂点バッファのうち何番から(第1引数)何個を(第2引数)を使うのか
		//vbは配列の先頭のアドレスを指すことになる
		//strideとoffsetも各頂点バッファごとに指定することになるのでこれらも配列
		D3D.m_deviceContext->IASetVertexBuffers(Slot, 1, Buffer.GetAddressOf(), &stride, &offset);
	}
	void UpdateAndSet(DataType* pData, int offset, int count) {
		D3D11_MAPPED_SUBRESOURCE mappedSubresource;
		//ここでGPUから頂点バッファへのアクセスを止め、CPUからのアクセスができるようにしている
		if (FAILED(D3D.m_deviceContext->Map(Buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource))) {
			throw("");
		}
		//ここでCPUによる処理でバッファの内容を変えている
		memcpy(mappedSubresource.pData, pData, sizeof(DataType) * count);//第3変数を変えることで書き換える数を変えられる
		//ここでGPUにアクセスを許可してる
		D3D.m_deviceContext->Unmap(Buffer.Get(), 0);
		// 頂点バッファを描画で使えるようにセットする
		UINT stride = sizeof(DataType);
		UINT uOffset = (UINT)offset;
		//頂点バッファのうち何番から(第1引数)何個を(第2引数)を使うのか
		//vbは配列の先頭のアドレスを指すことになる
		//strideとoffsetも各頂点バッファごとに指定することになるのでこれらも配列
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