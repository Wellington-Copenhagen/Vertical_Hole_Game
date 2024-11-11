#pragma once

#include "DirectX.h"

//=========================================
// �e�N�X�`���N���X
//=========================================
class Texture
{
public:
	//GPU�̃�������Ƀe�N�X�`�����ǂݍ��܂�Ă���Ƀn���h���݂�����
	// �V�F�[�_�[���\�[�X�r���[(�摜�f�[�^�ǂݎ��n���h��)
	ComPtr<ID3D11ShaderResourceView> m_srv = nullptr;
	// �摜���
	DirectX::TexMetadata m_info = {};

	// �摜�t�@�C����ǂݍ���
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