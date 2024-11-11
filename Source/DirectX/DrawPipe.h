#pragma once
#include "DirectX.h"
#include "framework.h"


class DrawPipe {
public:
	int Width;
	int Height;
	
	int IndexPerInstance;

	int DrawingIndex;

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

	Texture TextureToDraw;
	TextureArray tArrayToDraw;
	void AddRectInstance(DirectX::XMMATRIX world);
	void AddRectInstance(DirectX::XMMATRIX* world);
	void SetRectVertexes(RectVertexType vDataList[4]);
	void SetTexture(Texture tex);
	void SetTextureArray(TextureArray texArray);
	void AddStrip(DirectX::XMVECTOR* wpos, DirectX::XMFLOAT2* uvs, int count);
	void AddStrip(StripVertexType* vDatas, int* count);
	void AddLight(DirectX::XMVECTOR pos, DirectX::XMVECTOR color);
	void ResetPositionVertexBufferData();
	void ResetLightConstantBufferData();
	void DrawAsRectOpaque();
	void DrawAsStripTransparent();
	void DrawAsRectTransparent();
	void Init();
	static DrawPipe& GetInstance() {
		if (pInstance == nullptr) {
			pInstance = new DrawPipe(D3D.Width, D3D.Height);
		}
		return *pInstance;
	}
private:
	DrawPipe(int width, int height);
	static DrawPipe* pInstance;
};
#define DP DrawPipe::GetInstance()