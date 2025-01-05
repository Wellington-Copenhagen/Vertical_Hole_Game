
#include "DirectX.h"
#include "framework.h"
#include "DrawPipe.h"
#include "Source/Game/Camera.h"

DrawPipe::DrawPipe(int width, int height) {
	Width = width;
	Height = height;
	Buffers = AllBuffers();


	//深度ステンシルステート
	{
		D3D11_DEPTH_STENCIL_DESC dsDesc = {};
		dsDesc.DepthEnable = true;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;


		dsDesc.StencilEnable = false;
		dsDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
		dsDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
		dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
		dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_GREATER_EQUAL;

		dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
		dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_GREATER_EQUAL;

		if (FAILED(D3D.m_device->CreateDepthStencilState(&dsDesc, m_depthStencilStateEnable.GetAddressOf()))) {
			throw("深度ステンシルステートの生成に失敗");
		}
	}
	{
		D3D11_DEPTH_STENCIL_DESC dsDesc = {};
		dsDesc.DepthEnable = false;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;


		dsDesc.StencilEnable = false;
		dsDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
		dsDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
		dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
		dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_GREATER_EQUAL;

		dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
		dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_GREATER_EQUAL;

		if (FAILED(D3D.m_device->CreateDepthStencilState(&dsDesc, m_depthStencilStateDisable.GetAddressOf()))) {
			throw("深度ステンシルステートの生成に失敗");
		}
	}


	// ブレンドステート
	{
		D3D11_RENDER_TARGET_BLEND_DESC rtvBlendDesc = {};
		rtvBlendDesc.BlendEnable = true;
		rtvBlendDesc.BlendOp = D3D11_BLEND_OP_ADD;
		rtvBlendDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
		rtvBlendDesc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		rtvBlendDesc.DestBlendAlpha = D3D11_BLEND_ZERO;
		rtvBlendDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		rtvBlendDesc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
		rtvBlendDesc.SrcBlendAlpha = D3D11_BLEND_ONE;
		D3D11_BLEND_DESC blendDesc = {};
		blendDesc.AlphaToCoverageEnable = false;
		blendDesc.IndependentBlendEnable = false;
		blendDesc.RenderTarget[0] = rtvBlendDesc;
		if (FAILED(D3D.m_device->CreateBlendState(&blendDesc, m_blendStateEnable.GetAddressOf())))
		{
			throw("ブレンドステートの生成に失敗");
		}
	}

	{
		D3D11_RENDER_TARGET_BLEND_DESC rtvBlendDesc = {};
		rtvBlendDesc.BlendEnable = false;
		rtvBlendDesc.BlendOp = D3D11_BLEND_OP_ADD;
		rtvBlendDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
		rtvBlendDesc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		rtvBlendDesc.DestBlendAlpha = D3D11_BLEND_ZERO;
		rtvBlendDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		rtvBlendDesc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
		rtvBlendDesc.SrcBlendAlpha = D3D11_BLEND_ONE;
		D3D11_BLEND_DESC blendDesc = {};
		blendDesc.AlphaToCoverageEnable = false;
		blendDesc.IndependentBlendEnable = false;
		blendDesc.RenderTarget[0] = rtvBlendDesc;
		if (FAILED(D3D.m_device->CreateBlendState(&blendDesc, m_blendStateDisable.GetAddressOf())))
		{
			throw("ブレンドステートの生成に失敗");
		}
	}




	{
		D3D11_RASTERIZER_DESC rasterizerDesc;
		rasterizerDesc.CullMode = D3D11_CULL_BACK;
		rasterizerDesc.FillMode = D3D11_FILL_SOLID;
		rasterizerDesc.DepthClipEnable = true;
		rasterizerDesc.FrontCounterClockwise = true;
		rasterizerDesc.ScissorEnable = false;
		rasterizerDesc.MultisampleEnable = false;
		if (FAILED(D3D.m_device->CreateRasterizerState(&rasterizerDesc, m_rasterizerState.GetAddressOf()))) {
			throw("ラスタライザ―ステートの生成に失敗");
		}
	}




	//=====================================================
	// シェーダーの作成
	//=====================================================
	{
		// 頂点シェーダーを読み込み＆コンパイル
		ComPtr<ID3DBlob> compiledVS;
		if (FAILED(D3DCompileFromFile(L"Shader/Shader.hlsl", nullptr, nullptr, "VSRect", "vs_5_0", 0, 0, &compiledVS, nullptr)))
		{
			throw("頂点シェーダのコンパイルに失敗");
		}

		// 頂点シェーダー作成
		if (FAILED(D3D.m_device->CreateVertexShader(compiledVS->GetBufferPointer(), compiledVS->GetBufferSize(), nullptr, &m_vertexShaderForRect)))
		{
			throw("頂点シェーダの生成に失敗");
		}

		// １頂点の詳細な情報
		//頂点データの形式を示している
		//セマンティック、
		std::vector<D3D11_INPUT_ELEMENT_DESC> layout = {//頂点ごとであるのか、インスタンスごとであるのかは上手く使えば面白そう
			{ "TEXCOORD",		0, DXGI_FORMAT_R32G32_FLOAT,		0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "POSITIONT",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,		0,  8, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{"MATRIX",0,DXGI_FORMAT_R32G32B32A32_FLOAT,1,0,D3D11_INPUT_PER_INSTANCE_DATA,1},
			{"MATRIX",1,DXGI_FORMAT_R32G32B32A32_FLOAT,1,16,D3D11_INPUT_PER_INSTANCE_DATA,1},
			{"MATRIX",2,DXGI_FORMAT_R32G32B32A32_FLOAT,1,32,D3D11_INPUT_PER_INSTANCE_DATA,1},
			{"MATRIX",3,DXGI_FORMAT_R32G32B32A32_FLOAT,1,48,D3D11_INPUT_PER_INSTANCE_DATA,1},
		};

		// 頂点インプットレイアウト作成
		if (FAILED(D3D.m_device->CreateInputLayout(&layout[0], layout.size(), compiledVS->GetBufferPointer(), compiledVS->GetBufferSize(), &m_inputLayoutForRect)))
		{
			throw("頂点インプットレイアウトの生成に失敗");
		}
	}
	/*
	{
		// 頂点シェーダーを読み込み＆コンパイル
		ComPtr<ID3DBlob> compiledVS;
		if (FAILED(D3DCompileFromFile(L"Shader/Shader.hlsl", nullptr, nullptr, "VSStrip", "vs_5_0", 0, 0, &compiledVS, nullptr)))
		{
			throw("頂点シェーダのコンパイルに失敗");
		}

		// 頂点シェーダー作成
		if (FAILED(D3D.m_device->CreateVertexShader(compiledVS->GetBufferPointer(), compiledVS->GetBufferSize(), nullptr, &m_vertexShaderForStrip)))
		{
			throw("頂点シェーダの生成に失敗");
		}


		// １頂点の詳細な情報
		//頂点データの形式を示している
		//セマンティック、
		std::vector<D3D11_INPUT_ELEMENT_DESC> layout = {//頂点ごとであるのか、インスタンスごとであるのかは上手く使えば面白そう
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,		0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD",		0, DXGI_FORMAT_R32G32_FLOAT,		0,  12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		// 頂点インプットレイアウト作成
		if (FAILED(D3D.m_device->CreateInputLayout(&layout[0], layout.size(), compiledVS->GetBufferPointer(), compiledVS->GetBufferSize(), &m_inputLayoutForStrip)))
		{
			throw("頂点インプットレイアウトの生成に失敗");
		}
	}
	*/
	{
		// ピクセルシェーダーを読み込み＆コンパイル
		ComPtr<ID3DBlob> compiledPS;
		if (FAILED(D3DCompileFromFile(L"Shader/Shader.hlsl", nullptr, nullptr, "PS", "ps_5_0", 0, 0, &compiledPS, nullptr)))
		{
			throw("ピクセルシェーダのコンパイルに失敗");
		}
		// ピクセルシェーダー作成
		if (FAILED(D3D.m_device->CreatePixelShader(compiledPS->GetBufferPointer(), compiledPS->GetBufferSize(), nullptr, &m_pixelShader)))
		{
			throw("頂点シェーダのコンパイルに失敗");
		}

		//インスタンス毎バッファも用意してそっちを使いたい



	}
	//サンプラー
	{
		D3D11_SAMPLER_DESC samplerDesc;
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.MipLODBias = 0;
		samplerDesc.MaxAnisotropy = 1;

		if (FAILED(D3D.m_device->CreateSamplerState(&samplerDesc, &m_samplerState))) {
			throw("サンプラの生成に失敗");
		}
	}
}
void DrawPipe::SetTextureArray(TextureArray texArray) {
	tArrayToDraw = texArray;
}
void DrawPipe::ResetEveryDrawCall() {
	Buffers.RectDCBuffer.UsedCount = 0;
	Buffers.RectIBuffer.UsedCount = 0;
}
void DrawPipe::ResetEveryTick() {
}
void DrawPipe::DrawAsRectTransparent() {
	Buffers.RectDCBuffer.UpdateAndSet();
	Buffers.RectIBuffer.UpdateAndSet();
	D3D.m_deviceContext->OMSetDepthStencilState(m_depthStencilStateEnable.Get(), 0);
	D3D.m_deviceContext->PSSetShaderResources(0, 1, tArrayToDraw.m_srv.GetAddressOf());
	float blendFactor[4]{ D3D11_BLEND_ZERO,D3D11_BLEND_ZERO, D3D11_BLEND_ZERO, D3D11_BLEND_ZERO };
	D3D.m_deviceContext->OMSetBlendState(m_blendStateEnable.Get(), blendFactor, 0xffffffff);
	{
		D3D11_VIEWPORT vp = { 0.0f, 0.0f, (float)Width, (float)Height, 0.0f, 1.0f };
		vp.TopLeftX = 0.0f;
		vp.TopLeftY = 0.0f;
		vp.Width = (float)Width;
		vp.Height = (float)Height;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		D3D.m_deviceContext->RSSetViewports(1, &vp);
	}
	D3D.m_deviceContext->RSSetState(m_rasterizerState.Get());
	D3D.m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	D3D.m_deviceContext->PSSetSamplers(0, 1, m_samplerState.GetAddressOf());
	D3D.m_deviceContext->VSSetShader(m_vertexShaderForRect.Get(), 0, 0);
	D3D.m_deviceContext->PSSetShader(m_pixelShader.Get(), 0, 0);
	D3D.m_deviceContext->IASetInputLayout(m_inputLayoutForRect.Get());
	D3D.m_deviceContext->OMSetRenderTargets(1, D3D.m_backBufferView.GetAddressOf(), D3D.m_depthStencilView.Get());
	D3D.m_deviceContext->DrawInstanced(4, Buffers.RectIBuffer.UsedCount, 0, 0);
}