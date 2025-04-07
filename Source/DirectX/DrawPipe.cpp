
#include "DirectX.h"
#include "framework.h"
#include "DrawPipe.h"

int GraphicProcessSetter::Width;
int GraphicProcessSetter::Height;

ComPtr<ID3D11DepthStencilState> GraphicProcessSetter:: m_depthStencilStateEnable;
ComPtr<ID3D11DepthStencilState> GraphicProcessSetter:: m_depthStencilStateDisable;

ComPtr<ID3D11BlendState> GraphicProcessSetter:: m_blendStateEnable;
ComPtr<ID3D11BlendState> GraphicProcessSetter:: m_blendStateDisable;

ComPtr<ID3D11RasterizerState> GraphicProcessSetter:: m_rasterizerState;

ComPtr<ID3D11SamplerState> GraphicProcessSetter:: m_samplerState;

VertexShaderAndInputLayout<Interface::BlockDrawCallType, Interface::BlockInstanceType> GraphicProcessSetter::mBlockVShaderLayout;
ComPtr<ID3D11PixelShader> GraphicProcessSetter::mBlockPixelShader;


VertexShaderAndInputLayout<Interface::BallDrawCallType, Interface::BallInstanceType> GraphicProcessSetter::mBallVShaderLayout;
ComPtr<ID3D11PixelShader> GraphicProcessSetter::mBallPixelShader;

VertexShaderAndInputLayout<Interface::BulletDrawCallType, Interface::BulletInstanceType> GraphicProcessSetter::mBulletVShaderLayout;
ComPtr<ID3D11PixelShader> GraphicProcessSetter::mBulletPixelShader;

VertexShaderAndInputLayout<Interface::EffectDrawCallType, Interface::EffectInstanceType> GraphicProcessSetter::mEffectVShaderLayout;
ComPtr<ID3D11PixelShader> GraphicProcessSetter::mEffectPixelShader;

VertexShaderAndInputLayout<Interface::CharDrawCallType, Interface::CharInstanceType> GraphicProcessSetter::mCharVShaderLayout;
ComPtr<ID3D11PixelShader> GraphicProcessSetter::mCharPixelShader;

VertexShaderAndInputLayout<Interface::LineDrawCallType, Interface::LineInstanceType> GraphicProcessSetter::mLineVShaderLayout;
VertexShaderAndInputLayout<Interface::FreeShapeDrawCallType, Interface::FreeShapeInstanceType> GraphicProcessSetter::mFreeShapeVShaderLayout;
VertexShaderAndInputLayout<Interface::GeneralDrawCallType, Interface::GeneralInstanceType> GraphicProcessSetter::mGeneralVShaderLayout;
ComPtr<ID3D11PixelShader> GraphicProcessSetter::mGeneralPixelShader;






GraphicProcessSetter::GraphicProcessSetter(int width, int height) {
	Width = width;
	Height = height;


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
		rasterizerDesc.CullMode = D3D11_CULL_NONE;
		rasterizerDesc.FillMode = D3D11_FILL_SOLID;
		rasterizerDesc.DepthClipEnable = true;
		rasterizerDesc.FrontCounterClockwise = true;
		rasterizerDesc.ScissorEnable = false;
		rasterizerDesc.MultisampleEnable = false;
		if (FAILED(D3D.m_device->CreateRasterizerState(&rasterizerDesc, m_rasterizerState.GetAddressOf()))) {
			throw("ラスタライザ―ステートの生成に失敗");
		}
	}



	_ASSERT(_CrtCheckMemory());
	//ブロック用の
	{
		std::vector<D3D11_INPUT_ELEMENT_DESC> layout = {
			{ "TEXCOORD",		0, DXGI_FORMAT_R32G32_FLOAT,		0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "POSITION",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,		0,  8, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{"POSITION",1,DXGI_FORMAT_R32G32_FLOAT,1,0,D3D11_INPUT_PER_INSTANCE_DATA,1},
			{"TEXCOORD",1,DXGI_FORMAT_R32G32B32A32_FLOAT,1,8,D3D11_INPUT_PER_INSTANCE_DATA,1},
			{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,1,24,D3D11_INPUT_PER_INSTANCE_DATA,1},
		};
		mBlockVShaderLayout.Compile(L"Shader/BlockVertexShader.hlsl", layout);
		CompilePixelShader(L"Shader/BlockPixelShader.hlsl", mBlockPixelShader.GetAddressOf());
	}
	//ボール用の
	{
		std::vector<D3D11_INPUT_ELEMENT_DESC> layout = {
			{ "TEXCOORD",		0, DXGI_FORMAT_R32G32_FLOAT,		0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "POSITION",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,		0,  8, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{"MWorld",0,DXGI_FORMAT_R32G32B32A32_FLOAT,1,0,D3D11_INPUT_PER_INSTANCE_DATA,1},
			{"MWorld",1,DXGI_FORMAT_R32G32B32A32_FLOAT,1,16,D3D11_INPUT_PER_INSTANCE_DATA,1},
			{"MWorld",2,DXGI_FORMAT_R32G32B32A32_FLOAT,1,32,D3D11_INPUT_PER_INSTANCE_DATA,1},
			{"MWorld",3,DXGI_FORMAT_R32G32B32A32_FLOAT,1,48,D3D11_INPUT_PER_INSTANCE_DATA,1},
			{"TEXCOORD",1,DXGI_FORMAT_R32_FLOAT,1,64,D3D11_INPUT_PER_INSTANCE_DATA,1},
			{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,1,68,D3D11_INPUT_PER_INSTANCE_DATA,1},
			{"COLOR",1,DXGI_FORMAT_R32G32B32A32_FLOAT,1,84,D3D11_INPUT_PER_INSTANCE_DATA,1},
			{"COLOR",2,DXGI_FORMAT_R32G32B32A32_FLOAT,1,100,D3D11_INPUT_PER_INSTANCE_DATA,1},
		};
		mBallVShaderLayout.Compile(L"Shader/BallVertexShader.hlsl", layout);
		CompilePixelShader(L"Shader/BallPixelShader.hlsl", mBallPixelShader.GetAddressOf());
	}
	//エフェクト用の
	{
		std::vector<D3D11_INPUT_ELEMENT_DESC> layout = {
			{ "TEXCOORD",		0, DXGI_FORMAT_R32G32_FLOAT,		0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "POSITION",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,		0,  8, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{"MWorld",0,DXGI_FORMAT_R32G32B32A32_FLOAT,1,0,D3D11_INPUT_PER_INSTANCE_DATA,1},
			{"MWorld",1,DXGI_FORMAT_R32G32B32A32_FLOAT,1,16,D3D11_INPUT_PER_INSTANCE_DATA,1},
			{"MWorld",2,DXGI_FORMAT_R32G32B32A32_FLOAT,1,32,D3D11_INPUT_PER_INSTANCE_DATA,1},
			{"MWorld",3,DXGI_FORMAT_R32G32B32A32_FLOAT,1,48,D3D11_INPUT_PER_INSTANCE_DATA,1},
			{"TEXCOORD",1,DXGI_FORMAT_R32_FLOAT,1,64,D3D11_INPUT_PER_INSTANCE_DATA,1},
			{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,1,68,D3D11_INPUT_PER_INSTANCE_DATA,1},
			{"COLOR",1,DXGI_FORMAT_R32G32B32A32_FLOAT,1,84,D3D11_INPUT_PER_INSTANCE_DATA,1},
		};
		mEffectVShaderLayout.Compile(L"Shader/EffectVertexShader.hlsl", layout);
		CompilePixelShader(L"Shader/EffectPixelShader.hlsl", mEffectPixelShader.GetAddressOf());
	}
	//弾丸用の
	{
		std::vector<D3D11_INPUT_ELEMENT_DESC> layout = {
			{ "TEXCOORD",		0, DXGI_FORMAT_R32G32_FLOAT,		0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "POSITION",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,		0,  8, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{"MWorld",0,DXGI_FORMAT_R32G32B32A32_FLOAT,1,0,D3D11_INPUT_PER_INSTANCE_DATA,1},
			{"MWorld",1,DXGI_FORMAT_R32G32B32A32_FLOAT,1,16,D3D11_INPUT_PER_INSTANCE_DATA,1},
			{"MWorld",2,DXGI_FORMAT_R32G32B32A32_FLOAT,1,32,D3D11_INPUT_PER_INSTANCE_DATA,1},
			{"MWorld",3,DXGI_FORMAT_R32G32B32A32_FLOAT,1,48,D3D11_INPUT_PER_INSTANCE_DATA,1},
			{"TEXCOORD",1,DXGI_FORMAT_R32_FLOAT,1,64,D3D11_INPUT_PER_INSTANCE_DATA,1},
		};
		mBulletVShaderLayout.Compile(L"Shader/BulletVertexShader.hlsl", layout);
		CompilePixelShader(L"Shader/BulletPixelShader.hlsl", mBulletPixelShader.GetAddressOf());
	}
	//文字用の
	{
		std::vector<D3D11_INPUT_ELEMENT_DESC> layout = {
			{ "TEXCOORD",		0, DXGI_FORMAT_R32G32_FLOAT,		0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "POSITION",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,		0,  8, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{"MWorld",0,DXGI_FORMAT_R32G32B32A32_FLOAT,1,0,D3D11_INPUT_PER_INSTANCE_DATA,1},
			{"MWorld",1,DXGI_FORMAT_R32G32B32A32_FLOAT,1,16,D3D11_INPUT_PER_INSTANCE_DATA,1},
			{"MWorld",2,DXGI_FORMAT_R32G32B32A32_FLOAT,1,32,D3D11_INPUT_PER_INSTANCE_DATA,1},
			{"MWorld",3,DXGI_FORMAT_R32G32B32A32_FLOAT,1,48,D3D11_INPUT_PER_INSTANCE_DATA,1},
			{"TEXCOORD",1,DXGI_FORMAT_R32_FLOAT,1,64,D3D11_INPUT_PER_INSTANCE_DATA,1},
			{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,1,68,D3D11_INPUT_PER_INSTANCE_DATA,1},
			{"BlackBox",0,DXGI_FORMAT_R32G32_FLOAT,1,84,D3D11_INPUT_PER_INSTANCE_DATA,1},
		};
		mCharVShaderLayout.Compile(L"Shader/CharVertexShader.hlsl", layout);
		CompilePixelShader(L"Shader/CharPixelShader.hlsl", mCharPixelShader.GetAddressOf());
	}
	// 線
	{
		std::vector<D3D11_INPUT_ELEMENT_DESC> layout = {
			{ "TEXCOORD",		0, DXGI_FORMAT_R32G32_FLOAT,		0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "POSITION",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,		0,  8, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{"POSITION",1,DXGI_FORMAT_R32G32B32A32_FLOAT,1,0,D3D11_INPUT_PER_INSTANCE_DATA,1},
			{"POSITION",2,DXGI_FORMAT_R32G32B32A32_FLOAT,1,16,D3D11_INPUT_PER_INSTANCE_DATA,1},
			{"POSITION",3,DXGI_FORMAT_R32_FLOAT,1,32,D3D11_INPUT_PER_INSTANCE_DATA,1},
			{"TEXCOORD",1,DXGI_FORMAT_R32_FLOAT,1,36,D3D11_INPUT_PER_INSTANCE_DATA,1},
		};
		mLineVShaderLayout.Compile(L"Shader/LineVertexShader.hlsl", layout);
	}
	// 各頂点の位置を指定できる図形
	{
		std::vector<D3D11_INPUT_ELEMENT_DESC> layout = {
			{ "TEXCOORD",		0, DXGI_FORMAT_R32G32_FLOAT,		0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{"POSITION",0,DXGI_FORMAT_R32G32B32A32_FLOAT,1,0,D3D11_INPUT_PER_INSTANCE_DATA,1},
			{"POSITION",1,DXGI_FORMAT_R32G32B32A32_FLOAT,1,16,D3D11_INPUT_PER_INSTANCE_DATA,1},
			{"POSITION",2,DXGI_FORMAT_R32G32B32A32_FLOAT,1,32,D3D11_INPUT_PER_INSTANCE_DATA,1},
			{"POSITION",3,DXGI_FORMAT_R32G32B32A32_FLOAT,1,48,D3D11_INPUT_PER_INSTANCE_DATA,1},
			{"TEXCOORD",1,DXGI_FORMAT_R32_FLOAT,1,64,D3D11_INPUT_PER_INSTANCE_DATA,1},
		};
		mFreeShapeVShaderLayout.Compile(L"Shader/FreeShapeVertexShader.hlsl", layout);
	}
	// 汎用
	{
		std::vector<D3D11_INPUT_ELEMENT_DESC> layout = {
			{ "TEXCOORD",		0, DXGI_FORMAT_R32G32_FLOAT,		0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "POSITION",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,		0,  8, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{"MWorld",0,DXGI_FORMAT_R32G32B32A32_FLOAT,1,0,D3D11_INPUT_PER_INSTANCE_DATA,1},
			{"MWorld",1,DXGI_FORMAT_R32G32B32A32_FLOAT,1,16,D3D11_INPUT_PER_INSTANCE_DATA,1},
			{"MWorld",2,DXGI_FORMAT_R32G32B32A32_FLOAT,1,32,D3D11_INPUT_PER_INSTANCE_DATA,1},
			{"MWorld",3,DXGI_FORMAT_R32G32B32A32_FLOAT,1,48,D3D11_INPUT_PER_INSTANCE_DATA,1},
			{"TEXCOORD",1,DXGI_FORMAT_R32_FLOAT,1,64,D3D11_INPUT_PER_INSTANCE_DATA,1},
		};
		mGeneralVShaderLayout.Compile(L"Shader/GeneralVertexShader.hlsl", layout);
		CompilePixelShader(L"Shader/GeneralPixelShader.hlsl", mGeneralPixelShader.GetAddressOf());
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
		samplerDesc.MaxLOD = 0;
		samplerDesc.MinLOD = 0;

		if (FAILED(D3D.m_device->CreateSamplerState(&samplerDesc, &m_samplerState))) {
			throw("サンプラの生成に失敗");
		}
	}
}
void GraphicProcessSetter::SetAsBlock() {
	D3D.m_deviceContext->OMSetDepthStencilState(m_depthStencilStateDisable.Get(), 0);
	float blendFactor[4]{ D3D11_BLEND_ZERO,D3D11_BLEND_ZERO, D3D11_BLEND_ZERO, D3D11_BLEND_ZERO };
	D3D.m_deviceContext->OMSetBlendState(m_blendStateEnable.Get(), blendFactor, 0xffffffff);
	{
		D3D11_VIEWPORT vp;
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
	D3D.m_deviceContext->PSSetShader(mBlockPixelShader.Get(), 0, 0);
	mBlockVShaderLayout.SetToDrawPipe();
	D3D.m_deviceContext->OMSetRenderTargets(1, D3D.m_backBufferView.GetAddressOf(), D3D.m_depthStencilView.Get());

}
void GraphicProcessSetter::SetAsBall() {
	D3D.m_deviceContext->OMSetDepthStencilState(m_depthStencilStateEnable.Get(), 0);
	float blendFactor[4]{ D3D11_BLEND_ZERO,D3D11_BLEND_ZERO, D3D11_BLEND_ZERO, D3D11_BLEND_ZERO };
	D3D.m_deviceContext->OMSetBlendState(m_blendStateEnable.Get(), blendFactor, 0xffffffff);
	{
		D3D11_VIEWPORT vp;
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
	D3D.m_deviceContext->PSSetShader(mBallPixelShader.Get(), 0, 0);
	mBallVShaderLayout.SetToDrawPipe();
	D3D.m_deviceContext->OMSetRenderTargets(1, D3D.m_backBufferView.GetAddressOf(), D3D.m_depthStencilView.Get());

}
void GraphicProcessSetter::SetAsEffect() {
	D3D.m_deviceContext->OMSetDepthStencilState(m_depthStencilStateDisable.Get(), 0);
	float blendFactor[4]{ D3D11_BLEND_ZERO,D3D11_BLEND_ZERO, D3D11_BLEND_ZERO, D3D11_BLEND_ZERO };
	D3D.m_deviceContext->OMSetBlendState(m_blendStateEnable.Get(), blendFactor, 0xffffffff);
	{
		D3D11_VIEWPORT vp;
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
	D3D.m_deviceContext->PSSetShader(mEffectPixelShader.Get(), 0, 0);
	mEffectVShaderLayout.SetToDrawPipe();
	D3D.m_deviceContext->OMSetRenderTargets(1, D3D.m_backBufferView.GetAddressOf(), D3D.m_depthStencilView.Get());

}
void GraphicProcessSetter::SetAsBullet() {
	D3D.m_deviceContext->OMSetDepthStencilState(m_depthStencilStateDisable.Get(), 0);
	float blendFactor[4]{ D3D11_BLEND_ZERO,D3D11_BLEND_ZERO, D3D11_BLEND_ZERO, D3D11_BLEND_ZERO };
	D3D.m_deviceContext->OMSetBlendState(m_blendStateEnable.Get(), blendFactor, 0xffffffff);
	{
		D3D11_VIEWPORT vp;
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
	D3D.m_deviceContext->PSSetShader(mBulletPixelShader.Get(), 0, 0);
	mBulletVShaderLayout.SetToDrawPipe();
	D3D.m_deviceContext->OMSetRenderTargets(1, D3D.m_backBufferView.GetAddressOf(), D3D.m_depthStencilView.Get());

}
void GraphicProcessSetter::SetAsCharacter() {
	D3D.m_deviceContext->OMSetDepthStencilState(m_depthStencilStateDisable.Get(), 0);
	float blendFactor[4]{ D3D11_BLEND_ZERO,D3D11_BLEND_ZERO, D3D11_BLEND_ZERO, D3D11_BLEND_ZERO };
	D3D.m_deviceContext->OMSetBlendState(m_blendStateEnable.Get(), blendFactor, 0xffffffff);
	{
		D3D11_VIEWPORT vp;
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
	D3D.m_deviceContext->PSSetShader(mCharPixelShader.Get(), 0, 0);
	mCharVShaderLayout.SetToDrawPipe();
	D3D.m_deviceContext->OMSetRenderTargets(1, D3D.m_backBufferView.GetAddressOf(), D3D.m_depthStencilView.Get());

}
void GraphicProcessSetter::SetAsGeneral() {
	D3D.m_deviceContext->OMSetDepthStencilState(m_depthStencilStateEnable.Get(), 0);
	float blendFactor[4]{ D3D11_BLEND_ZERO,D3D11_BLEND_ZERO, D3D11_BLEND_ZERO, D3D11_BLEND_ZERO };
	D3D.m_deviceContext->OMSetBlendState(m_blendStateEnable.Get(), blendFactor, 0xffffffff);
	{
		D3D11_VIEWPORT vp;
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
	D3D.m_deviceContext->PSSetShader(mGeneralPixelShader.Get(), 0, 0);
	mGeneralVShaderLayout.SetToDrawPipe();
	D3D.m_deviceContext->OMSetRenderTargets(1, D3D.m_backBufferView.GetAddressOf(), D3D.m_depthStencilView.Get());

}
void GraphicProcessSetter::SetAsLine() {
	D3D.m_deviceContext->OMSetDepthStencilState(m_depthStencilStateDisable.Get(), 0);
	float blendFactor[4]{ D3D11_BLEND_ZERO,D3D11_BLEND_ZERO, D3D11_BLEND_ZERO, D3D11_BLEND_ZERO };
	D3D.m_deviceContext->OMSetBlendState(m_blendStateEnable.Get(), blendFactor, 0xffffffff);
	{
		D3D11_VIEWPORT vp;
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
	D3D.m_deviceContext->PSSetShader(mGeneralPixelShader.Get(), 0, 0);
	mLineVShaderLayout.SetToDrawPipe();
	D3D.m_deviceContext->OMSetRenderTargets(1, D3D.m_backBufferView.GetAddressOf(), D3D.m_depthStencilView.Get());

}
void GraphicProcessSetter::SetAsFreeShape() {
	D3D.m_deviceContext->OMSetDepthStencilState(m_depthStencilStateDisable.Get(), 0);
	float blendFactor[4]{ D3D11_BLEND_ZERO,D3D11_BLEND_ZERO, D3D11_BLEND_ZERO, D3D11_BLEND_ZERO };
	D3D.m_deviceContext->OMSetBlendState(m_blendStateEnable.Get(), blendFactor, 0xffffffff);
	{
		D3D11_VIEWPORT vp;
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
	D3D.m_deviceContext->PSSetShader(mGeneralPixelShader.Get(), 0, 0);
	mFreeShapeVShaderLayout.SetToDrawPipe();
	D3D.m_deviceContext->OMSetRenderTargets(1, D3D.m_backBufferView.GetAddressOf(), D3D.m_depthStencilView.Get());

}
void GraphicProcessSetter::CompilePixelShader(LPCWSTR pixelShaderPath, ID3D11PixelShader** ppPixelShader) {
	ComPtr<ID3DBlob> pError;
	// ピクセルシェーダーを読み込み＆コンパイル
	ComPtr<ID3DBlob> compiledPS;
	if (pixelShaderPath != nullptr) {
		if (FAILED(D3DCompileFromFile(pixelShaderPath, nullptr, nullptr, "main", "ps_5_0", 0, 0, &compiledPS, &pError)))
		{
			OutputDebugStringA((char*)pError->GetBufferPointer());
			throw("ピクセルシェーダのコンパイルに失敗");
		}
		// ピクセルシェーダー作成
		if (FAILED(D3D.m_device->CreatePixelShader(compiledPS->GetBufferPointer(), compiledPS->GetBufferSize(), nullptr, ppPixelShader)))
		{
			throw("頂点シェーダのコンパイルに失敗");
		}
	}
}