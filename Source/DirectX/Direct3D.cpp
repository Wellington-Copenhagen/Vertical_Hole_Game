#include "Direct3D.h"
#include "framework.h"

#include "Source/DirectX/DirectX.h"

#include <windows.h>
bool Direct3D::Initialize(HWND hWnd, int width, int height)
{
	Height = height;
	Width = width;
	//=====================================================
	// ファクトリー作成(ビデオ グラフィックの設定の列挙や指定に使用されるオブジェクト)
	//=====================================================
	ComPtr<IDXGIFactory> factory;

	if (FAILED(CreateDXGIFactory1(IID_PPV_ARGS(&factory))))
	{
		return false;
	}

	//=====================================================
	//デバイス生成(主にリソース作成時に使用するオブジェクト)
	//=====================================================
	UINT creationFlags = 0;
#ifdef _DEBUG
	// DEBUGビルド時はDirect3Dのデバッグを有効にする
	// (すごく重いが細かいエラーがわかる)
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	//機能とグラボの対応のセットみたいなもん
	//例えば750tiだと11_0までしか対応してないので11_1だと失敗する
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,	// Direct3D 11.1  ShaderModel 5
		D3D_FEATURE_LEVEL_11_0,	// Direct3D 11    ShaderModel 5
		D3D_FEATURE_LEVEL_10_1,	// Direct3D 10.1  ShaderModel 4
		D3D_FEATURE_LEVEL_10_0,	// Direct3D 10.0  ShaderModel 4
		D3D_FEATURE_LEVEL_9_3,	// Direct3D 9.3   ShaderModel 3
		D3D_FEATURE_LEVEL_9_2,	// Direct3D 9.2   ShaderModel 3
		D3D_FEATURE_LEVEL_9_1,	// Direct3D 9.1   ShaderModel 3
	};

	// デバイスとでデバイスコンテキストを作成
	D3D_FEATURE_LEVEL futureLevel;
	if (FAILED(D3D11CreateDevice(
		nullptr,//画面？を選んでる？
		D3D_DRIVER_TYPE_HARDWARE,//
		nullptr,//ラスタライズするライブラリ、上がSOFTWAREの場合ヌルポインタにできない
		creationFlags,//設定みたいな
		featureLevels,//対応するバージョンみたいな感じだろうか
		_countof(featureLevels),
		D3D11_SDK_VERSION,//SDKのバージョン(リソースで定義されてるマクロ)
		&m_device,//返すデバイスのCOMPtrのポインタ
		&futureLevel,//futureLevelsのうち上から試して成功したバージョンを返す
		&m_deviceContext)))//デバイスコンテキストのCOMPtrのポインタ
	{
		return false;
	}

	//=====================================================
	// スワップチェイン作成(フロントバッファに表示可能なバックバッファを持つもの)
	//=====================================================
	DXGI_SWAP_CHAIN_DESC scDesc = {};		// スワップチェーンの設定データ
	scDesc.BufferDesc.Width = width;						// 画面の幅
	scDesc.BufferDesc.Height = height;						// 画面の高さ
	scDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// バッファの形式、色の形式
	scDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;//スキャンラインは物体の前後関係を判定する方法
	scDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;//画面サイズに合わせて変形させるかどうか
	scDesc.BufferDesc.RefreshRate.Numerator = 0;//分数になってて単位はHzらしい
	scDesc.BufferDesc.RefreshRate.Denominator = 1;
	scDesc.SampleDesc.Count = 1;							// MSAAは使用しない、ピクセル内での色の比率の計算法
	scDesc.SampleDesc.Quality = 0;							// MSAAは使用しない
	scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	// バッファの使用方法
	scDesc.BufferCount = 2;									// バッファの数
	scDesc.OutputWindow = hWnd;
	scDesc.Windowed = TRUE;									// ウィンドウモード
	scDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;//バックバッファが白紙に戻るということだろうか…
	scDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;//andでいろいろ設定できる。今は全画面かどうかを切り替え可というだけ

	// スワップチェインの作成
	if (FAILED(factory->CreateSwapChain(m_device.Get(), &scDesc, &m_swapChain)))
	{
		return false;
	}
	// スワップチェインからバックバッファリソース取得
	ComPtr<ID3D11Texture2D> pBackBuffer;
	if (FAILED(m_swapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer))))
	{
		return false;
	}

	// バックバッファリソース用のRTVを作成
	//RTVは副次的に使うバッファみたいなやつ？
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = scDesc.BufferDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	if (FAILED(m_device->CreateRenderTargetView(pBackBuffer.Get(), &rtvDesc, &m_backBufferView)))
	{
		return false;
	}


	ComPtr<ID3D11Texture2D> m_depthStencil;
	D3D11_TEXTURE2D_DESC t2Desc = {};
	ZeroMemory(&t2Desc, sizeof(t2Desc));
	pBackBuffer->GetDesc(&t2Desc);
	t2Desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	t2Desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	/*
	t2Desc.Width = scDesc.BufferDesc.Width;
	t2Desc.Height = scDesc.BufferDesc.Height;
	t2Desc.CPUAccessFlags = 0;
	t2Desc.MiscFlags = 0;

	t2Desc.Usage = D3D11_USAGE_DEFAULT;
	t2Desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	t2Desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	t2Desc.SampleDesc.Count = 1;
	t2Desc.SampleDesc.Quality = 0;
	t2Desc.MipLevels = 1;
	t2Desc.ArraySize = 1;
	*/
	if (FAILED(m_device->CreateTexture2D(&t2Desc, nullptr, m_depthStencil.GetAddressOf()))) {
		return false;
	}
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	ZeroMemory(&dsvDesc, sizeof(dsvDesc));
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Format = t2Desc.Format;
	dsvDesc.Texture2D.MipSlice = 0;
	if (FAILED(m_device->CreateDepthStencilView(m_depthStencil.Get(),&dsvDesc,m_depthStencilView.GetAddressOf() ))) {
		return false;
	}
	m_deviceContext->OMSetRenderTargets(1, m_backBufferView.GetAddressOf(), m_depthStencilView.Get());
	//=====================================================
	// デバイスコンテキストに描画に関する設定を行っておく
	//=====================================================

	// バックバッファをRTとしてセット
	//バインドするレンダーの数だけどさっき1つしか作らなかったので1なのだろう
	//
	//第3変数はつなげる深度ステンシルビューのポインタ、何が前にいるかみたいなもんだろうか
	return true;
}