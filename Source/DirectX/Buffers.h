#pragma once
#include "framework.h"
#include "Source/DirectX/DirectX.h"

#define BLENGTHINSTANCE 1000
#define BLENGTHVERTEX 1000000
#define BLENGTHINDEX 2000000
#define BLENGTHLIGHT 100
//おそらく同じ頂点に別のUVを当てることはできない
//頂点バッファには各頂点のワールド座標だけを持たせる
//インデックスバッファは呼び出し順
//定数バッファにはワールド→投影の変換座標や光源の位置を持たせる
//構造化バッファにはUV座標を持たせる(同じ頂点でも別のUV座標を持ちうるので)
//プリミティブトポロジーは単純に三角形を何度も描画するやつで

//おそらくだけどインスタンス描画は頂点の数が同じものしか同時に描画できない
//手くらい三角形を使えば一つのストリップでいくつもの離れ離れの図形を描画できる！
//これはつまり最初に最初の頂点を1回書き込み終わった時点で最後に最後の頂点をもう1回書き込めばよい
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