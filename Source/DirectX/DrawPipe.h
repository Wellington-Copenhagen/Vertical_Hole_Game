#pragma once
#include "DirectX.h"
#include "framework.h"
#include "Interfaces.h"
#include "Texture.h"
#include "Buffers.h"

class DrawPipe {
public:
	int Width;
	int Height;
	

	AllBuffers Buffers;
	ComPtr<ID3D11DepthStencilState> m_depthStencilStateEnable;
	ComPtr<ID3D11DepthStencilState> m_depthStencilStateDisable;
	ComPtr<ID3D11BlendState> m_blendStateEnable;
	ComPtr<ID3D11BlendState> m_blendStateDisable;
	ComPtr<ID3D11RasterizerState> m_rasterizerState;
	ComPtr<ID3D11SamplerState> m_samplerState;

	ComPtr<ID3D11VertexShader> m_vertexShaderForStrip;
	ComPtr<ID3D11VertexShader> m_vertexShaderForRect;
	ComPtr<ID3D11PixelShader> m_pixelShader;
	ComPtr<ID3D11InputLayout> m_inputLayoutForStrip;
	ComPtr<ID3D11InputLayout> m_inputLayoutForRect;
	TextureArray tArrayToDraw;

	void SetTextureArray(TextureArray texArray);
	void ResetEveryDrawCall();
	void ResetEveryTick();
	void DrawAsRectTransparent();
	DrawPipe(int width, int height);
};
#define DP DrawPipe::GetInstance()