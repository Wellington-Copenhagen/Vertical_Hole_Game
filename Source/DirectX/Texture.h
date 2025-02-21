#pragma once

#include "DirectX.h"
#include "Direct3D.h"
#include "Interfaces.h"

//�����ɂ�����́A�s�����Ȃ��̗D�悪�悢
//�Ǐ��L�����N�^�[�G�t�F�N�g�݂�����

//�`��p�C�v���C���ɂ�����e�N�X�`���̐ݒ�
class Texture
{
public:
	//GPU�̃�������Ƀe�N�X�`�����ǂݍ��܂�Ă���Ƀn���h���݂�����
	// �V�F�[�_�[���\�[�X�r���[(�摜�f�[�^�ǂݎ��n���h��)
	ComPtr<ID3D11ShaderResourceView> m_srv = nullptr;
	// �摜���
	DirectX::TexMetadata m_info = {};

	// �摜�t�@�C����ǂݍ���
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