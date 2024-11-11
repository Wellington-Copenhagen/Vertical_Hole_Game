// GameSystem�N���X���g����悤�ɂ���
#include "Game.h"
// Direct3D�N���X���g����悤�ɂ���
//#include "Source/DirectX/Direct3D.h"

#include "framework.h"

#include "Source/DirectX/DirectX.h"
#include "Source/Game/Game.h"
#include "Source/DirectX/DrawPipe.h"

//�n�`�͂���ɂ�������3D���f���͍��Ȃ��̂Ř�p�𐧌�����
//�����̃A�N�V�����Q�[�ł��������ł���ΐ헪�A�N�V�����ɂ�����
//�ݒ�����₷�����ȈӖ��ł̓_���W����
//

DirectX::XMMATRIX rotation;
// �Q�[���̏����ݒ���s��
GameSystem::GameSystem() {

}
void GameSystem::Initialize()
{
	timeCount = 0;
	//_tex.Load("Data/ColorTable.png");
	//D3D.m_deviceContext->PSSetShaderResources(0, 1, m_tex.m_srv.GetAddressOf());
	//���߂͏�������Ȃ�(0,0,0,0)�ƔF������č����Ȃ�

	// �e�N�X�`����T���v���[�̓n���͈�x�ŗǂ�(�������X���b�g�ɂ����炭���肪����̂ł���ȏ�Ɏg�������ꍇ�͓r���œ���ւ���K�v������̂��Ǝv��)
	
	// ����Ȋ����ŁA�Ђ�����f�o�C�X�R���e�L�X�g�ɏ���n��
	// 
	// �e�N�X�`�����A�s�N�Z���V�F�[�_�[�̃X���b�g0�ɃZ�b�g
	//0�Ԗڂ���1�ݒ�Ƃ��������̗p��
	//���������ɂ��ł���H�݂��������ǂ����͕s��

	//�T���v���[�����ѕt�����ĂȂ��H�Ƃ����G���[���o�邪�����ɖ{���͏����Ȃ��Ƃ����Ȃ��Ƃ����������낤��
	//�e�N�Z��(�e�N�X�`����̃s�N�Z��)�Ɖ�ʏ�ł̃s�N�Z���̈ʒu������Ȃ��ꍇ�����R����A���̎��ɐF���ǂ����߂邩
	//���`�ł͎���̃e�N�Z���̐F������`��Ԃ���
	//�ٕ���(D3D11_FILTER_ANISOTROPIC)�ł͑�`���g��(�����琳�΂��Ă��Ȃ��ʂ̕`��ɋ����炵��)
	/*
	{
		ColorObject::Initialize();
		std::vector<DirectX::XMFLOAT4> colors;
		std::vector<DirectX::XMVECTOR> coefs;
		{
			DirectX::XMVECTOR relPos = { -0.1,-0.1,0,1 };
			DirectX::XMFLOAT4 Color = { 1,0,0,0 };
			colors.push_back(Color);
			coefs.push_back(relPos);
		}
		{
			DirectX::XMVECTOR relPos = { -0.1,0.1,0 ,1 };
			DirectX::XMFLOAT4 Color = { 0,1,0,0 };
			colors.push_back(Color);
			coefs.push_back(relPos);
		}
		{
			DirectX::XMVECTOR relPos = { 0.1,-0.1,0 ,1 };
			DirectX::XMFLOAT4 Color = { 0,0,1,0 };
			colors.push_back(Color);
			coefs.push_back(relPos);
		}
		{
			DirectX::XMVECTOR relPos = { 0.1,0.1,0 ,1 };
			DirectX::XMFLOAT4 Color = { 0,0,0,0 };
			colors.push_back(Color);
			coefs.push_back(relPos);
		}
		{
			DirectX::XMVECTOR relPos = { -0.1,-0.1,0 ,1 };
			DirectX::XMFLOAT4 Color = { 1,0,0,0 };
			colors.push_back(Color);
			coefs.push_back(relPos);
		}
		{
			DirectX::XMVECTOR relPos = { -0.1,0.1,0 ,1 };
			DirectX::XMFLOAT4 Color = { 0,1,0,0 };
			colors.push_back(Color);
			coefs.push_back(relPos);
		}
		PaperC.Colors.push_back(colors);
		PaperC.Coefs.push_back(coefs);
		PaperC.Parallel = { 0,0,0.5 };
	}
	*/
	{
		DirectX::XMFLOAT3 LEFTDOWN = DirectX::XMFLOAT3{ 0,0,0 };
		DirectX::XMFLOAT3 LEFTUP = DirectX::XMFLOAT3{ 0,1 ,0 };
		DirectX::XMFLOAT3 RIGHTDOWN = DirectX::XMFLOAT3{ 1,0 ,0 };
		DirectX::XMFLOAT3 RIGHTUP = DirectX::XMFLOAT3{ 1,1 ,0 };

		float innerWidth = std::tan(PI * 10 / 180) * 10;
		float outerWidth = std::tan(PI * 10 / 180) * 12;
		DirectX::XMMATRIX vRotate = DirectX::XMMatrixRotationY(PI * -10 / 180);
		DirectX::XMVECTOR leftInner{innerWidth * -1 ,2,0};
		DirectX::XMVECTOR leftOuter{ outerWidth * -1 ,2,2 };
		DirectX::XMVECTOR rightInner{ innerWidth ,2,0 };
		DirectX::XMVECTOR rightOuter{ outerWidth ,2,2 };
		DirectX::XMVECTOR virtical{ 0,-4,0 };

		//��]���Ĉړ�

		Root = Object(1, 6);
		DirectX::XMVECTOR pos[8];
		pos[0] = leftInner;
		pos[1] = rightInner;
		pos[2] = rightOuter;
		pos[3] = leftOuter;
		pos[4] = DirectX::XMVectorAdd(pos[0], virtical);
		pos[5] = DirectX::XMVectorAdd(pos[1], virtical);
		pos[6] = DirectX::XMVectorAdd(pos[2], virtical);
		pos[7] = DirectX::XMVectorAdd(pos[3], virtical);
		int UVrot[6] = { 0,0,0,0,0,0 };
		int UVindex[6] = { 0,1,2,3,4,5 };
		Root.AddHexahedron(pos, UVrot,UVindex,5);
		DirectX::XMVECTOR boxPosition{ 0,0,40,1 };
		DirectX::XMMATRIX rotate = DirectX::XMMatrixRotationY(PI*10/180);
		Position BoxCenter = Position({0,-15,120,1},0,0,0,36,0);
		for (int i = 0; i < 36; i++) {
			Position EachBox = Position(boxPosition, 0, i*PI*10/180, 0, 0, 6);
			//boxPosition = DirectX::XMVector4Transform(boxPosition, rotate);
			EachBox.Add(0, 6);
			BoxCenter.Add(EachBox);
		}
		Root.Add(BoxCenter);

		{
			Sphere = Object(1, 1);
			Sphere.AddShpere(25, 73, 72, 9);
			Position temp = Position({ 0,10,120,1 }, 0, 0, 0, 0, 1);
			temp.Add(0, 1);
			Sphere.Add(temp);
		}

		{
			Ground = Object(1, 1);
			Ground.AddRandomizedPlane({ -500,0,500,1 }, 100, 100, 1000, 1000, 3, 0.5, 9);
			Position temp = Position({ 0,0,0,1 }, 0, 0, 0, 0, 1);
			temp.Add(0, 1);
			Ground.Add(temp);
		}
		/*
		leftInner = DirectX::XMVector4Transform(leftInner, vRotate);
		leftOuter = DirectX::XMVector4Transform(leftOuter, vRotate);
		rightInner = DirectX::XMVector4Transform(rightInner, vRotate);
		rightOuter = DirectX::XMVector4Transform(rightOuter, vRotate);
		front = DirectX::XMVector4Transform(front, vRotate);
		rear = DirectX::XMVector4Transform(rear, vRotate);
		left = DirectX::XMVector4Transform(left, vRotate);
		right = DirectX::XMVector4Transform(right, vRotate);
		*/

	}
	/*
	{
		ConstantBufferData cbData
		{
			CAMERA.GetViewProjectionMatrix()
		};
		D3D11_BUFFER_DESC bufferDesc;
		bufferDesc.ByteWidth = sizeof(cbData);
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA subresourceData
		{
			&cbData,sizeof(cbData),0
		};
		if (FAILED(D3D.m_device->CreateBuffer(&bufferDesc, &subresourceData, &ConstantBuffer))) {
			return;
		}
		D3D.m_deviceContext->VSSetConstantBuffers(1,1,ConstantBuffer.GetAddressOf());
	}
	*/



	DP.GetInstance();
	DP.InitStates();
	DP.InitOther();
	rotation = DirectX::XMMatrixRotationY(3.141592 / 180);
}
//4�̓_���w��ł���Έ�ӂɑ��̓_���w��ł���͂��Ȃ̂ň��ɂȂ����_�Q�͂���4�_�Ƃ̊֌W�������ŋL�q���邱�Ƃ��ł��A4�_�̈ړ������ł��̓_�Q�̈ړ���\���ł���
//A,B,C,D��4�_�Ƃ�����A��B,A��C,A��D���Ɨ��ł���Ȃ炱���3�̌W���̂����œ_��\���ł���
//���_�Ő������Ĉړ�������̂��g��
//�g���C�A���O���X�g���b�v�ł���������̂�1�Ȃ���ŕ`��ł���킯�ł͂Ȃ����낤����
//���_�����߂���W��3�̃f�[�^�̃��X�g���������i�[�ł���悤�ɂ��Ă������ق����ǂ����낤
// ���̃Q�[���̎��Ԃ�i�߂�(���������s����)
void GameSystem::Execute()
{
	// ��ʂ�F�œh��Ԃ�
	float color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	D3D.m_deviceContext->ClearRenderTargetView(D3D.m_backBufferView.Get(), color);
	D3D.m_deviceContext->ClearDepthStencilView(D3D.m_depthStencilView.Get(),D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,1.0f,0.0f);
	/*
	// �O�p�`�̕`��
	{
		//�e�N�X�`�����ł̈ʒu�������K�v������


		// �O�p�`����邽�߁A���_���R���
		//�E�オ���̍��W�n
		//�E�B���h�E�̕���2�Œ��S��0
		//Pos��3�Ԗڂ�0�ȉ��ɂȂ�ƕ`�悳��Ȃ��Ȃ�
		//���_����Z�����������Ă���Ƃ������ƂȂ̂��ƍl������
		//�����1�𒴂��Ă��`�悳��Ȃ��Ȃ�
		//w���W�͎���̕��������Ă���炵��(�萔�����̏ꍇ������)
		//�Ȃ̂ő傫������Ƃ��̂�������������
		const int vCount = 360;
		VertexType v[vCount] = {
			{{-0.125f, -0.125f, 0.5}, {1,0}},
			{{-0.125f,  0.125f, 0.5}, {0,1}},
			{{ 0.125f, -0.125f, 0.5}, {0,0}},
			{{ 0.125f,  0.125f, 0.5}, {1,0}},
			{{ 0.375f, -0.125f, 0.5}, {0,1}},
			{{ 0.375f,  0.125f, 0.5}, {0,0}},
		};

		for (int i = 0; i < vCount; i++) {
			float x = std::cos(3.1415926535 * -2 * i / vCount) * (0.05 + 0.05 * (i % 2));
			float y = std::sin(3.1415926535 * -2 * i / vCount) * (0.05 + 0.05 * (i % 2));
			float U = (i % 2);
			float V = ((i - i % 2)/2)%2;
			v[i] = { {x,y,0.2},{U,V} };
		}
		//vbDesc��ByteWidth��ς���Β��_�o�b�t�@�̃T�C�Y�͕ς����g���f�[�^�̗ʂ͐����ł���

		//-----------------------------
		// ���_�o�b�t�@�쐬
		// �E��L�ō�����R�̒��_�͂��̂܂܂ł͕`��Ɏg�p�ł��Ȃ��񂷁c
		// �E�r�f�I���������Ɂu���_�o�b�t�@�v�Ƃ����`�ō��K�v������܂��I
		// �E����͌����������āA���̏�ō���āA�g���āA�����̂Ă܂��B
		//-----------------------------
		// �쐬����o�b�t�@�̎d�l�����߂�
		// �E����͒��_�o�b�t�@�ɂ��邼�I���Ċ����̐ݒ�
		D3D11_BUFFER_DESC vbDesc = {};
		vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;	// �f�o�C�X�Ƀo�C���h����Ƃ��̎��(���_�o�b�t�@�A�C���f�b�N�X�o�b�t�@�A�萔�o�b�t�@�Ȃ�)
		vbDesc.ByteWidth = sizeof(v);					// �쐬����o�b�t�@�̃o�C�g�T�C�Y
		//�N�����Ɋm�ۂ��Ďg���܂킷�ꍇ�����̃T�C�Y�����炩���ߌ��߂�K�v������A���ꂪ�`��\�ʂ̌��E�ƂȂ�
		vbDesc.MiscFlags = 0;							// ���̑��̃t���O
		vbDesc.StructureByteStride = 0;					// �\�����o�b�t�@�̏ꍇ�A���̍\���̂̃T�C�Y
		vbDesc.Usage = D3D11_USAGE_DYNAMIC;				// �쐬����o�b�t�@�̎g�p�@(DYNAMIC�ɂ��Ȃ��Ƃ��Ƃ��珑���������Ȃ�)
		vbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; //WRITE���w�肵�ĂȂ��Ə����������Ȃ�

		// ��̎d�l��n���Ē��_�o�b�t�@������Ă��炤
		// �������f�o�C�X����ɂ��肢����
		ComPtr<ID3D11Buffer> vb;
		D3D11_SUBRESOURCE_DATA initData = { &v[0], sizeof(v), 0 };	// �������ރf�[�^
		// ���_�o�b�t�@�̍쐬
		D3D.m_device->CreateBuffer(&vbDesc, &initData, &vb);
		//++++++++++++++++++++ ��������`�悵�Ă����܂� ++++++++++++++++++++

		//���������̎���
		for (int i = 0; i < vCount; i++) {
			float x = std::cos(3.1415926535 * -2 * i / vCount) * (0.02 + 0.08 * (i % 2));
			float y = std::sin(3.1415926535 * -2 * i / vCount) * (0.02 + 0.08 * (i % 2));
			float U = ((i - i % 2) / 2) % 2;
			float V = (i % 2);
			v[i] = { {x,y,0.2},{U,V} };
		}


		D3D11_MAPPED_SUBRESOURCE mappedSubresource;
		//������GPU���璸�_�o�b�t�@�ւ̃A�N�Z�X���~�߁ACPU����̃A�N�Z�X���ł���悤�ɂ��Ă���
		D3D.m_deviceContext->Map(vb.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
		//������CPU�ɂ�鏈���Ńo�b�t�@�̓��e��ς��Ă���
		memcpy(mappedSubresource.pData, v, sizeof(v));//��3�ϐ���ς��邱�Ƃŏ��������鐔��ς�����
		//������GPU�ɃA�N�Z�X�������Ă�
		D3D.m_deviceContext->Unmap(vb.Get(),0);

		//�o�b�t�@��2�����deviceContext->CopyResource()���g���ăf�[�^���ڂ����Ƃ��\

		// ��������́u�`��̐E�l�v�f�o�C�X�R���e�L�X�g���񂪑劈��

		// ���_�o�b�t�@��`��Ŏg����悤�ɃZ�b�g����
		UINT stride = sizeof(VertexType);
		UINT offset = 0;
		//���_�o�b�t�@�̂������Ԃ���(��1����)����(��2����)���g���̂�
		//vb�͔z��̐擪�̃A�h���X���w�����ƂɂȂ�
		//stride��offset���e���_�o�b�t�@���ƂɎw�肷�邱�ƂɂȂ�̂ł������z��
		D3D.m_deviceContext->IASetVertexBuffers(0, 1, vb.GetAddressOf(), &stride, &offset);
		// �v���~�e�B�u�E�g�|���W�[���Z�b�g
		//���_���W�̏������@
		//���v��肩�琔���n�߂�
		//���v�������v�����v�c
		//�ŐV�̒��_�Ƃ���1�O�ɍ�������_�ɖʂ���悤�ɐV�����O�p�`������Ă���
		// 7--6
		// | /|
		// |/ |
		// 5--4
		// | /|
		// |/ |
		// 3--2
		// | /|
		// |/ |
		// 1--0
		D3D.m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		// �C���f�b�N�X�o�b�t�@�Ƃ����̂��g���ƈ�̒��_���g���܂킵�ď������ł���炵���H
		
		
		//-----------------------------
		// �V�F�[�_�[���Z�b�g
		//-----------------------------
		D3D.m_deviceContext->VSSetShader(D3D.m_spriteVS.Get(), 0, 0);
		D3D.m_deviceContext->PSSetShader(D3D.m_spritePS.Get(), 0, 0);
		//�����͂悭�������
		D3D.m_deviceContext->IASetInputLayout(D3D.m_spriteInputLayout.Get());


		//-----------------------------
		// �`����s
		//-----------------------------
		// �f�o�C�X�R���e�L�X�g����A��L�̃Z�b�g�������e�ŕ`�悵�Ă��������A�Ƃ��肢����
		//vertexCount��ς��邱�Ƃł����炭���Z�ʂ𐧌��\

		//�O��֌W��Draw�̏����Ɉ˂炸�K�؂ɏ��������̂ŐS�z�͂���Ȃ�

		D3D.m_deviceContext->Draw(vCount, 0);
	}
	*/
	DP.cBuffer.Data.SunDirection = { 0,-1,0 ,0};
	Sphere.Draw();
	Root.Draw();
	Ground.Draw();
	//Root.Positions[0].rotationXaxis = Root.Positions[0].rotationXaxis + (3.1415926535 / 180.0);
	//CAMERA.rotationYaxis = CAMERA.rotationYaxis + (3.1415926535 / 270.0);
	// �o�b�N�o�b�t�@�̓��e����ʂɕ\��
	D3D.m_swapChain->Present(1, 0);
	timeCount++;
}