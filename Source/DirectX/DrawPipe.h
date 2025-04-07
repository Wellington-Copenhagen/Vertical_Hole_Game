#pragma once
#include "DirectX.h"
#include "framework.h"
#include "Interfaces.h"
// インプットレイアウトと頂点シェーダー
// テンプレートを使ってるのは不一致を検出してエラーを吐かせるため
template<typename DCType,typename IType>
struct VertexShaderAndInputLayout {
	ComPtr<ID3D11VertexShader> mVertexShader;
	ComPtr<ID3D11InputLayout> mInputLayout;
	static int SizeOfDXGIFormat(DXGI_FORMAT format) {
		if (format == DXGI_FORMAT_R32_FLOAT) {
			return sizeof(float);
		}
		else if (format == DXGI_FORMAT_R32G32_FLOAT) {
			return sizeof(float) * 2;
		}
		else if (format == DXGI_FORMAT_R32G32B32_FLOAT) {
			return sizeof(float) * 3;
		}
		else if (format == DXGI_FORMAT_R32G32B32A32_FLOAT) {
			return sizeof(float) * 4;
		}
		else {
			throw("不明なDXGI_FORMAT");
		}
	}
	void Compile(
		LPCWSTR vertexShaderPath,
		std::vector<D3D11_INPUT_ELEMENT_DESC> layoutDesc) {
		int drawcallSize = 0;
		int instanceSize = 0;
		for (auto elementDesc : layoutDesc) {
			if (elementDesc.InputSlotClass == D3D11_INPUT_PER_INSTANCE_DATA) {
				instanceSize = instanceSize + SizeOfDXGIFormat(elementDesc.Format);
			}
			if (elementDesc.InputSlotClass == D3D11_INPUT_PER_VERTEX_DATA) {
				drawcallSize = drawcallSize + SizeOfDXGIFormat(elementDesc.Format);
			}
		}
		if (drawcallSize != sizeof(DCType) || instanceSize != sizeof(IType)) {
			throw("Input Layoutとバッファに使う型のサイズが不一致");
		}



		// 頂点シェーダーを読み込み＆コンパイル
		ComPtr<ID3DBlob> compiledVS;
		ComPtr<ID3DBlob> pError;
		if (FAILED(D3DCompileFromFile(vertexShaderPath, nullptr, nullptr, "main", "vs_5_0", 0, 0, &compiledVS, &pError)))
		{
			OutputDebugStringA((char*)pError->GetBufferPointer());
			throw("頂点シェーダのコンパイルに失敗");
		}
		// 頂点シェーダー作成
		if (FAILED(D3D.m_device->CreateVertexShader(compiledVS->GetBufferPointer(), compiledVS->GetBufferSize(), nullptr, mVertexShader.GetAddressOf())))
		{
			throw("頂点シェーダの生成に失敗");
		}
		// １頂点の詳細な情報
		//頂点データの形式を示している
		// 頂点インプットレイアウト作成
		if (FAILED(D3D.m_device->CreateInputLayout(&layoutDesc[0], layoutDesc.size(), compiledVS->GetBufferPointer(), compiledVS->GetBufferSize(), mInputLayout.GetAddressOf())))
		{
			throw("頂点インプットレイアウトの生成に失敗");
		}
	}
	void SetToDrawPipe() {
		D3D.m_deviceContext->VSSetShader(mVertexShader.Get(), 0, 0);
		D3D.m_deviceContext->IASetInputLayout(mInputLayout.Get());
	}
};
//描画パイプラインにおけるシェーダーリソース以外の設定
class GraphicProcessSetter {
public:
	static int Width;
	static int Height;
	
	GraphicProcessSetter() {

	}

	static ComPtr<ID3D11DepthStencilState> m_depthStencilStateEnable;
	static ComPtr<ID3D11DepthStencilState> m_depthStencilStateDisable;

	static ComPtr<ID3D11BlendState> m_blendStateEnable;
	static ComPtr<ID3D11BlendState> m_blendStateDisable;

	static ComPtr<ID3D11RasterizerState> m_rasterizerState;

	static ComPtr<ID3D11SamplerState> m_samplerState;

	static VertexShaderAndInputLayout<Interface::BlockDrawCallType, Interface::BlockInstanceType> mBlockVShaderLayout;
	static ComPtr<ID3D11PixelShader> mBlockPixelShader;


	static VertexShaderAndInputLayout<Interface::BallDrawCallType, Interface::BallInstanceType> mBallVShaderLayout;
	static ComPtr<ID3D11PixelShader> mBallPixelShader;

	static VertexShaderAndInputLayout<Interface::BulletDrawCallType, Interface::BulletInstanceType> mBulletVShaderLayout;
	static ComPtr<ID3D11PixelShader> mBulletPixelShader;

	static VertexShaderAndInputLayout<Interface::EffectDrawCallType, Interface::EffectInstanceType> mEffectVShaderLayout;
	static ComPtr<ID3D11PixelShader> mEffectPixelShader;

	static VertexShaderAndInputLayout<Interface::CharDrawCallType, Interface::CharInstanceType> mCharVShaderLayout;
	static ComPtr<ID3D11PixelShader> mCharPixelShader;

	static VertexShaderAndInputLayout<Interface::GeneralDrawCallType, Interface::GeneralInstanceType> mGeneralVShaderLayout;
	static VertexShaderAndInputLayout<Interface::LineDrawCallType, Interface::LineInstanceType> mLineVShaderLayout;
	static VertexShaderAndInputLayout<Interface::FreeShapeDrawCallType, Interface::FreeShapeInstanceType> mFreeShapeVShaderLayout;

	static ComPtr<ID3D11PixelShader> mGeneralPixelShader;
	static void SetAsBlock();
	static void SetAsBall();
	static void SetAsBullet();
	static void SetAsEffect();
	static void SetAsCharacter();
	static void SetAsGeneral();
	static void SetAsLine();
	static void SetAsFreeShape();
	static void CompilePixelShader(LPCWSTR pixelShaderPath, ID3D11PixelShader** ppPixelShader);
	GraphicProcessSetter(int width, int height);
};
#define DP GraphicProcessSetter::GetInstance()