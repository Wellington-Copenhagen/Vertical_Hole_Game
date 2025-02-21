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
	static ComPtr<ID3D11ShaderResourceView> m_srv;
	static ComPtr<ID3D11Texture2D> m_texture;
	static std::vector<std::string> FilePaths;
	static int MipLevel;
	SameFormatTextureArray() {

	};
	SameFormatTextureArray(int width, int height, int separation, int arraySize);
	static int Append(std::string filePath);
	static void SetToGraphicPipeLine();
};