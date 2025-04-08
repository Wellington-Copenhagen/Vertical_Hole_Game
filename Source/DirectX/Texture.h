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
class SameFormatTextureArray {
public:
	ComPtr<ID3D11ShaderResourceView> m_srv;
	ComPtr<ID3D11Texture2D> m_texture;
	std::unordered_map<std::string, int> FilePathIndexTable;
	std::unordered_map<int, int> ColorIndexTable;
	int FloorShadowIndex;
	int WallShadowIndex;

	int MipLevel;
	int TexCount;
	int Width;
	SameFormatTextureArray() {

	}
	SameFormatTextureArray(int arraySize,bool useMipmap,int width) {
		Width = width;
		FloorShadowIndex = -1;
		WallShadowIndex = -1;
		if (useMipmap) {
			MipLevel = log2(width) + 1;
		}
		else {
			MipLevel = 1;
		}
		TexCount = 0;
		FilePathIndexTable = std::unordered_map<std::string, int>();
		ColorIndexTable = std::unordered_map<int, int>();
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
		auto itr = FilePathIndexTable.find(filePath);
		if (itr!=FilePathIndexTable.end()) {
			return itr->second;
		}
		setlocale(LC_CTYPE, "jpn");
		wchar_t wFilename[256];
		size_t ret;
		mbstowcs_s(&ret, wFilename, filePath.c_str(), 256);
		auto image = std::make_unique<DirectX::ScratchImage>();
		DirectX::TexMetadata m_info = {};
		if (FAILED(DirectX::LoadFromWICFile(wFilename, DirectX::WIC_FLAGS_ALL_FRAMES, &m_info, *image)))
		{
			m_info = {};
			throw("");
		}
		if (m_info.width != Width || m_info.height != Width) {
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
		DirectX::CreateTexture(D3D.m_device.Get(), image->GetImages(), image->GetImageCount(), m_info, texture.GetAddressOf());
		// 読み込んでミップマップにした画像をsrvに紐づけられているテクスチャファイルにコピーする
		int headIndex = TexCount;
		for (int i = 0; i < m_info.arraySize; i++) {
			for (int j = 0; j < MipLevel; j++) {
				D3D.m_deviceContext->CopySubresourceRegion(m_texture.Get(), TexCount * MipLevel + j, 0, 0, 0, texture.Get(), i * MipLevel + j, nullptr);
			}
			TexCount++;
		}
		FilePathIndexTable.emplace(filePath, headIndex);
		return headIndex;
	}
	int Append(void* pData,int sliceLength,int sliceWidth) {
		DirectX::Image img;
		img.pixels = (uint8_t*)pData;
		img.format = DXGI_FORMAT_R8G8B8A8_UNORM;
		img.width = Width;
		img.height = Width;
		img.rowPitch = sliceWidth;
		img.slicePitch = sliceLength;
		DirectX::ScratchImage mippedImage;
		if (FAILED(DirectX::GenerateMipMaps(img, DirectX::TEX_FILTER_DEFAULT, MipLevel, mippedImage))) {
			throw("");
		}

		ComPtr<ID3D11Resource> tempTexture = nullptr;
		DirectX::CreateTexture(D3D.m_device.Get(), mippedImage.GetImages(), mippedImage.GetImageCount(), mippedImage.GetMetadata(), &tempTexture);
		for (int j = 0; j < MipLevel; j++) {
			D3D.m_deviceContext->CopySubresourceRegion(m_texture.Get(), TexCount * MipLevel + j, 0, 0, 0, tempTexture.Get(), j, nullptr);
		}
		TexCount++;
		return TexCount - 1;
	}
	int Append(Json::Value textureJson) {
		if (Interface::SameString(textureJson.get("kind", "").asString(), "file")) {
			return AppendFromFileName(textureJson.get("filePath", "").asString());
		}
		else if (Interface::SameString(textureJson.get("kind", "").asString(), "oneColor")) {
			float red = textureJson.get("color", "")[0].asFloat();
			float green = textureJson.get("color", "")[1].asFloat();
			float blue = textureJson.get("color", "")[2].asFloat();
			float alpha = textureJson.get("color", "")[3].asFloat();
			return AppendOneColorTexture(red, green, blue, alpha);
		}
		else {
			throw("");
		}
	}
	void SetToGraphicPipeLine() {
		D3D.m_deviceContext->PSSetShaderResources(0, 1, m_srv.GetAddressOf());
	}
	// 入力は0~1
	int AppendOneColorTexture(float red,float green,float blue,float alpha) {
		byte iRed = min(255,max(0,(byte)(red * 255)));
		byte iGreen = min(255, max(0, (byte)(green * 255)));
		byte iBlue = min(255, max(0, (byte)(blue * 255)));
		byte iAlpha = min(255, max(0, (byte)(alpha * 255)));
		int color = iRed + iGreen * 256 + iBlue * 256 * 256 + iAlpha * 256 * 256 * 256;
		auto itr = ColorIndexTable.find(color);
		if (itr != ColorIndexTable.end()) {
			return itr->second;
		}
		std::vector<byte> pInit = std::vector<byte>(Width * Width * 4, 0);
		for (int i = 0; i < Width * Width; i++) {
			pInit[i * 4] = iRed;
			pInit[i * 4 + 1] = iGreen;
			pInit[i * 4 + 2] = iBlue;
			pInit[i * 4 + 3] = iAlpha;
		}
		int headIndex = Append((void*)&pInit[0], Width * Width * 4, Width * 4);
		ColorIndexTable.emplace(color, headIndex);
		return headIndex;
	}
	static void SaveShadowTexture(int width, std::string filePath, int topShadowWidth, int bottomShadowWidth, int leftShadowWidth, int rightShadowWidth) {
		int imgCount = pow(2, 8);
		DirectX::Image* imgs = new DirectX::Image[imgCount];
		for (int i = 0; i < imgCount; i++) {
			byte* pInit = new byte[width * width * 4];
			int offset = 0;
			for (int y = 0; y < width; y++) {
				for (int x = 0; x < width; x++) {
					byte iBase = 255;
					byte iGreen = 0;
					byte iShadow = 0;
					byte iAlpha = 255;
					byte fromTop = 0;
					byte fromBottom = 0;
					byte fromLeft = 0;
					byte fromRight = 0;
					if (topShadowWidth > 0) {
						fromTop = max(0, min(255, (topShadowWidth - y) * 256 / topShadowWidth - 1));
					}
					if (bottomShadowWidth > 0) {
						fromBottom = max(0, min(255, (y - (width - bottomShadowWidth)) * 256 / bottomShadowWidth - 1));
					}
					if (leftShadowWidth > 0) {
						fromLeft = max(0, min(255, (leftShadowWidth - x) * 256 / leftShadowWidth - 1));
					}
					if (rightShadowWidth > 0) {
						fromRight = max(0, min(255, (x - (width - rightShadowWidth)) * 256 / rightShadowWidth - 1));
					}
					// 上→右上→右の順
					if (i % 2 == 1) {
						// 上
						iShadow = max(iShadow, fromTop);
					}
					if ((i / 2) % 2 == 1) {
						// 右上
						iShadow = max(iShadow, min(fromRight,fromTop));
					}
					if ((i / 4) % 2 == 1) {
						// 右
						iShadow = max(iShadow, fromRight);
					}
					if ((i / 8) % 2 == 1) {
						// 右下
						iShadow = max(iShadow, min(fromRight, fromBottom));
					}
					if ((i / 16) % 2 == 1) {
						// 下
						iShadow = max(iShadow, fromBottom);
					}
					if ((i / 32) % 2 == 1) {
						// 左下
						iShadow = max(iShadow, min(fromLeft, fromBottom));
					}
					if ((i / 64) % 2 == 1) {
						// 左
						iShadow = max(iShadow, fromLeft);
					}
					if ((i / 128) % 2 == 1) {
						// 左上
						iShadow = max(iShadow, min(fromLeft, fromTop));
					}
					pInit[offset] = iBase;
					offset++;
					pInit[offset] = iGreen;
					offset++;
					pInit[offset] = iShadow;
					offset++;
					pInit[offset] = iAlpha;
					offset++;
					//OutputDebugStringA(std::to_string(iShadow).c_str());
					//OutputDebugStringA(",");
				}
			}
			DirectX::Image img;
			img.format = DXGI_FORMAT_R8G8B8A8_UNORM;
			img.width = width;
			img.height = width;
			img.rowPitch = width * 4;
			img.slicePitch = width * width * 4;
			img.pixels = pInit;
			imgs[i] = img;
			//OutputDebugStringA("\n");
		}
		DirectX::ScratchImage scratchImg;
		scratchImg.InitializeArrayFromImages(imgs,pow(2,8));
		wchar_t* wPath = new wchar_t[256];
		if (FAILED(mbstowcs_s(nullptr, wPath, 256, filePath.c_str(), 256))) {
			throw("");
		}
		
		if (FAILED(DirectX::SaveToWICFile(imgs, imgCount, DirectX::WIC_FLAGS_ALL_FRAMES, DirectX::GetWICCodec(DirectX::WIC_CODEC_TIFF), wPath))) {
			throw("");
		}
		delete[] imgs;
		delete[] wPath;
	}

	static void SaveWallMask(int width, std::string filePath) {
		int imgCount = pow(2, 8);
		DirectX::Image* imgs = new DirectX::Image[imgCount];
		for (int i = 0; i < imgCount; i++) {
			byte* pInit = new byte[width * width * 4];
			int offset = 0;
			for (int y = 0; y < width; y++) {
				for (int x = 0; x < width; x++) {
					byte iBase = 255;
					byte iGreen = 0;
					byte iShadow = 0;
					byte iAlpha = 255;
					// その場所にブロックがないことを示す
					bool up = i % 2 == 1;
					bool upRight = (i / 2) % 2 == 1;
					bool right = (i / 4) % 2 == 1;
					bool downRight = (i / 8) % 2 == 1;
					bool down = (i / 16) % 2 == 1;
					bool downLeft = (i / 32) % 2 == 1;
					bool left = (i / 64) % 2 == 1;
					bool upLeft = (i / 128) % 2 == 1;
					if (up && upRight && right && x >= width / 2 && y < width / 2) {
						iBase = max(0, min(255, (-4 * (pow(abs(x / (float)width - 0.5) * 2, 1.5) + pow(abs(y / (float)width - 0.5) * 2, 1.5)) + 5) * 255));
					}
					if (down && downRight && right && x >= width / 2 && y >= width / 2) {
						iBase = max(0, min(255, (-4 * (pow(abs(x / (float)width - 0.5) * 2, 1.5) + pow(abs(y / (float)width - 0.5) * 2, 1.5)) + 5) * 255));
					}
					if (down && downLeft && left && x < width / 2 && y >= width / 2) {
						iBase = max(0, min(255, (-4 * (pow(abs(x / (float)width - 0.5) * 2, 1.5) + pow(abs(y / (float)width - 0.5) * 2, 1.5)) + 5) * 255));
					}
					if (up && upLeft && left && x < width / 2 && y < width / 2) {
						iBase = max(0, min(255, (-4 * (pow(abs(x / (float)width - 0.5) * 2, 1.5) + pow(abs(y / (float)width - 0.5) * 2, 1.5)) + 5) * 255));
					}
					pInit[offset] = iBase;
					offset++;
					pInit[offset] = iGreen;
					offset++;
					pInit[offset] = iShadow;
					offset++;
					pInit[offset] = iAlpha;
					offset++;
					//OutputDebugStringA(std::to_string(iShadow).c_str());
					//OutputDebugStringA(",");
				}
			}
			DirectX::Image img;
			img.format = DXGI_FORMAT_R8G8B8A8_UNORM;
			img.width = width;
			img.height = width;
			img.rowPitch = width * 4;
			img.slicePitch = width * width * 4;
			img.pixels = pInit;
			imgs[i] = img;
			//OutputDebugStringA("\n");
		}
		DirectX::ScratchImage scratchImg;
		scratchImg.InitializeArrayFromImages(imgs, pow(2, 8));
		wchar_t* wPath = new wchar_t[256];
		if (FAILED(mbstowcs_s(nullptr, wPath, 256, filePath.c_str(), 256))) {
			throw("");
		}

		if (FAILED(DirectX::SaveToWICFile(imgs, imgCount, DirectX::WIC_FLAGS_ALL_FRAMES, DirectX::GetWICCodec(DirectX::WIC_CODEC_TIFF), wPath))) {
			throw("");
		}
		delete[] imgs;
		delete[] wPath;
	}
	static void SaveLensMask(int width, std::string filePath) {
		int imgCount = 1;
		DirectX::Image* imgs = new DirectX::Image[imgCount];
		for (int i = 0; i < imgCount; i++) {
			byte* pInit = new byte[width * width * 4];
			int offset = 0;
			for (int y = 0; y < width; y++) {
				for (int x = 0; x < width; x++) {
					byte iBase = 0;
					byte iGreen = 0;
					byte iShadow = 0;
					byte iAlpha = 0;
					iAlpha = max(0, min(255, (-4 * (pow(abs(x / (float)width - 0.5) * 2, 4) + pow(abs(y / (float)width - 0.5) * 2, 4)) + 5) * 255));
					iAlpha = 255 - iAlpha;
					pInit[offset] = iBase;
					offset++;
					pInit[offset] = iGreen;
					offset++;
					pInit[offset] = iShadow;
					offset++;
					pInit[offset] = iAlpha;
					offset++;
					//OutputDebugStringA(std::to_string(iShadow).c_str());
					//OutputDebugStringA(",");
				}
			}
			DirectX::Image img;
			img.format = DXGI_FORMAT_R8G8B8A8_UNORM;
			img.width = width;
			img.height = width;
			img.rowPitch = width * 4;
			img.slicePitch = width * width * 4;
			img.pixels = pInit;
			imgs[i] = img;
			//OutputDebugStringA("\n");
		}
		DirectX::ScratchImage scratchImg;
		scratchImg.InitializeArrayFromImages(imgs, pow(2, 8));
		wchar_t* wPath = new wchar_t[256];
		if (FAILED(mbstowcs_s(nullptr, wPath, 256, filePath.c_str(), 256))) {
			throw("");
		}

		if (FAILED(DirectX::SaveToWICFile(imgs, imgCount, DirectX::WIC_FLAGS_ALL_FRAMES, DirectX::GetWICCodec(DirectX::WIC_CODEC_TIFF), wPath))) {
			throw("");
		}
		delete[] imgs;
		delete[] wPath;
	}
};