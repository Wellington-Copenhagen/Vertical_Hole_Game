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

//=========================================
// GameSystem�N���X
// �E���̃Q�[���̓y��ƂȂ����
//=========================================
extern SameFormatTextureArray<256> BlockTextureArray;
extern SameFormatTextureArray<256> BallTextureArray;
extern SameFormatTextureArray<256> BulletTextureArray;
extern SameFormatTextureArray<256> LineTextureArray;
class GameSystem
{
public :
	AllSystem mAllSystem;
	Entities mEntities;


	Hurtboxes mHurtboxes;
	ConstantBuffer mCBuffer;

	// ���_�o�b�t�@�ƒ��_�̃f�[�^���L�^����Appearance

	//�}�b�v�ő�T�C�Y�ɏ��ƕǑS���z�u�����ꍇ�̑傫��
	VertexBuffer<Interface::BlockDrawCallType, 4,0> mFloorDrawCallBuffer;
	VertexBuffer<Interface::BlockDrawCallType, 4, 0> mWallDrawCallBuffer;
	Appearances<Interface::BlockDrawCallType, Interface::BlockInstanceType, 1, WorldWidth* WorldHeight> mFloorAppearances;
	VertexBuffer<Interface::BlockInstanceType, WorldWidth* WorldHeight,1> mFloorInstanceBuffer;
	VertexBuffer<Interface::BlockInstanceType, WorldWidth* WorldHeight, 1> mWallInstanceBuffer;
	Appearances<Interface::BlockDrawCallType, Interface::BlockInstanceType, 1, WorldWidth* WorldHeight> mWallAppearances;


	//�e�A�{�́A�͗l
	VertexBuffer<Interface::BallDrawCallType, 4,0> mBallDrawCallBuffer[3];
	VertexBuffer<Interface::BallInstanceType, MaxBallCount,1> mBallInstanceBuffer[3];
	Appearances<Interface::BallDrawCallType, Interface::BallInstanceType, 1, MaxBallCount> mBallAppearances[3];



	VertexBuffer<Interface::BulletDrawCallType, 4,0> mBulletDrawCallBuffer;
	VertexBuffer<Interface::BulletInstanceType, MaxBulletCount,1> mBulletInstanceBuffer;
	Appearances<Interface::BulletDrawCallType, Interface::BulletInstanceType, 1, MaxBulletCount> mBulletAppearances;

	VertexBuffer<Interface::LineDrawCallType, 4, 0> mLineDrawCallBuffer;
	VertexBuffer<Interface::LineInstanceType, MaxLineCount, 1> mLineInstanceBuffer;
	Appearances<Interface::LineDrawCallType, Interface::LineInstanceType, 1, MaxLineCount> mLineAppearances;



	int DrawCallUsed;
	Camera mCamera;
	std::chrono::system_clock::time_point RefreshedTimeStamp;

	HDC hdc;
	// 32*32*64
	GraphicalStringDraw<65536,2048,32> StringDrawTest;

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

