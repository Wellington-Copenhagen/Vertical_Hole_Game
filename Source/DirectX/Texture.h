#pragma once

#include "DirectX.h"
#include "Direct3D.h"
#include "Interfaces.h"

//高くにあるもの、不透明なもの優先がよい
//壁床キャラクターエフェクトみたいな

//描画パイプラインにおけるテクスチャの設定
class Texture
{
public:
	//GPUのメモリ上にテクスチャが読み込まれてそれにハンドルみたいな
	// シェーダーリソースビュー(画像データ読み取りハンドル)
	ComPtr<ID3D11ShaderResourceView> m_srv = nullptr;
	// 画像情報
	DirectX::TexMetadata m_info = {};

	// 画像ファイルを読み込む
	Texture(std::string filename);
};
class TextureArray {
public:
	TextureArray() {

	}
	TextureArray(std::string fileName);
	int Width;
	int Height;
	int BitePerPixel;
	int NumberOfTexture;
	ComPtr<ID3D11ShaderResourceView> m_srv = nullptr;
	void SetToGraphicPipeLine();
};
template<int width>
class SameFormatTextureArray {
public:
	ComPtr<ID3D11ShaderResourceView> m_srv;
	ComPtr<ID3D11Texture2D> m_texture;
	std::vector<std::string> FilePaths;
	int MipLevel;
	int TexCount;
	SameFormatTextureArray() {

	}
	SameFormatTextureArray(int arraySize,bool useMipmap) {
		if (useMipmap) {
			MipLevel = log2(width) + 1;
		}
		else {
			MipLevel = 1;
		}
		TexCount = 0;
		FilePaths = std::vector<std::string>();
		D3D11_TEXTURE2D_DESC Tex2Ddesc;
		Tex2Ddesc.Width = width;
		Tex2Ddesc.Height = width;
		Tex2Ddesc.MipLevels = MipLevel;
		Tex2Ddesc.ArraySize = arraySize;
		Tex2Ddesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		Tex2Ddesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		Tex2Ddesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		Tex2Ddesc.Usage = D3D11_USAGE_DEFAULT;
		Tex2Ddesc.MiscFlags = 0;
		Tex2Ddesc.SampleDesc.Count = 1;
		Tex2Ddesc.SampleDesc.Quality = 0;

		if (FAILED(D3D.m_device->CreateTexture2D(&Tex2Ddesc, nullptr, m_texture.GetAddressOf()))) {
			throw("");
		}
		D3D11_SHADER_RESOURCE_VIEW_DESC Tex2DArraydesc;
		Tex2DArraydesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		Tex2DArraydesc.Texture2DArray.ArraySize = arraySize;
		Tex2DArraydesc.Texture2DArray.MipLevels = -1;
		Tex2DArraydesc.Texture2DArray.MostDetailedMip = 0;
		Tex2DArraydesc.Texture2DArray.FirstArraySlice = 0;
		Tex2DArraydesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		if (FAILED(D3D.m_device->CreateShaderResourceView(m_texture.Get(), &Tex2DArraydesc, m_srv.GetAddressOf()))) {
			throw("");
		}
	}
	int AppendFromFileName(std::string filePath) {
		for (int i = 0; i < FilePaths.size(); i++) {
			if (Interface::SameString(filePath, FilePaths[i])) {
				return i;
			}
		}
		setlocale(LC_CTYPE, "jpn");
		wchar_t wFilename[256];
		size_t ret;
		mbstowcs_s(&ret, wFilename, filePath.c_str(), 256);
		auto image = std::make_unique<DirectX::ScratchImage>();
		DirectX::TexMetadata m_info = {};
		if (FAILED(DirectX::LoadFromWICFile(wFilename, DirectX::WIC_FLAGS_ALL_FRAMES, &m_info, *image)))
		{
			// 失敗
			m_info = {};
			throw("");
		}
		if (m_info.mipLevels == 1)
		{
			auto mipChain = std::make_unique<DirectX::ScratchImage>();
			if (SUCCEEDED(DirectX::GenerateMipMaps(image->GetImages(), image->GetImageCount(), image->GetMetadata(), DirectX::TEX_FILTER_DEFAULT, MipLevel, *mipChain)))
			{
				image = std::move(mipChain);
				m_info.mipLevels = MipLevel;
			}
		}
		ComPtr<ID3D11Resource> texture = nullptr;
		// 読み込んでミップマップにした画像をsrvに紐づけられているテクスチャファイルにコピーする
		DirectX::CreateTexture(D3D.m_device.Get(), image->GetImages(), image->GetImageCount(), m_info, texture.GetAddressOf());
		// 読み込んでミップマップにした画像をsrvに紐づけられているテクスチャファイルにコピーする
		for (int i = 0; i < MipLevel; i++) {
			D3D.m_deviceContext->CopySubresourceRegion(m_texture.Get(), TexCount * MipLevel + i, 0, 0, 0, texture.Get(), i, nullptr);
		}
		TexCount++;
		FilePaths.push_back(filePath);
		return TexCount - 1;
	}
	int Append(void* pData,int sliceLength,int sliceWidth) {
		D3D11_TEXTURE2D_DESC Tex2Ddesc;
		Tex2Ddesc.Width = width;
		Tex2Ddesc.Height = width;
		Tex2Ddesc.MipLevels = MipLevel;
		Tex2Ddesc.ArraySize = 1;
		Tex2Ddesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		Tex2Ddesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		Tex2Ddesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		Tex2Ddesc.Usage = D3D11_USAGE_DEFAULT;
		Tex2Ddesc.MiscFlags = 0;
		Tex2Ddesc.SampleDesc.Count = 1;
		Tex2Ddesc.SampleDesc.Quality = 0;
		ComPtr<ID3D11Texture2D> pTempTexture;
		D3D11_SUBRESOURCE_DATA init;
		init.pSysMem = pData;
		init.SysMemPitch = sliceWidth;
		init.SysMemSlicePitch = sliceLength;
		if (FAILED(D3D.m_device->CreateTexture2D(&Tex2Ddesc, &init, pTempTexture.GetAddressOf()))) {
			throw("");
		}
		for (int i = 0; i < MipLevel; i++) {
			D3D.m_deviceContext->CopySubresourceRegion(m_texture.Get(), TexCount * MipLevel + i, 0, 0, 0, pTempTexture.Get(), 0, nullptr);
		}
		TexCount++;
		return TexCount - 1;
	}
	void SetToGraphicPipeLine() {
		D3D.m_deviceContext->PSSetShaderResources(0, 1, m_srv.GetAddressOf());
	}
};