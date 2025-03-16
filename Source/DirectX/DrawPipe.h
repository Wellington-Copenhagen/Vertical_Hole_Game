#pragma once
#include "DirectX.h"
#include "framework.h"
#include "Interfaces.h"
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

	static ComPtr<ID3D11VertexShader> mBlockVertexShader;
	static ComPtr<ID3D11PixelShader> mBlockPixelShader;
	static ComPtr<ID3D11InputLayout> mBlockInputLayout;

	static ComPtr<ID3D11VertexShader> mBallVertexShader;
	static ComPtr<ID3D11PixelShader> mBallPixelShader;
	static ComPtr<ID3D11InputLayout> mBallInputLayout;

	static ComPtr<ID3D11VertexShader> mBulletVertexShader;
	static ComPtr<ID3D11PixelShader> mBulletPixelShader;
	static ComPtr<ID3D11InputLayout> mBulletInputLayout;

	static ComPtr<ID3D11VertexShader> mCharVertexShader;
	static ComPtr<ID3D11PixelShader> mCharPixelShader;
	static ComPtr<ID3D11InputLayout> mCharInputLayout;

	static void SetAsBlock();
	static void SetAsBall();
	static void SetAsBullet();
	static void SetAsCharacter();
	GraphicProcessSetter(int width, int height);
};
#define DP GraphicProcessSetter::GetInstance()