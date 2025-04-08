// GameSystem�N���X���g����悤�ɂ���
#include "Game.h"
// Direct3D�N���X���g����悤�ɂ���
//#include "Source/DirectX/Direct3D.h"


//�n�`�͂���ɂ�������3D���f���͍��Ȃ��̂Ř�p�𐧌�����
//�����̃A�N�V�����Q�[�ł��������ł���ΐ헪�A�N�V�����ɂ�����
//�ݒ�����₷�����ȈӖ��ł̓_���W����
//

// ���̕ӂ̊Ď����ɗ�entity�ōs������
// ���܂�^�C�~���O
// �J���ɂ��w��
// �EEntity.json��Unit.json�Ɏw��
// �E�����̐���
// �~�b�V�����ɂ��w��
// �EMission.json�Ɏw��
// �E�U���A�ϋv�Ȃǂ̔{��
// �E�o���ʒu�A�o���^�C�~���O
// �v���C���[�ɂ��J�X�^�}�C�Y
// �EUnit.json�ɂ��w��
// �E�F�ABall�̑g�ݕ�
// 

DirectX::XMMATRIX rotation;
// ������̓f�o�b�O�p�Ƃ������Ƃ�
GraphicalStringDraw globalStringDraw;
LineDraw globalLineDraw;
int Tick;
// �Q�[���̏����ݒ���s��
GameSystem::GameSystem() {
}
void GameSystem::Initialize(HWND hWnd)
{
	//SameFormatTextureArray::SaveShadowTexture(64, "texWallShadow.tif", 0, 16, 8, 8);
	//SameFormatTextureArray::SaveShadowTexture(64, "texFloorShadow.tif", 8, 8, 8, 8);
	//SameFormatTextureArray::SaveWallMask(64, "texWallShadow.tif");
	SameFormatTextureArray::SaveLensMask(64, "texLensMask.tif");













	GraphicProcessSetter(D3D.Width, D3D.Height);
	Tick = 0;
	Interface::UnitNameHash = std::map<std::string, Interface::UnitIndex>();
	Interface::EntNameHash = std::map<std::string, entt::entity>();
	mHurtboxes = Hurtboxes(&mEntities.Registry);

	// �e�N�X�`���̏�����
	BlockTextureArray = SameFormatTextureArray(1024, true, 64);
	BallTextureArray = SameFormatTextureArray(1024, true, 64);
	BulletTextureArray = SameFormatTextureArray(1024, true, 64);
	EffectTextureArray = SameFormatTextureArray(1024, true, 64);

	// ���_�o�b�t�@�A���_�f�[�^�̏�����
	mFloorAppearances = EntityBindAppearances
		<Interface::BlockDrawCallType, Interface::BlockInstanceType,Component::BlockAppearance, 1>(WorldWidth * WorldHeight);
	Interface::InitDrawCallUV(mFloorAppearances.DrawCall, 0);
	Interface::InitDrawCallPos(mFloorAppearances.DrawCall, 0);
	mWallAppearances = EntityBindAppearances
		<Interface::BlockDrawCallType, Interface::BlockInstanceType, Component::BlockAppearance, 1>(WorldWidth * WorldHeight);
	Interface::InitDrawCallUV(mWallAppearances.DrawCall, 0);
	Interface::InitDrawCallPos(mWallAppearances.DrawCall, 0);
	mFloorDrawCallBuffer = VertexBuffer<Interface::BlockDrawCallType>(mFloorAppearances.GetDrawCallPointer(0), 4, 0);
	mFloorInstanceBuffer = VertexBuffer<Interface::BlockInstanceType>(mFloorAppearances.GetInstancePointer(0), WorldWidth * WorldHeight, 1);
	mWallDrawCallBuffer = VertexBuffer<Interface::BlockDrawCallType>(mWallAppearances.GetDrawCallPointer(0), 4, 0);
	mWallInstanceBuffer = VertexBuffer<Interface::BlockInstanceType>(mWallAppearances.GetInstancePointer(0), WorldWidth * WorldHeight, 1);

	mBallAppearances = EntityBindAppearances
		<Interface::BallDrawCallType, Interface::BallInstanceType,Component::BallAppearance, 3>(MaxBallCount);
	Interface::InitDrawCallUV(mBallAppearances.DrawCall, 0);
	Interface::InitDrawCallPos(mBallAppearances.DrawCall, 0, 0.4, 0.4, 0.4, -0.01);
	Interface::InitDrawCallUV(mBallAppearances.DrawCall, 1);
	Interface::InitDrawCallPos(mBallAppearances.DrawCall, 1, 0.5, 0.5, 0.5, 0);
	Interface::InitDrawCallUV(mBallAppearances.DrawCall, 2);
	Interface::InitDrawCallPos(mBallAppearances.DrawCall, 2, 0.2, 0.2, 0.2, -0.1);
	for (int i = 0; i < 3; i++) {
		mBallDrawCallBuffer[i] = VertexBuffer<Interface::BallDrawCallType>(mBallAppearances.GetDrawCallPointer(i), 4, 0);
		mBallInstanceBuffer[i] = VertexBuffer<Interface::BallInstanceType>(mBallAppearances.GetInstancePointer(i), MaxBallCount, 1);
	}

	mBulletAppearances = EntityBindAppearances
		<Interface::BulletDrawCallType, Interface::BulletInstanceType, Component::BulletAppearance, 1>(MaxBulletCount);
	Interface::InitDrawCallUV(mBulletAppearances.DrawCall, 0);
	Interface::InitDrawCallPos(mBulletAppearances.DrawCall, 0);
	mBulletDrawCallBuffer = VertexBuffer<Interface::BulletDrawCallType>(mBulletAppearances.GetDrawCallPointer(0), 4, 0);
	mBulletInstanceBuffer = VertexBuffer<Interface::BulletInstanceType>(mBulletAppearances.GetInstancePointer(0), MaxBulletCount, 1);

	mEffectAppearances = EntityBindAppearances
		<Interface::EffectDrawCallType, Interface::EffectInstanceType, Component::EffectAppearance, 1>(MaxEffectCount);
	Interface::InitDrawCallUV(mEffectAppearances.DrawCall, 0);
	Interface::InitDrawCallPos(mEffectAppearances.DrawCall, 0);
	mEffectDrawCallBuffer = VertexBuffer<Interface::EffectDrawCallType>(mEffectAppearances.GetDrawCallPointer(0), 4, 0);
	mEffectInstanceBuffer = VertexBuffer<Interface::EffectInstanceType>(mEffectAppearances.GetInstancePointer(0), MaxEffectCount, 1);
	//mLineAppearances = Appearances
		//<Interface::LineDrawCallType, Interface::LineInstanceType, 1, MaxLineCount>(4);
	//mLineDrawCallBuffer = VertexBuffer<Interface::LineDrawCallType, 4, 0>(mLineAppearances.DrawCall);
	//mLineInstanceBuffer = VertexBuffer<Interface::LineInstanceType, MaxLineCount, 1>(mLineAppearances.Instances);











	int w = D3D.Width;
	int h = D3D.Height;
	mCamera = Camera();

	mCBuffer = ConstantBuffer();


	HFONT hFont = CreateFontW(28,//�����̍���
		0,//�����̕��A0���ƍ����ɍ��킹�Ē��������H
		0,//���̌X���H
		0,//�����̌X���H
		1000,//�����@�傫���قǑ���
		FALSE,// �Α�
		FALSE,// ����
		FALSE,// ��������
		SHIFTJIS_CHARSET,//�����Z�b�g SHIFT_JIS������
		OUT_TT_ONLY_PRECIS,//���x�@�悭�킩���
		CLIP_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY,//�i�� �Ƃ肠�����A���`�G�C���A�X�͎g��
		DEFAULT_PITCH || FF_MODERN,//�����ڂ̃O���[�v�H
		L"�l�r ����"//�t�H���g�̖��O
	);
	StringDrawTest = GraphicalStringDraw(hFont, 65536, 2048, 32);
	globalStringDraw = GraphicalStringDraw(hFont, 65536, 2048, 32);
	for (int x = 0; x < 256; x = x + 8) {
		for (int y = 0; y < 256; y = y + 8) {
			Interface::VisibleStringInfo toAppend = Interface::VisibleStringInfo();
			std::string content = "(" + std::to_string(x) + "," + std::to_string(y) + ")";
			DirectX::XMVECTOR pos = { (float)x,(float)y,0,1 };
			DirectX::XMVECTOR color = { 0,0,0,1 };
			StringDrawTest.Append(content, &color, &pos, 0.5f, 60000, StrDrawPos::AsCenter);
		}
	}
	globalLineDraw = LineDraw(65536, 256);

	mUserInterface = UserInterface(&mEntities, &mCBuffer);

	std::vector<std::string> EntityFileNames = {
		"Data/EntityData/Block.json",
		"Data/EntityData/Effect.json",
		"Data/EntityData/Bullet.json",
		"Data/EntityData/Ball.json"
	};
	mEntities.LoadEntities(EntityFileNames);
	std::vector<std::string> UnitFileNames = {
		"Data/unitData/unit.json",
	};
	mEntities.LoadUnits(UnitFileNames);

	std::string missionName = "Data/MissionData/Mission_Test.json";

	mEntities.LoadMission(missionName);
	mEntities.LoadPlayerCorps("Data/SaveData/PlayerCorps.json");
	mAllSystem = AllSystem(this);

	std::random_device RandSeedGen;
	Interface::RandEngine = std::mt19937(RandSeedGen());

	RefreshedTimeStamp = std::chrono::system_clock::now();
	/*
	Texture Text = Texture("Data/Appearance/Test.tif");
	Texture Block = Texture("Data/Appearance/Block.tif");
	D3D11_SHADER_RESOURCE_VIEW_DESC desc;
	desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	desc.Texture2DArray.ArraySize = 2;
	desc.Texture2DArray.MipLevels = -1;
	desc.Texture2DArray.MostDetailedMip = 7;
	desc.Texture2DArray.FirstArraySlice = 0;
	D3D.m_device->CreateShaderResourceView(, &desc, mTextureArray.m_srv.GetAddressOf());
	// ���\�[�X�ƃV�F�[�_�[���\�[�X�r���[���쐬
	if (FAILED(DirectX::CreateShaderResourceView(D3D.m_device.Get(), image->GetImages(), image->GetImageCount(), m_info, &m_srv)))
	{
		// ���s
		m_info = {};
		return;
	}
	mTextureArray = TextureArray("Data/AppearanceData/Test.tif");
	D3D.m_deviceContext->CopySubresourceRegion(mTextureArray.m_srv.Get(), 0, 0, 0, 0, Text.m_srv.Get(), 0, nullptr);

	*/

	// MessageBoxA(hWnd,"�e�X�g�p","�����炭��̂�ł���",MB_OK);
	// Win 32 API��GUI�n���Ăׂ�̂ŃQ�[�����Ŏg���Ă��������f�o�b�O�Ɏg���̂��悢���낤
	
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
}
//4�̓_���w��ł���Έ�ӂɑ��̓_���w��ł���͂��Ȃ̂ň��ɂȂ����_�Q�͂���4�_�Ƃ̊֌W�������ŋL�q���邱�Ƃ��ł��A4�_�̈ړ������ł��̓_�Q�̈ړ���\���ł���
//A,B,C,D��4�_�Ƃ�����A��B,A��C,A��D���Ɨ��ł���Ȃ炱���3�̌W���̂����œ_��\���ł���
//���_�Ő������Ĉړ�������̂��g��
//�g���C�A���O���X�g���b�v�ł���������̂�1�Ȃ���ŕ`��ł���킯�ł͂Ȃ����낤����
//���_�����߂���W��3�̃f�[�^�̃��X�g���������i�[�ł���悤�ɂ��Ă������ق����ǂ����낤



System::System(GameSystem* pGameSystem) {
	pHurtboxes = &pGameSystem->mHurtboxes;
	pFloorAppearance = &pGameSystem->mFloorAppearances;
	pWallAppearance = &pGameSystem->mWallAppearances;
	pBallAppearance = &pGameSystem->mBallAppearances;
	pBulletAppearance = &pGameSystem->mBulletAppearances;
	pEntities = &pGameSystem->mEntities;
}
// ���̃Q�[���̎��Ԃ�i�߂�(���������s����)

//�f�o�b�O���ɒ��ӁI
//1�e�B�b�N�ڂɂ͗L���ȃG���e�B�e�B����0�Ȃ̂ŉ����\������Ȃ��Đ�����
void GameSystem::Execute(HWND hWnd)
{
	try {
		ApplyInput();
		mAllSystem.Update();
		float color[4] = { 0.9f, 0.9f, 0.9f, 1.0f };
		D3D.m_deviceContext->ClearRenderTargetView(D3D.m_backBufferView.Get(), color);
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

		mFloorAppearances.EraseDeletedEntities(&mEntities.Registry);
		mWallAppearances.EraseDeletedEntities(&mEntities.Registry);
		mBallAppearances.EraseDeletedEntities(&mEntities.Registry);
		mBulletAppearances.EraseDeletedEntities(&mEntities.Registry);
		mEffectAppearances.EraseDeletedEntities(&mEntities.Registry);


		// �J�����E�萔�o�b�t�@
		mCamera.Update();
		DirectX::XMStoreFloat4x4(&mCBuffer.Data.ViewProjection, mCamera.CameraMatrix);
		mCBuffer.UpdateAndSet();
		// �e�N�X�`��
		BlockTextureArray.SetToGraphicPipeLine();
		//IASetVertexBuffer�̏������e������݂���
		// ���ۂ̕`��
		GraphicProcessSetter::SetAsBlock();
		mFloorInstanceBuffer.UpdateAndSet(mFloorAppearances.GetInstancePointer(0), 0, mFloorAppearances.InstanceCount);
		mFloorDrawCallBuffer.UpdateAndSet(mFloorAppearances.GetDrawCallPointer(0), 0, 4);
		D3D.m_deviceContext->DrawInstanced(4, mFloorAppearances.InstanceCount, 0, 0);
		mWallInstanceBuffer.UpdateAndSet(mWallAppearances.GetInstancePointer(0), 0, mWallAppearances.InstanceCount);
		mWallDrawCallBuffer.UpdateAndSet(mWallAppearances.GetDrawCallPointer(0), 0, 4);
		D3D.m_deviceContext->DrawInstanced(4, mWallAppearances.InstanceCount, 0, 0);


		BallTextureArray.SetToGraphicPipeLine();
		for (int i = 0; i < 3; i++) {
			mBallDrawCallBuffer[i].UpdateAndSet(mBallAppearances.GetDrawCallPointer(i), 0, 4);
			mBallInstanceBuffer[i].UpdateAndSet(mBallAppearances.GetInstancePointer(i), 0, mBallAppearances.InstanceCount);
			GraphicProcessSetter::SetAsBall();
			D3D.m_deviceContext->DrawInstanced(4, mBallAppearances.InstanceCount, 0, 0);
		}


		BulletTextureArray.SetToGraphicPipeLine();
		mBulletDrawCallBuffer.UpdateAndSet(mBulletAppearances.GetDrawCallPointer(0), 0, 4);
		mBulletInstanceBuffer.UpdateAndSet(mBulletAppearances.GetInstancePointer(0), 0, mBulletAppearances.InstanceCount);
		GraphicProcessSetter::SetAsBullet();
		D3D.m_deviceContext->DrawInstanced(4, mBulletAppearances.InstanceCount, 0, 0);

		for (int i = 0; i < mBulletAppearances.InstanceCount; i++) {
			DirectX::XMMATRIX pos = DirectX::XMLoadFloat4x4(&mBulletAppearances.Instances[i].World);
			globalStringDraw.SimpleAppend(std::to_string(i), 1, 0, 0, pos.r[3], 0.5, 1, StrDrawPos::AsCenter);
		}

		EffectTextureArray.SetToGraphicPipeLine();
		mEffectDrawCallBuffer.UpdateAndSet(mEffectAppearances.GetDrawCallPointer(0), 0, 4);
		mEffectInstanceBuffer.UpdateAndSet(mEffectAppearances.GetInstancePointer(0), 0, mEffectAppearances.InstanceCount);
		GraphicProcessSetter::SetAsEffect();
		D3D.m_deviceContext->DrawInstanced(4, mEffectAppearances.InstanceCount, 0, 0);


		StringDrawTest.Update();
		StringDrawTest.Draw();
		globalStringDraw.Update();
		globalStringDraw.Draw();
		globalLineDraw.Update();
		globalLineDraw.Draw();

		// �����Œ萔�o�b�t�@�������������Ă���̂Œ���
		mUserInterface.UpdateWhileMission();

		std::chrono::system_clock::time_point wakeUpTime = RefreshedTimeStamp + std::chrono::milliseconds(17);
		std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
		long waitMilliSeconds = (wakeUpTime - now).count()/10000;
		if (waitMilliSeconds > 0) {
			//Sleep(waitMilliSeconds);
		}
		RefreshedTimeStamp = std::chrono::system_clock::now();
		D3D.m_swapChain->Present(1, 0);
		OutputDebugStringA(("Tick:" + std::to_string(Tick) + "\n").c_str());
		Tick++;
	}
	catch (char* e) {
		OutputDebugStringA(e);
	}
}
void GameSystem::ApplyInput() {
	if (Input::KeyPushed.test(VK_UP)) {
		Camera::MoveCamera({ 0.0f, 0.5f, 0.0f ,0.0f});
	}
	if (Input::KeyPushed.test(VK_DOWN)) {
		Camera::MoveCamera({ 0.0f, -0.5f, 0.0f ,0.0f });
	}
	if (Input::KeyPushed.test(VK_LEFT)) {
		Camera::MoveCamera({ -0.5f, 0.0f, 0.0f ,0.0f });
	}
	if (Input::KeyPushed.test(VK_RIGHT)) {
		Camera::MoveCamera({ 0.5f, 0.0f, 0.0f ,0.0f });
	}
	Camera::ZoomRate = Camera::ZoomRate * (1.0f + (float)Input::WheelRotate / 1200.0);

	Input::Reset();
}