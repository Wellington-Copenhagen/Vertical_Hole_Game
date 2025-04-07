#pragma once
#include "DirectX.h"
#include "framework.h"
#include "Interfaces.h"
// �C���v�b�g���C�A�E�g�ƒ��_�V�F�[�_�[
// �e���v���[�g���g���Ă�͕̂s��v�����o���ăG���[��f�����邽��
template<typename DCType,typename IType>
struct VertexShaderAndInputLayout {
	ComPtr<ID3D11VertexShader> mVertexShader;
	ComPtr<ID3D11InputLayout> mInputLayout;
	static int SizeOfDXGIFormat(DXGI_FORMAT format) {
		if (format == DXGI_FORMAT_R32_FLOAT) {
			return sizeof(float);
		}
		else if (format == DXGI_FORMAT_R32G32_FLOAT) {
			return sizeof(float) * 2;
		}
		else if (format == DXGI_FORMAT_R32G32B32_FLOAT) {
			return sizeof(float) * 3;
		}
		else if (format == DXGI_FORMAT_R32G32B32A32_FLOAT) {
			return sizeof(float) * 4;
		}
		else {
			throw("�s����DXGI_FORMAT");
		}
	}
	void Compile(
		LPCWSTR vertexShaderPath,
		std::vector<D3D11_INPUT_ELEMENT_DESC> layoutDesc) {
		int drawcallSize = 0;
		int instanceSize = 0;
		for (auto elementDesc : layoutDesc) {
			if (elementDesc.InputSlotClass == D3D11_INPUT_PER_INSTANCE_DATA) {
				instanceSize = instanceSize + SizeOfDXGIFormat(elementDesc.Format);
			}
			if (elementDesc.InputSlotClass == D3D11_INPUT_PER_VERTEX_DATA) {
				drawcallSize = drawcallSize + SizeOfDXGIFormat(elementDesc.Format);
			}
		}
		if (drawcallSize != sizeof(DCType) || instanceSize != sizeof(IType)) {
			throw("Input Layout�ƃo�b�t�@�Ɏg���^�̃T�C�Y���s��v");
		}



		// ���_�V�F�[�_�[��ǂݍ��݁��R���p�C��
		ComPtr<ID3DBlob> compiledVS;
		ComPtr<ID3DBlob> pError;
		if (FAILED(D3DCompileFromFile(vertexShaderPath, nullptr, nullptr, "main", "vs_5_0", 0, 0, &compiledVS, &pError)))
		{
			OutputDebugStringA((char*)pError->GetBufferPointer());
			throw("���_�V�F�[�_�̃R���p�C���Ɏ��s");
		}
		// ���_�V�F�[�_�[�쐬
		if (FAILED(D3D.m_device->CreateVertexShader(compiledVS->GetBufferPointer(), compiledVS->GetBufferSize(), nullptr, mVertexShader.GetAddressOf())))
		{
			throw("���_�V�F�[�_�̐����Ɏ��s");
		}
		// �P���_�̏ڍׂȏ��
		//���_�f�[�^�̌`���������Ă���
		// ���_�C���v�b�g���C�A�E�g�쐬
		if (FAILED(D3D.m_device->CreateInputLayout(&layoutDesc[0], layoutDesc.size(), compiledVS->GetBufferPointer(), compiledVS->GetBufferSize(), mInputLayout.GetAddressOf())))
		{
			throw("���_�C���v�b�g���C�A�E�g�̐����Ɏ��s");
		}
	}
	void SetToDrawPipe() {
		D3D.m_deviceContext->VSSetShader(mVertexShader.Get(), 0, 0);
		D3D.m_deviceContext->IASetInputLayout(mInputLayout.Get());
	}
};
//�`��p�C�v���C���ɂ�����V�F�[�_�[���\�[�X�ȊO�̐ݒ�
class GraphicProcessSetter {
public:
	static int Width;
	static int Height;
	
	GraphicProcessSetter() {

	}

	static ComPtr<ID3D11DepthStencilState> m_depthStencilStateEnable;
	static ComPtr<ID3D11DepthStencilState> m_depthStencilStateDisable;

	static ComPtr<ID3D11BlendState> m_blendStateEnable;
	static ComPtr<ID3D11BlendState> m_blendStateDisable;

	static ComPtr<ID3D11RasterizerState> m_rasterizerState;

	static ComPtr<ID3D11SamplerState> m_samplerState;

	static VertexShaderAndInputLayout<Interface::BlockDrawCallType, Interface::BlockInstanceType> mBlockVShaderLayout;
	static ComPtr<ID3D11PixelShader> mBlockPixelShader;


	static VertexShaderAndInputLayout<Interface::BallDrawCallType, Interface::BallInstanceType> mBallVShaderLayout;
	static ComPtr<ID3D11PixelShader> mBallPixelShader;

	static VertexShaderAndInputLayout<Interface::BulletDrawCallType, Interface::BulletInstanceType> mBulletVShaderLayout;
	static ComPtr<ID3D11PixelShader> mBulletPixelShader;

	static VertexShaderAndInputLayout<Interface::EffectDrawCallType, Interface::EffectInstanceType> mEffectVShaderLayout;
	static ComPtr<ID3D11PixelShader> mEffectPixelShader;

	static VertexShaderAndInputLayout<Interface::CharDrawCallType, Interface::CharInstanceType> mCharVShaderLayout;
	static ComPtr<ID3D11PixelShader> mCharPixelShader;

	static VertexShaderAndInputLayout<Interface::GeneralDrawCallType, Interface::GeneralInstanceType> mGeneralVShaderLayout;
	static VertexShaderAndInputLayout<Interface::LineDrawCallType, Interface::LineInstanceType> mLineVShaderLayout;
	static VertexShaderAndInputLayout<Interface::FreeShapeDrawCallType, Interface::FreeShapeInstanceType> mFreeShapeVShaderLayout;

	static ComPtr<ID3D11PixelShader> mGeneralPixelShader;
	static void SetAsBlock();
	static void SetAsBall();
	static void SetAsBullet();
	static void SetAsEffect();
	static void SetAsCharacter();
	static void SetAsGeneral();
	static void SetAsLine();
	static void SetAsFreeShape();
	static void CompilePixelShader(LPCWSTR pixelShaderPath, ID3D11PixelShader** ppPixelShader);
	GraphicProcessSetter(int width, int height);
};
#define DP GraphicProcessSetter::GetInstance()