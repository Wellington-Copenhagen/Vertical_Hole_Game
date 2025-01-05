#pragma once

#include "framework.h"
#include "Source/DirectX/DirectX.h"



#include "Source/Game/Game.h"
#include "Source/Game/Camera.h"
//=========================================
// GameSystem�N���X
// �E���̃Q�[���̓y��ƂȂ����
//=========================================
class GameSystem
{
public:
	int timeCount;


	Object Root;
	Object Sphere;
	Object Ground;
	//���_�o�b�t�@���g���܂킹�Ȃ����m���߂Ă������



	// ���̃Q�[���̏����ݒ���s��
	void Initialize();

	// ���̃Q�[�����E�̎��Ԃ�i�߂�(���������s����)
	void Execute();

	// ���̑��A�Q�[���p�̃f�[�^�Ȃǂ��R�R�ɏ���

	Texture m_tex;
	/*
	struct ConstantBufferData {
		DirectX::XMFLOAT4X4 ViewProjection;
	};
	ComPtr<ID3D11Buffer> ConstantBuffer;
	*/
//=========================================
// ���񂱂̃N���X���A�ǂ�����ł��A�N�Z�X�ł���悤��
// �V���O���g���p�^�[���ɂ��Ă����܂��B
// �������ȉ��A�V���O���g���p�^�[���̃R�[�h
//=========================================
private:
	// �B��̃C���X�^���X�p�̃|�C���^
	static inline GameSystem* s_instance;
	// �R���X�g���N�^��private�ɂ���
	GameSystem();
public:
	// �C���X�^���X�쐬
	static void CreateInstance()
	{
		DeleteInstance();

		s_instance = new GameSystem();
	}
	// �C���X�^���X�폜
	static void DeleteInstance()
	{
		if (s_instance != nullptr)
		{
			delete s_instance;
			s_instance = nullptr;
		}
	}
	// �B��̃C���X�^���X���擾
	static GameSystem& GetInstance()
	{
		return *s_instance;
	}
};

// GameSystem�̗B��̃C���X�^���X���ȒP�Ɏ擾���邽�߂̃}�N��
#define GAMESYS GameSystem::GetInstance()


