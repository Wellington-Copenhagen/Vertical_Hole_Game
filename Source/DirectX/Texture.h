#pragma once

#include "DirectX.h"

//=========================================
// テクスチャクラス
//=========================================
class Texture
{
public:
	//GPUのメモリ上にテクスチャが読み込まれてそれにハンドルみたいな
	// シェーダーリソースビュー(画像データ読み取りハンドル)
	ComPtr<ID3D11ShaderResourceView> m_srv = nullptr;
	// 画像情報
	DirectX::TexMetadata m_info = {};

	// 画像ファイルを読み込む
	bool Load(const std::string& filename);
};
class TextureArray {
public:
	std::string FileNames;
	int Width;
	int Height;
	int BitePerPixel;
	int NumberOfTexture;
	ComPtr<ID3D11ShaderResourceView> m_srv = nullptr;
	void Load();
};