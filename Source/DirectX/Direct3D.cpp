#include "Direct3D.h"
#include "framework.h"

#include "Source/DirectX/DirectX.h"

#include <windows.h>
bool Direct3D::Initialize(HWND hWnd, int width, int height)
{
	Height = height;
	Width = width;
	//=====================================================
	// �t�@�N�g���[�쐬(�r�f�I �O���t�B�b�N�̐ݒ�̗񋓂�w��Ɏg�p�����I�u�W�F�N�g)
	//=====================================================
	ComPtr<IDXGIFactory> factory;

	if (FAILED(CreateDXGIFactory1(IID_PPV_ARGS(&factory))))
	{
		return false;
	}

	//=====================================================
	//�f�o�C�X����(��Ƀ��\�[�X�쐬���Ɏg�p����I�u�W�F�N�g)
	//=====================================================
	UINT creationFlags = 0;
#ifdef _DEBUG
	// DEBUG�r���h����Direct3D�̃f�o�b�O��L���ɂ���
	// (�������d�����ׂ����G���[���킩��)
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	//�@�\�ƃO���{�̑Ή��̃Z�b�g�݂����Ȃ���
	//�Ⴆ��750ti����11_0�܂ł����Ή����ĂȂ��̂�11_1���Ǝ��s����
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,	// Direct3D 11.1  ShaderModel 5
		D3D_FEATURE_LEVEL_11_0,	// Direct3D 11    ShaderModel 5
		D3D_FEATURE_LEVEL_10_1,	// Direct3D 10.1  ShaderModel 4
		D3D_FEATURE_LEVEL_10_0,	// Direct3D 10.0  ShaderModel 4
		D3D_FEATURE_LEVEL_9_3,	// Direct3D 9.3   ShaderModel 3
		D3D_FEATURE_LEVEL_9_2,	// Direct3D 9.2   ShaderModel 3
		D3D_FEATURE_LEVEL_9_1,	// Direct3D 9.1   ShaderModel 3
	};

	// �f�o�C�X�ƂŃf�o�C�X�R���e�L�X�g���쐬
	D3D_FEATURE_LEVEL futureLevel;
	if (FAILED(D3D11CreateDevice(
		nullptr,//��ʁH��I��ł�H
		D3D_DRIVER_TYPE_HARDWARE,//
		nullptr,//���X�^���C�Y���郉�C�u�����A�オSOFTWARE�̏ꍇ�k���|�C���^�ɂł��Ȃ�
		creationFlags,//�ݒ�݂�����
		featureLevels,//�Ή�����o�[�W�����݂����Ȋ������낤��
		_countof(featureLevels),
		D3D11_SDK_VERSION,//SDK�̃o�[�W����(���\�[�X�Œ�`����Ă�}�N��)
		&m_device,//�Ԃ��f�o�C�X��COMPtr�̃|�C���^
		&futureLevel,//futureLevels�̂����ォ�玎���Đ��������o�[�W������Ԃ�
		&m_deviceContext)))//�f�o�C�X�R���e�L�X�g��COMPtr�̃|�C���^
	{
		return false;
	}

	//=====================================================
	// �X���b�v�`�F�C���쐬(�t�����g�o�b�t�@�ɕ\���\�ȃo�b�N�o�b�t�@��������)
	//=====================================================
	DXGI_SWAP_CHAIN_DESC scDesc = {};		// �X���b�v�`�F�[���̐ݒ�f�[�^
	scDesc.BufferDesc.Width = width;						// ��ʂ̕�
	scDesc.BufferDesc.Height = height;						// ��ʂ̍���
	scDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// �o�b�t�@�̌`���A�F�̌`��
	scDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;//�X�L�������C���͕��̂̑O��֌W�𔻒肷����@
	scDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;//��ʃT�C�Y�ɍ��킹�ĕό`�����邩�ǂ���
	scDesc.BufferDesc.RefreshRate.Numerator = 0;//�����ɂȂ��ĂĒP�ʂ�Hz�炵��
	scDesc.BufferDesc.RefreshRate.Denominator = 1;
	scDesc.SampleDesc.Count = 1;							// MSAA�͎g�p���Ȃ��A�s�N�Z�����ł̐F�̔䗦�̌v�Z�@
	scDesc.SampleDesc.Quality = 0;							// MSAA�͎g�p���Ȃ�
	scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	// �o�b�t�@�̎g�p���@
	scDesc.BufferCount = 2;									// �o�b�t�@�̐�
	scDesc.OutputWindow = hWnd;
	scDesc.Windowed = TRUE;									// �E�B���h�E���[�h
	scDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;//�o�b�N�o�b�t�@�������ɖ߂�Ƃ������Ƃ��낤���c
	scDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;//and�ł��낢��ݒ�ł���B���͑S��ʂ��ǂ�����؂�ւ��Ƃ�������

	// �X���b�v�`�F�C���̍쐬
	if (FAILED(factory->CreateSwapChain(m_device.Get(), &scDesc, &m_swapChain)))
	{
		return false;
	}
	// �X���b�v�`�F�C������o�b�N�o�b�t�@���\�[�X�擾
	ComPtr<ID3D11Texture2D> pBackBuffer;
	if (FAILED(m_swapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer))))
	{
		return false;
	}

	// �o�b�N�o�b�t�@���\�[�X�p��RTV���쐬
	//RTV�͕����I�Ɏg���o�b�t�@�݂����Ȃ�H
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = scDesc.BufferDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	if (FAILED(m_device->CreateRenderTargetView(pBackBuffer.Get(), &rtvDesc, &m_backBufferView)))
	{
		return false;
	}


	ComPtr<ID3D11Texture2D> m_depthStencil;
	D3D11_TEXTURE2D_DESC t2Desc = {};
	ZeroMemory(&t2Desc, sizeof(t2Desc));
	pBackBuffer->GetDesc(&t2Desc);
	t2Desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	t2Desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	/*
	t2Desc.Width = scDesc.BufferDesc.Width;
	t2Desc.Height = scDesc.BufferDesc.Height;
	t2Desc.CPUAccessFlags = 0;
	t2Desc.MiscFlags = 0;

	t2Desc.Usage = D3D11_USAGE_DEFAULT;
	t2Desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	t2Desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	t2Desc.SampleDesc.Count = 1;
	t2Desc.SampleDesc.Quality = 0;
	t2Desc.MipLevels = 1;
	t2Desc.ArraySize = 1;
	*/
	if (FAILED(m_device->CreateTexture2D(&t2Desc, nullptr, m_depthStencil.GetAddressOf()))) {
		return false;
	}
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	ZeroMemory(&dsvDesc, sizeof(dsvDesc));
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Format = t2Desc.Format;
	dsvDesc.Texture2D.MipSlice = 0;
	if (FAILED(m_device->CreateDepthStencilView(m_depthStencil.Get(),&dsvDesc,m_depthStencilView.GetAddressOf() ))) {
		return false;
	}
	m_deviceContext->OMSetRenderTargets(1, m_backBufferView.GetAddressOf(), m_depthStencilView.Get());
	//=====================================================
	// �f�o�C�X�R���e�L�X�g�ɕ`��Ɋւ���ݒ���s���Ă���
	//=====================================================

	// �o�b�N�o�b�t�@��RT�Ƃ��ăZ�b�g
	//�o�C���h���郌���_�[�̐������ǂ�����1�������Ȃ������̂�1�Ȃ̂��낤
	//
	//��3�ϐ��͂Ȃ���[�x�X�e���V���r���[�̃|�C���^�A�����O�ɂ��邩�݂����Ȃ��񂾂낤��
	return true;
}