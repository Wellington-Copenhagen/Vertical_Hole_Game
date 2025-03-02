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
// �Q�[���̏����ݒ���s��
GameSystem::GameSystem() {
}
void GameSystem::Initialize(HWND hWnd)
{
	Tick = 0;
	Interface::UnitNameHash = std::map<std::string, Interface::UnitIndex>();
	Interface::EntNameHash = std::map<std::string, Interface::EntityPointer>();
	mBlockAppearances = Appearances
		<Interface::BlockDrawCallType, Interface::BlockInstanceType, 1, 256 * 256 * 2>(&mAllEntities.INtoIndex, 4);
	mBallAppearances[0] = Appearances
		<Interface::BallDrawCallType, Interface::BallInstanceType, 1, MaxBallCount>(&mAllEntities.INtoIndex, 4, 0.4, 0.4, 0.4, -0.01);
	mBallAppearances[1] = Appearances
		<Interface::BallDrawCallType, Interface::BallInstanceType, 1, MaxBallCount>(&mAllEntities.INtoIndex, 4,0.5, 0.5, 0.5,0);
	mBallAppearances[2] = Appearances
		<Interface::BallDrawCallType, Interface::BallInstanceType, 1, MaxBallCount>(&mAllEntities.INtoIndex, 4,0.2,0.2,0.2,-0.1);

	mBulletAppearances = Appearances
		<Interface::BulletDrawCallType, Interface::BulletInstanceType, 1, 256 * 256>(&mAllEntities.INtoIndex, 4);
	SameFormatTextureArray(256, 256, 4, 32);//��4�ϐ���ArraySize�Ȃ̂ŐL�΂��Ȃ��ƃI�[�o�[����댯������
	mBlockDrawCallBuffer = VertexBuffer<Interface::BlockDrawCallType, 4, 0>(mBlockAppearances.DrawCall);
	mBlockInstanceBuffer = VertexBuffer<Interface::BlockInstanceType, 256 * 256 * 2, 1>(mBlockAppearances.Instances);

	for (int i = 0; i < 3; i++) {
		mBallDrawCallBuffer[i] = VertexBuffer<Interface::BallDrawCallType, 4, 0>(mBallAppearances[i].DrawCall);
		mBallInstanceBuffer[i] = VertexBuffer<Interface::BallInstanceType, MaxBallCount, 1>(mBallAppearances[i].Instances);
	}

	mBulletDrawCallBuffer = VertexBuffer<Interface::BulletDrawCallType, 4, 0>(mBulletAppearances.DrawCall);
	mBulletInstanceBuffer = VertexBuffer<Interface::BulletInstanceType, 256 * 256, 1>(mBulletAppearances.Instances);
	mGraphicProcessSetter = GraphicProcessSetter(D3D.Width,D3D.Height);
	mCamera = Camera(D3D.Height, D3D.Width);

	mCBuffer = ConstantBuffer();
	mAllEntities = AllEntities();
	std::vector<std::string> EntityFileNames = {
		"Data/EntityData/Block.json",
		"Data/EntityData/Effect.json",
		"Data/EntityData/Ball.json"
	};
	mAllEntities.LoadEntities(EntityFileNames);
	std::vector<std::string> UnitFileNames = {
		"Data/unitData/unit.json",
	};
	mAllEntities.LoadUnits(UnitFileNames);

	std::string mapName = "Data/MissionData/Mission_Test.json";

	mAllEntities.LoadMission(mapName);
	mAllSystem = AllSystem(this);

	std::random_device RandSeedGen;
	Interface::RandEngine = std::mt19937(RandSeedGen());
	{
		Json::Value root;
		std::ifstream file = std::ifstream(mapName);
		file >> root;
		file.close();
		LoadMap(root.get("mapFilePath","").asString());
	}

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
	pBlockAppearance = &pGameSystem->mBlockAppearances;
	pBallAppearance = pGameSystem->mBallAppearances;
	pBulletAppearance = &pGameSystem->mBulletAppearances;
	pAllEntities = &pGameSystem->mAllEntities;
	TargetArchetypes = std::vector<Interface::ArchetypeIndex>();
	RequireComponents = Interface::RawArchetype();
	RequireComponents.reset();
}
// ���̃Q�[���̎��Ԃ�i�߂�(���������s����)

//�f�o�b�O���ɒ��ӁI
//1�e�B�b�N�ڂɂ͗L���ȃG���e�B�e�B����0�Ȃ̂ŉ����\������Ȃ��Đ�����
void GameSystem::Execute(HWND hWnd)
{
	try {
		ApplyInput();
		mAllSystem.Update();
		float color[4] = { 0.8f, 0.0f, 0.0f, 1.0f };
		D3D.m_deviceContext->ClearRenderTargetView(D3D.m_backBufferView.Get(), color);
		D3D.m_deviceContext->ClearDepthStencilView(D3D.m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0.0f);
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
		// �J�����E�萔�o�b�t�@
		mCamera.Update();
		DirectX::XMStoreFloat4x4(&mCBuffer.Data.ViewProjection, mCamera.CameraMatrix);
		mCBuffer.UpdateAndSet();
		// �e�N�X�`��
		mSameFormatTextureArray.SetToGraphicPipeLine();
		//IASetVertexBuffer�̏������e������݂���
		// ���ۂ̕`��

		mBlockInstanceBuffer.UpdateAndSet(mBlockAppearances.Instances, 0, mBlockAppearances.InstanceCount);
		mBlockDrawCallBuffer.UpdateAndSet(mBlockAppearances.DrawCall, 0, 4);
		mGraphicProcessSetter.SetAsBlock();
		D3D.m_deviceContext->DrawInstanced(4, mBlockAppearances.InstanceCount, 0, 0);

		for (int i = 0; i < 3; i++) {
			mBallDrawCallBuffer[i].UpdateAndSet(mBallAppearances[i].DrawCall, 0, 4);
			mBallInstanceBuffer[i].UpdateAndSet(mBallAppearances[i].Instances, 0, mBallAppearances[i].InstanceCount);
			mGraphicProcessSetter.SetAsBall();
			D3D.m_deviceContext->DrawInstanced(4, mBallAppearances[i].InstanceCount, 0, 0);
		}


		mBulletDrawCallBuffer.UpdateAndSet(mBulletAppearances.DrawCall, 0, 4);
		mBulletInstanceBuffer.UpdateAndSet(mBulletAppearances.Instances, 0, mBulletAppearances.InstanceCount);
		mGraphicProcessSetter.SetAsBullet();
		D3D.m_deviceContext->DrawInstanced(4, mBulletAppearances.InstanceCount, 0, 0);


		D3D.m_swapChain->Present(1, 0);
		Tick++;
	}
	catch (char* e) {
		OutputDebugStringA(e);
	}
}
//�n�ʂƕǂ͕ʃI�u�W�F�N�g�Ƃ���
void GameSystem::LoadMap(std::string mapFileName)
{
	//�}�b�v�f�[�^�̓ǂݍ���
	Json::Value root;
	std::ifstream file = std::ifstream(mapFileName);
	file >> root;
	file.close();
	//�}�b�v�T�C�Y�̓ǂݍ���
	int width = root.get("width", "").asInt();
	int height = root.get("height", "").asInt();
	if (width > 256 || height > 256) {
		throw("�}�b�v�̃T�C�Y���傫�����܂�");
	}
	mHurtboxes = Hurtboxes(&mAllEntities, width, height);
	Json::Value areaList = root.get("areaList", "");
	//���̏ꏊ�ɕǂ����邩
	std::bitset<256 * 256> isWallMap;
	//���̏ꏊ�̕ǂ����łɔz�u������
	std::bitset<256 * 256> wallPlaced;
	//���̏ꏊ�̏������łɔz�u������
	std::bitset<256 * 256> floorPlaced;
	isWallMap.reset();
	wallPlaced.reset();
	floorPlaced.reset();
	//�ǂ�����ꏊ���L�^����
	for (int i = 0; i < areaList.size(); i++) {
		int top = areaList[i].get("top", "").asInt();
		int bottom = areaList[i].get("bottom", "").asInt();
		int left = areaList[i].get("left", "").asInt();
		int right = areaList[i].get("right", "").asInt();
		if (areaList[i].get("isWall", "").asBool()) {
			for (int x = left; x < right; x++) {
				for (int y = bottom; y < top; y++) {
					isWallMap[y * 256 + x] = true;
				}
			}
		}
	}
	//���ۂɃu���b�N�̃f�[�^���G���e�B�e�B���X�g�ɋL�^����
	for (int i = 0; i < areaList.size(); i++) {
		int top = areaList[i].get("top", "").asInt();
		int bottom = areaList[i].get("bottom", "").asInt();
		int left = areaList[i].get("left", "").asInt();
		int right = areaList[i].get("right", "").asInt();
		Interface::EntityPointer blockPointer = Interface::EntNameHash[(std::string)areaList[i].get("name", "").asString()];
		if (areaList[i].get("isWall", "").asBool()) {
			for (int x = left; x < right; x++) {
				for (int y = bottom; y < top; y++) {
					if (!wallPlaced[256 * y + x]) {
						Interface::EntityInitData init;
						wallPlaced[256 * y + x] = true;
						init.Pos.Identity();
						init.Pos.Parallel = { (float)x,(float)y,0.0f,1.0f };
						init.initialMaskIndex = 0;
						init.Prototype = blockPointer;
						if (x != 0 && !isWallMap[y * 256 + x - 1]) {
							init.initialMaskIndex += 1;
						}
						if (x != width - 1 && !isWallMap[y * 256 + x + 1]) {
							init.initialMaskIndex += 2;
						}
						if (y != 0 && !isWallMap[(y - 1) * 256 + x]) {
							init.initialMaskIndex += 4;
						}
						mAllEntities.AddFromEntPointer(&init);
					}
				}
			}
		}
		else {
			for (int x = left; x < right; x++) {
				for (int y = bottom; y < top; y++) {
					if (!floorPlaced[256 * y + x]) {
						Interface::EntityInitData init;
						floorPlaced[256 * y + x] = true;
						init.Pos.Identity();
						init.Pos.Parallel = { (float)x,(float)y,0.0f,1.0f };
						init.initialMaskIndex = 0;
						init.Prototype = blockPointer;
						if (x != 0 && isWallMap[y * 256 + x - 1]) {
							init.initialMaskIndex += 1;
						}
						if (x != width - 1 && isWallMap[y * 256 + x + 1]) {
							init.initialMaskIndex += 2;
						}
						if (y != 0 && isWallMap[(y - 1) * 256 + x]) {
							init.initialMaskIndex += 4;
						}
						if (y != height - 1 && isWallMap[(y + 1) * 256 + x]) {
							init.initialMaskIndex += 8;
						}
						mAllEntities.AddFromEntPointer(&init);
					}
				}
			}
		}
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