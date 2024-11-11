#pragma once
/*
// Direct3D�̃��C�u�������g�p�ł���悤�ɂ���
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib,"d3dcompiler.lib")

// Direct3D�̌^�E�N���X�E�֐��Ȃǂ��Ăׂ�悤�ɂ���
#include <d3d11.h>
#include <d3dcompiler.h>

// DirectXMath(���w���C�u����)���g�p�ł���悤�ɂ���
#include <DirectXMath.h>

// ComPtr���g�p�ł���悤�ɂ���
#include <wrl/client.h>
*/

#include "framework.h"

#include "Source/DirectX/DirectX.h"

using Microsoft::WRL::ComPtr;

//=========================================
// Direct3D�N���X
//=========================================
class Direct3D
{
public:
	int Width;
	int Height;

	// ���ϐ��͍���͑S��public�ɂ��܂����A�{����private�ŉB���ׂ���

	// Direct3D�f�o�C�X
	ComPtr<ID3D11Device>		m_device;
	// Direct3D�f�o�C�X�R���e�L�X�g
	ComPtr<ID3D11DeviceContext>	m_deviceContext;
	// �X���b�v�`�F�C��
	ComPtr<IDXGISwapChain>		m_swapChain;
	// �o�b�N�o�b�t�@�[��RT�r���[
	ComPtr<ID3D11RenderTargetView> m_backBufferView;
	// �[�x�X�e���V���r���[
	ComPtr<ID3D11DepthStencilView> m_depthStencilView;

	//--------------------------------------------
	// Direct3D�����������A�g�p�ł���悤�ɂ���֐�
	// hWnd		: �E�B���h�E�n���h��
	// width	: ��ʂ̕�
	// height	: ��ʂ̍���
	//--------------------------------------------
	bool Initialize(HWND hWnd, int width, int height);


	//=========================================
	// ���񂱂̃N���X�́A�ǂ�����ł��A�N�Z�X�ł���悤��
	// �V���O���g���p�^�[���ɂ��܂�
	// �������ȉ��A�V���O���g���p�^�[���̃R�[�h
	//=========================================
private:
	// �B��̃C���X�^���X�p�̃|�C���^
	static inline Direct3D* s_instance;
	// �R���X�g���N�^��private�ɂ���
	//Create�����Init���ĂԂ��Ƃ��O��ɂȂ��Ă�̂ł����͎g���Ȃ�
	Direct3D() {}
public:
	// �C���X�^���X�쐬
	static void CreateInstance()
	{
		//�V���O���g���p�^�[���̕ώ�
		//�Â��C���X�^���X�������Ă���V�����̂����
		//Create��1�񂵂��Ă΂Ȃ��̂Ȃ�֌W�Ȃ��b����
		DeleteInstance();

		s_instance = new Direct3D();
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
	static Direct3D& GetInstance()
	{
		return *s_instance;
	}
};

// Direct3D�̗B��̃C���X�^���X���ȒP�Ɏ擾���邽�߂̃}�N��
#define D3D Direct3D::GetInstance()