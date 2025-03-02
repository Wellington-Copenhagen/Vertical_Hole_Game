#pragma once

#include "framework.h"
#include "Source/DirectX/DirectX.h"
#include "Camera.h"
#include "Appearance.h"
#include "Input.h"
#include "HitBox.h"
#include "System.h"
#include "Entity.h"
#include "Source/DirectX/Buffers.h"
#include "Source/DirectX/Texture.h"
#include "Source/DirectX/DrawPipe.h"

//=========================================
// GameSystem�N���X
// �E���̃Q�[���̓y��ƂȂ����
//=========================================
class GameSystem
{
public :
	AllEntities mAllEntities;
	AllSystem mAllSystem;
	//�}�b�v�ő�T�C�Y�ɏ��ƕǑS���z�u�����ꍇ�̑傫��
	Appearances<Interface::BlockDrawCallType, Interface::BlockInstanceType, 1, 256 * 256 * 2> mBlockAppearances;
	//�e�A�{�́A�͗l
	Appearances<Interface::BallDrawCallType, Interface::BallInstanceType, 1, MaxBallCount> mBallAppearances[3];
	Appearances<Interface::BulletDrawCallType, Interface::BulletInstanceType, 1, 256 * 256> mBulletAppearances;
	Hurtboxes mHurtboxes;
	ConstantBuffer mCBuffer;
	VertexBuffer<Interface::BlockDrawCallType, 4,0> mBlockDrawCallBuffer;
	VertexBuffer<Interface::BlockInstanceType, 256 * 256 * 2,1> mBlockInstanceBuffer;
	//�e�A�{�́A�͗l�A�e
	VertexBuffer<Interface::BallDrawCallType, 4,0> mBallDrawCallBuffer[3];
	//�e�A�{�́A�͗l
	VertexBuffer<Interface::BallInstanceType, MaxBallCount,1> mBallInstanceBuffer[3];
	VertexBuffer<Interface::BulletDrawCallType, 4,0> mBulletDrawCallBuffer;
	VertexBuffer<Interface::BulletInstanceType, 256 * 256,1> mBulletInstanceBuffer;
	GraphicProcessSetter mGraphicProcessSetter;
	int DrawCallUsed;
	Camera mCamera;
	int Tick;


	SameFormatTextureArray mSameFormatTextureArray;
	// ���̃Q�[���̏����ݒ���s��
	void Initialize(HWND hWnd);

	// ���̃Q�[�����E�̎��Ԃ�i�߂�(���������s����)
	void Execute(HWND hWnd);

	void LoadMap(std::string mapFileName);

	void ApplyInput();

	// ���̑��A�Q�[���p�̃f�[�^�Ȃǂ��R�R�ɏ���

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


