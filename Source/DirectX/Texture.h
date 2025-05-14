#pragma once

#include "DirectX.h"
#include "Direct3D.h"
#include "Interfaces.h"
#include "PathBindJsonValue.h"

//高くにあるもの、不透明なもの優先がよい
//壁床キャラクターエフェクトみたいな
//描画パイプラインにおけるテクスチャの設定
enum TextureType {
	FromFile,
	OneColor,
	ColorChanged
};
struct TextureIdentifier {
	TextureIdentifier() {
		Type = FromFile;
		Path = "";
		Color1 = 0;
		Color2 = 0;
		Color3 = 0;
	}
	TextureIdentifier(std::string path) {
		Type = FromFile;
		Path = path;
		Color1 = 0;
		Color2 = 0;
		Color3 = 0;
	}
	TextureIdentifier(DirectX::XMVECTOR color) {
		Type = OneColor;
		Path = "";
		Color1 = getColorInt(color);
		Color2 = 0;
		Color3 = 0;
	}
	TextureIdentifier(std::string path,DirectX::XMVECTOR color1, DirectX::XMVECTOR color2, DirectX::XMVECTOR color3) {
		Type = ColorChanged;
		Path = path;
		Color1 = getColorInt(color1);
		Color2 = getColorInt(color2);
		Color3 = getColorInt(color3);
	}
	int getColorInt(DirectX::XMVECTOR colorVec) {
		byte iRed = min(255, max(0, (byte)(colorVec.m128_f32[0] * 255)));
		byte iGreen = min(255, max(0, (byte)(colorVec.m128_f32[1] * 255)));
		byte iBlue = min(255, max(0, (byte)(colorVec.m128_f32[2] * 255)));
		byte iAlpha = min(255, max(0, (byte)(colorVec.m128_f32[3] * 255)));
		return iRed + iGreen * 256 + iBlue * 256 * 256 + iAlpha * 256 * 256 * 256;
	}
	TextureType Type;
	std::string Path;
	int Color1;
	int Color2;
	int Color3;
};
static bool operator<(const TextureIdentifier& lhs, const TextureIdentifier& rhs) {
	if (lhs.Type != rhs.Type) {
		return lhs.Type < rhs.Type;
	}
	else if (lhs.Color1 != rhs.Color1) {
		return lhs.Color1 < rhs.Color1;
	}
	else if (lhs.Color2 != rhs.Color2) {
		return lhs.Color2 < rhs.Color2;
	}
	else if (lhs.Color3 != rhs.Color3) {
		return lhs.Color3 < rhs.Color3;
	}
	else if (lhs.Path != rhs.Path) {
		return std::hash<std::string>()(lhs.Path) < std::hash<std::string>()(rhs.Path);
	}
	return false;
}
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
	std::map<TextureIdentifier, int> IndexTable;
	std::vector<DirectX::XMFLOAT4> BlackboxMatrices;
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
		IndexTable = std::map<TextureIdentifier, int>();
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
		BlackboxMatrices = std::vector<DirectX::XMFLOAT4>(arraySize);
	}
	static std::unique_ptr<DirectX::ScratchImage> LoadFromFile(std::string filePath) {
		setlocale(LC_CTYPE, "jpn");
		wchar_t wFilename[256];
		size_t ret;
		mbstowcs_s(&ret, wFilename, filePath.c_str(), 256);
		auto image = std::make_unique<DirectX::ScratchImage>();
		DirectX::TexMetadata m_info = {};
		if (FAILED(DirectX::LoadFromWICFile(wFilename, DirectX::WIC_FLAGS_ALL_FRAMES, &m_info, *image)))
		{
			m_info = {};
			throw("ファイル名：" + filePath + "というテクスチャファイルはありません。");
		}
		return image;
	}
	static std::unique_ptr<DirectX::ScratchImage> MakeImageFromColorVector(std::vector<byte> colors, int width, int height,int bytePerPixel,int pageCount) {
		if (width * height * bytePerPixel * pageCount != sizeof(byte) * colors.size()) {
			throw("");
		}
		DirectX::Image img;
		img.pixels = (uint8_t*)(&colors[0]);
		img.format = DXGI_FORMAT_R8G8B8A8_UNORM;
		img.width = width;
		img.height = height;
		img.rowPitch = width * bytePerPixel;
		img.slicePitch = width * height * bytePerPixel;
		std::unique_ptr<DirectX::ScratchImage> pScratchImage = std::make_unique<DirectX::ScratchImage>(DirectX::ScratchImage());
		pScratchImage->InitializeArrayFromImages(&img, pageCount);
		return pScratchImage;
	}
	// バックバッファの方を読んでるので画面をスワップしなくても良い
	static std::unique_ptr<DirectX::ScratchImage> MakeImageFromRTV(int top,int left,int width,int height) {
		ID3D11Texture2D* pRTVbuffer;
		if (FAILED(D3D.m_swapChain->GetBuffer(0, IID_PPV_ARGS(&pRTVbuffer))))
		{
			throw("");
		}
		ID3D11Texture2D* pTexture;
		D3D11_TEXTURE2D_DESC desc;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.BindFlags = 0;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		desc.MipLevels = 1;
		desc.Width = width;
		desc.Height = height;
		desc.ArraySize = 1;
		desc.Usage = D3D11_USAGE_STAGING;
		desc.MiscFlags = 0;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		if (FAILED(D3D.m_device->CreateTexture2D(&desc, nullptr, &pTexture))) {
			throw("");
		}
		D3D11_BOX box;
		box.left = left;
		box.right = left + width;
		box.top = top;
		box.bottom = top + height;
		box.front = 0;
		box.back = 1;
		D3D11_TEXTURE2D_DESC t2Desc = {};
		ZeroMemory(&t2Desc, sizeof(t2Desc));
		pRTVbuffer->GetDesc(&t2Desc);
		D3D.m_deviceContext->CopySubresourceRegion(pTexture, 0, 0, 0, 0, pRTVbuffer, 0, &box);
		D3D11_MAPPED_SUBRESOURCE mapped;
		if (FAILED(D3D.m_deviceContext->Map(pTexture, 0, D3D11_MAP_READ, 0, &mapped))) {
			throw("");
		}
		std::vector<byte> vData = std::vector<byte>(width * height * 4);
		if (memcpy_s((void*)(&vData[0]), width * height * 4, mapped.pData, width * height * 4)) {
			throw("");
		}
		for (int i = 0; i < vData.size() / 4; i++) {
			vData[i * 4 + 3] = 255;
		}
		D3D.m_deviceContext->Unmap(pTexture, 0);
		pTexture->Release();
		return MakeImageFromColorVector(vData, width, height, 4, 1);
	}
	static std::unique_ptr<DirectX::ScratchImage> GetTransparentFromOpaque(std::unique_ptr<DirectX::ScratchImage> blackBack, std::unique_ptr<DirectX::ScratchImage> whiteBack) {
		DirectX::Image black = blackBack->GetImages()[0];
		DirectX::Image white = whiteBack->GetImages()[0];
		if (black.width != white.width || black.height != white.height) {
			throw("");
		}
		std::vector<byte> transparent(black.width * black.height * 4);
		for (int i = 0; i < black.width * black.height; i++) {
			// 黒背景でも白背景でも色が同じなら完全に不透明
			transparent[i * 4 + 3] = 255 - (white.pixels[i * 4] - black.pixels[i * 4]);
			for (int j = 0; j < 3; j++) {
				transparent[i * 4 + j] = (byte)roundf(255 * black.pixels[i * 4 + j] / (float)(black.pixels[i * 4 + j] + (255 - white.pixels[i * 4 + j])));
			}
		}
		return MakeImageFromColorVector(transparent, black.width, black.height, 4, 1);
	}
	int AppendImage(std::unique_ptr<DirectX::ScratchImage> image) {
		DirectX::TexMetadata m_info = image->GetMetadata();
		if (m_info.width != Width || m_info.height != Width) {
			throw("");
		}
		if (TexCount + image->GetImageCount() > BlackboxMatrices.size()) {
			throw("");
		}
		// テクスチャの外側のエリアを求める
		// この辺のデータは完成版ではメタデータに書き出せると良い(ロードが早くなる)
		for (int i = 0; i < image->GetImageCount();i++) {
			uint8_t* pixels= image->GetImages()[i].pixels;
			float bbTop = 0;
			float bbBottom = Width;
			float bbLeft = 0;
			float bbRight = Width;
			bool found = false;
			for (int u = 0; u < Width; u++) {
				for (int v = 0; v < Width; v++) {
					if (pixels[4 * (Width * v + u) + 3] > 0) {
						bbLeft = u;
						found = true;
						break;
					}
				}
				if (found)break;
			}
			if (bbLeft == Width - 1) {
				bbLeft = 0;
				bbRight = 0;
				bbTop = 0;
				bbBottom == 0;
			}
			else {
				found = false;
				for (int u = Width - 1; u >= 0; u--) {
					for (int v = 0; v < Width; v++) {
						if (pixels[4 * (Width * v + u) + 3] > 0) {
							bbRight = u + 1;
							found = true;
							break;
						}
					}
					if (found)break;
				}
				found = false;
				for (int v = 0; v < Width; v++) {
					for (int u = 0; u < Width; u++) {
						if (pixels[4 * (Width * v + u) + 3] > 0) {
							bbTop = v;
							found = true;
							break;
						}
					}
					if (found)break;
				}
				found = false;
				for (int v = Width - 1; v >= 0; v--) {
					for (int u = 0; u < Width; u++) {
						if (pixels[4 * (Width * v + u) + 3] > 0) {
							bbBottom = v + 1;
							found = true;
							break;
						}
					}
					if (found)break;
				}
			}
			bbTop = bbTop / Width;
			bbBottom = bbBottom / Width;
			bbLeft = bbLeft / Width;
			bbRight = bbRight / Width;
			DirectX::XMVECTOR blackBox{
				bbRight - bbLeft,bbBottom - bbTop,bbLeft,bbTop
			};
			DirectX::XMStoreFloat4(&BlackboxMatrices[TexCount + i], blackBox);
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
		image.release();
		return headIndex;
	}
	static void SaveImage(std::unique_ptr<DirectX::ScratchImage> pScratchImage,std::string filePath) {
		wchar_t* wPath = new wchar_t[256];
		if (FAILED(mbstowcs_s(nullptr, wPath, 256, filePath.c_str(), 256))) {
			throw("");
		}
		if (FAILED(DirectX::SaveToWICFile(pScratchImage->GetImages(), pScratchImage->GetMetadata().arraySize, DirectX::WIC_FLAGS_ALL_FRAMES, DirectX::GetWICCodec(DirectX::WIC_CODEC_TIFF), wPath))) {
			throw("");
		}
		delete[] wPath;
		pScratchImage.release();
	}
	int AppendFromFileName(std::string filePath) {
		auto itr = IndexTable.find(TextureIdentifier(filePath));
		if (itr!=IndexTable.end()) {
			return itr->second;
		}
		int headIndex = AppendImage(LoadFromFile(filePath));
		IndexTable.emplace(TextureIdentifier(filePath), headIndex);
		return headIndex;
	}
	int Append(PathBindJsonValue textureJson) {
		if (Interface::SameString(textureJson.get("kind").asString(), "file")) {
			return AppendFromFileName(textureJson.get("filePath").asString());
		}
		else if (Interface::SameString(textureJson.get("kind").asString(), "oneColor")) {
			return AppendOneColorTexture(textureJson.get("color").asVector());
		}
		else if (Interface::SameString(textureJson.get("kind").asString(), "changeColor")) {
			return AppendColorChangedTexture(
				textureJson.get("filePath").asString(),
				textureJson.get("color1").asVector(),
				textureJson.get("color2").asVector(),
				textureJson.get("color3").asVector());
		}
		else {
			throw("");
		}
	}
	void SetToGraphicPipeLine() {
		D3D.m_deviceContext->PSSetShaderResources(0, 1, m_srv.GetAddressOf());
	}
	// 入力は0~1
	int AppendOneColorTexture(DirectX::XMVECTOR color) {
		auto itr = IndexTable.find(TextureIdentifier(color));
		if (itr != IndexTable.end()) {
			return itr->second;
		}
		byte iRed = min(255, max(0, (byte)(color.m128_f32[0] * 255)));
		byte iGreen = min(255, max(0, (byte)(color.m128_f32[1] * 255)));
		byte iBlue = min(255, max(0, (byte)(color.m128_f32[2] * 255)));
		byte iAlpha = min(255, max(0, (byte)(color.m128_f32[3] * 255)));
		std::vector<byte> pInit = std::vector<byte>(Width * Width * 4, 0);
		for (int i = 0; i < Width * Width; i++) {
			pInit[i * 4] = iRed;
			pInit[i * 4 + 1] = iGreen;
			pInit[i * 4 + 2] = iBlue;
			pInit[i * 4 + 3] = iAlpha;
		}
		int headIndex = AppendImage(MakeImageFromColorVector(pInit, Width, Width, 4, 1));
		IndexTable.emplace(TextureIdentifier(color), headIndex);
		return headIndex;
	}
	static std::unique_ptr<DirectX::ScratchImage> ChangeColor(std::unique_ptr<DirectX::ScratchImage> scratchImage, DirectX::XMVECTOR color1, DirectX::XMVECTOR color2, DirectX::XMVECTOR color3) {
		float colors[9];
		colors[0] = color1.m128_f32[0];
		colors[1] = color1.m128_f32[1];
		colors[2] = color1.m128_f32[2];
		colors[3] = color2.m128_f32[0];
		colors[4] = color2.m128_f32[1];
		colors[5] = color2.m128_f32[2];
		colors[6] = color3.m128_f32[0];
		colors[7] = color3.m128_f32[1];
		colors[8] = color3.m128_f32[2];
		int sliceLength = scratchImage->GetMetadata().height * scratchImage->GetMetadata().width;
		for (int image = 0; image < scratchImage->GetImageCount(); image++) {
			uint8_t* pixelsOfOneImage = scratchImage->GetImages()[image].pixels;
			for (int pixel = 0; pixel < sliceLength; pixel++) {
				float red = 0;
				float green = 0;
				float blue = 0;
				for (int color = 0; color < 3; color++) {
					red = red + pixelsOfOneImage[pixel * 4 + color] * colors[color * 3 + 0];
					green = green + pixelsOfOneImage[pixel * 4 + color] * colors[color * 3 + 1];
					blue = blue + pixelsOfOneImage[pixel * 4 + color] * colors[color * 3 + 2];
				}
				pixelsOfOneImage[pixel * 4 + 0] = min(255, red);
				pixelsOfOneImage[pixel * 4 + 1] = min(255, green);
				pixelsOfOneImage[pixel * 4 + 2] = min(255, blue);
			}
		}
		return scratchImage;
	}
	int AppendColorChangedTexture(std::string filePath, DirectX::XMVECTOR color1, DirectX::XMVECTOR color2, DirectX::XMVECTOR color3) {
		auto itr = IndexTable.find(TextureIdentifier(filePath,color1,color2,color3));
		if (itr != IndexTable.end()) {
			return itr->second;
		}
		std::unique_ptr<DirectX::ScratchImage> scratchImage = LoadFromFile(filePath);
		scratchImage = ChangeColor(std::move(scratchImage), color1, color2, color3);
		int headIndex = AppendImage(std::move(scratchImage));
		IndexTable.emplace(TextureIdentifier(filePath, color1, color2, color3), headIndex);
		return headIndex;
	}
	static void SaveShadowTexture(int width, std::string filePath, int topShadowWidth, int bottomShadowWidth, int leftShadowWidth, int rightShadowWidth) {
		int imgCount = pow(2, 8);
		std::vector<byte> data = std::vector<byte>();
		for (int i = 0; i < imgCount; i++) {
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
					data.push_back(iBase);
					data.push_back(iGreen);
					data.push_back(iShadow);
					data.push_back(iAlpha);
				}
			}
		}
		SaveImage(MakeImageFromColorVector(data, width, width, 4, imgCount),filePath);
	}

	static void SaveWallMask(int width, std::string filePath) {
		int imgCount = pow(2, 8);
		std::vector<byte> data = std::vector<byte>();
		for (int i = 0; i < imgCount; i++) {
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
					data.push_back(iBase);
					data.push_back(iGreen);
					data.push_back(iShadow);
					data.push_back(iAlpha);
				}
			}
		}
		SaveImage(MakeImageFromColorVector(data, width, width, 4, imgCount), filePath);
	}
	static void SaveLensMask(int width, std::string filePath) {
		int imgCount = 1;
		std::vector<byte> data = std::vector<byte>();
		for (int i = 0; i < imgCount; i++) {
			for (int y = 0; y < width; y++) {
				for (int x = 0; x < width; x++) {
					byte iBase = 0;
					byte iGreen = 0;
					byte iShadow = 0;
					byte iAlpha = 0;
					iAlpha = max(0, min(255, (-4 * (pow(abs(x / (float)width - 0.5) * 2, 4) + pow(abs(y / (float)width - 0.5) * 2, 4)) + 5) * 255));
					iAlpha = 255 - iAlpha;
					data.push_back(iBase);
					data.push_back(iGreen);
					data.push_back(iShadow);
					data.push_back(iAlpha);
				}
			}
		}
		SaveImage(MakeImageFromColorVector(data, width, width, 4, imgCount), filePath);
	}
	void Release() {
		if (m_srv) {
			m_srv.Reset();
		}
		if (m_texture) {
			m_texture.Reset();
		}
	}
};