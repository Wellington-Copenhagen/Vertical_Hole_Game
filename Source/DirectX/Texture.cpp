#include "framework.h"

#include <locale.h>

#include "Texture.h"

Texture::Texture(std::string filename)
{
	// マルチバイト文字列からワイド文字列へ変換
	setlocale(LC_CTYPE, "jpn");
	wchar_t wFilename[256];
	size_t ret;
	mbstowcs_s(&ret, wFilename, filename.c_str(), 256);

	//unique_ptrもスマートポインタの一種みたいな
	// WIC画像を読み込む
	auto image = std::make_unique<DirectX::ScratchImage>();
	if (FAILED(DirectX::LoadFromWICFile(wFilename, DirectX::WIC_FLAGS_NONE, &m_info, *image)))
	{
		// 失敗
		m_info = {};
		return;
	}

	// ミップマップの生成
	if (m_info.mipLevels == 1)
	{
		auto mipChain = std::make_unique<DirectX::ScratchImage>();
		if (SUCCEEDED(DirectX::GenerateMipMaps(image->GetImages(), image->GetImageCount(), image->GetMetadata(), DirectX::TEX_FILTER_DEFAULT, 0, *mipChain)))
		{
			image = std::move(mipChain);
		}
	}
	// リソースとシェーダーリソースビューを作成
	if (FAILED(DirectX::CreateShaderResourceView(D3D.m_device.Get(), image->GetImages(), image->GetImageCount(), m_info, &m_srv)))
	{
		// 失敗
		m_info = {};
		return;
	}
}
//64x64でRGBA8bitずつだと画像1枚16kBになる
//256x256でRGBA8bitずつだと画像1枚256kBになる→100枚で25MBまぁ全然許容範囲だろう
TextureArray::TextureArray(std::string fileName) {
	/*
	ComPtr<ID3D11Texture2D> texture = nullptr;//おそらくこいつに全部書き込む
	{
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.ArraySize = NumberOfTexture;
		desc.Height = Width;
		desc.Width = Height;
		desc.CPUAccessFlags = 0;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.MipLevels = mipLevel;
		desc.MiscFlags = 0;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		D3D11_SUBRESOURCE_DATA subresourceData;
		subresourceData.SysMemSlicePitch = 0;
		subresourceData.SysMemPitch = Width * BitePerPixel;
		if (FAILED(D3D.m_device->CreateTexture2D(&desc, nullptr, texture.GetAddressOf()))) {
			return;
		}
	}
	*/
	setlocale(LC_CTYPE, "jpn");
	wchar_t wFilename[256];
	size_t ret;
	mbstowcs_s(&ret, wFilename, fileName.c_str(), 256);
	auto image = std::make_unique<DirectX::ScratchImage>();
	DirectX::TexMetadata m_info = {};
	if (FAILED(DirectX::LoadFromWICFile(wFilename, DirectX::WIC_FLAGS_ALL_FRAMES, &m_info, *image)))
	{
		// 失敗
		m_info = {};
		return;
	}
	if (m_info.mipLevels == 1)
	{
		auto mipChain = std::make_unique<DirectX::ScratchImage>();
		if (SUCCEEDED(DirectX::GenerateMipMaps(image->GetImages(), image->GetImageCount(), image->GetMetadata(), DirectX::TEX_FILTER_DEFAULT, 7, *mipChain)))
		{
			image = std::move(mipChain);
			m_info.mipLevels = 7;
		}
	}
	// リソースとシェーダーリソースビューを作成
	if (FAILED(DirectX::CreateShaderResourceView(D3D.m_device.Get(), image->GetImages(), image->GetImageCount(), m_info, &m_srv)))
	{
		// 失敗
		m_info = {};
		return;
	}
	/*
	for (int i = 0; i < TexCount; i++) {
		for(int j = 0;j < )
		D3D.m_deviceContext->UpdateSubresource(texture.Get(),D3D11CalcSubresource(),nullptr,image->GetPixels(),)
		D3D11_MAPPED_SUBRESOURCE mapped;
		if (FAILED(D3D.m_deviceContext->Map(texture.Get(), 0, D3D11_MAP_WRITE, 0, &mapped))) {
			return;
		}
		memcpy(&mapped.pData, image->GetPixels(), image->GetPixelsSize());
		D3D.m_deviceContext->Unmap(texture.Get(), 0);
	}
	{
		D3D11_TEXTURE2D_DESC texDesc;
		texture->GetDesc(&texDesc);
		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Format = texDesc.Format;
		desc.ViewDimension = D3D10_1_SRV_DIMENSION_TEXTURE2DARRAY;
		desc.Texture2DArray.ArraySize = texDesc.ArraySize;
		desc.Texture2DArray.FirstArraySlice = 0;
		desc.Texture2DArray.MipLevels = texDesc.MipLevels;
		desc.Texture2DArray.MostDetailedMip = 0;
		if (FAILED(D3D.m_device->CreateShaderResourceView(texture.Get(), &desc, m_srv.GetAddressOf()))) {
			return;
		}
		
	}
	*/
}
void TextureArray::SetToGraphicPipeLine() {
	D3D.m_deviceContext->PSSetShaderResources(0, 1, m_srv.GetAddressOf());
}
SameFormatTextureArray BlockTextureArray;

SameFormatTextureArray ConstantTextureArray;
SameFormatTextureArray VariableTextureArray;