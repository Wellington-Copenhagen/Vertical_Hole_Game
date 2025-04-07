#pragma once

#include "framework.h"
#include "Source/DirectX/DirectX.h"
#include "Camera.h"
#include "Input.h"
#include "HitBox.h"
#include "System.h"
#include "Component.h"
#include "Entity.h"
#include "Source/DirectX/Buffers.h"
#include "Source/DirectX/Texture.h"
#include "Source/DirectX/DrawPipe.h"
#include "Source/DirectX/Appearance.h"
#include "Routing.h"
#include "Source/DirectX/StringDraw.h"
#include "Source/DirectX/LineDraw.h"
#include "UserInterface.h"

//=========================================
// GameSystem�N���X
// �E���̃Q�[���̓y��ƂȂ����
//=========================================
extern SameFormatTextureArray BlockTextureArray;
extern SameFormatTextureArray BallTextureArray;
extern SameFormatTextureArray BulletTextureArray;
extern SameFormatTextureArray EffectTextureArray;
class GameSystem
{
public :


	AllSystem mAllSystem;
	Entities mEntities;


	Hurtboxes mHurtboxes;
	ConstantBuffer mCBuffer;

	// ���_�o�b�t�@�ƒ��_�̃f�[�^���L�^����Appearance

	//�}�b�v�ő�T�C�Y�ɏ��ƕǑS���z�u�����ꍇ�̑傫��
	VertexBuffer<Interface::BlockDrawCallType> mFloorDrawCallBuffer;
	VertexBuffer<Interface::BlockDrawCallType> mWallDrawCallBuffer;
	EntityBindAppearances<Interface::BlockDrawCallType, Interface::BlockInstanceType,Component::BlockAppearance, 1> mFloorAppearances;
	VertexBuffer<Interface::BlockInstanceType> mFloorInstanceBuffer;
	VertexBuffer<Interface::BlockInstanceType> mWallInstanceBuffer;
	EntityBindAppearances<Interface::BlockDrawCallType, Interface::BlockInstanceType, Component::BlockAppearance, 1> mWallAppearances;


	//�e�A�{�́A�͗l
	VertexBuffer<Interface::BallDrawCallType> mBallDrawCallBuffer[3];
	VertexBuffer<Interface::BallInstanceType> mBallInstanceBuffer[3];
	EntityBindAppearances<Interface::BallDrawCallType, Interface::BallInstanceType, Component::BallAppearance, 3> mBallAppearances;



	VertexBuffer<Interface::BulletDrawCallType> mBulletDrawCallBuffer;
	VertexBuffer<Interface::BulletInstanceType> mBulletInstanceBuffer;
	EntityBindAppearances<Interface::BulletDrawCallType, Interface::BulletInstanceType,Component::BulletAppearance, 1> mBulletAppearances;

	VertexBuffer<Interface::EffectDrawCallType> mEffectDrawCallBuffer;
	VertexBuffer<Interface::EffectInstanceType> mEffectInstanceBuffer;
	EntityBindAppearances<Interface::EffectDrawCallType, Interface::EffectInstanceType,Component::EffectAppearance, 1> mEffectAppearances;



	int DrawCallUsed;
	Camera mCamera;
	std::chrono::system_clock::time_point RefreshedTimeStamp;

	HDC hdc;
	// 32*32*64
	GraphicalStringDraw StringDrawTest;
	UserInterface mUserInterface;
 
	// ���̃Q�[���̏����ݒ���s��
	void Initialize(HWND hWnd);

	// ���̃Q�[�����E�̎��Ԃ�i�߂�(���������s����)
	void Execute(HWND hWnd);


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

