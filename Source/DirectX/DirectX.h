#pragma once
#include "framework.h"
#define PI 3.1415926535
#define TeamCount 2
#define StatusCount 3
//DirectX�n�̃t�@�C���͑S�������o�R��include����

// Direct3D�̃��C�u�������g�p�ł���悤�ɂ���
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

// Direct3D�̌^�E�N���X�E�֐��Ȃǂ��Ăׂ�悤�ɂ���
#include <d3d11.h>
#include <d3dcompiler.h>

// DirectXMath(���w���C�u����)���g�p�ł���悤�ɂ���
#include <DirectXMath.h>

// DirectX�e�N�X�`�����C�u�������g�p�ł���悤�ɂ���
#include <DirectXTex.h>
//NuGet����C���X�g�[������K�v����

// ComPtr���g�p�ł���悤�ɂ���
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

// �����DirectX�֌W�̃w�b�_�[���C���N���[�h

#include "Direct3D.h"