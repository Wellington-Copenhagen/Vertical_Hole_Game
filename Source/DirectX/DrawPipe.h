#pragma once
#include "DirectX.h"
#include "framework.h"
#include "Interfaces.h"
//描画パイプラインにおけるシェーダーリソース以外の設定
class GraphicProcessSetter {
public:
	int Width;
	int Height;
	
	GraphicProcessSetter() {

	}

	ComPtr<ID3D11DepthStencilState> m_depthStencilStateEnable;
	ComPtr<ID3D11DepthStencilState> m_depthStencilStateDisable;

	ComPtr<ID3D11BlendState> m_blendStateEnable;
	ComPtr<ID3D11BlendState> m_blendStateDisable;

	ComPtr<ID3D11RasterizerState> m_rasterizerState;

	ComPtr<ID3D11SamplerState> m_samplerState;

	ComPtr<ID3D11VertexShader> mBlockVertexShader;
	ComPtr<ID3D11PixelShader> mBlockPixelShader;
	ComPtr<ID3D11InputLayout> mBlockInputLayout;

	ComPtr<ID3D11VertexShader> mCharacterVertexShader;
	ComPtr<ID3D11PixelShader> mCharacterPixelShader;
	ComPtr<ID3D11InputLayout> mCharacterInputLayout;

	ComPtr<ID3D11VertexShader> mBulletVertexShader;
	ComPtr<ID3D11PixelShader> mBulletPixelShader;
	ComPtr<ID3D11InputLayout> mBulletInputLayout;

	void SetAsBlock();
	void SetAsCharacter();
	void SetAsBullet();
	GraphicProcessSetter(int width, int height);
};
#define DP GraphicProcessSetter::GetInstance()