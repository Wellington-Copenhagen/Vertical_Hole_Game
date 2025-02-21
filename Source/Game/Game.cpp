// GameSystemクラスを使えるようにする
#include "Game.h"
// Direct3Dクラスを使えるようにする
//#include "Source/DirectX/Direct3D.h"


//地形はありにしたいが3Dモデルは作れないので俯角を制限して
//ただのアクションゲーでもいいができれば戦略アクションにしたい
//設定を作りやすそうな意味ではダンジョン
//

DirectX::XMMATRIX rotation;
// ゲームの初期設定を行う
GameSystem::GameSystem() {
}
void GameSystem::Initialize(HWND hWnd)
{
	Interface::EntNameHash = std::map<std::string, Interface::EntityPointer>();
	Tick = 0;
	mBlockAppearances = Appearances
		<Interface::BlockDrawCallType, Interface::BlockInstanceType, 1, 256 * 256 * 2>(&mAllEntities.INtoIndex, 4);
	mCharacterAppearances[0] = Appearances
		<Interface::CharacterDrawCallType, Interface::CharacterInstanceType, 1, 1024>(&mAllEntities.INtoIndex, 4, 0.8, 0.8, 0.8, -0.3);
	mCharacterAppearances[1] = Appearances
		<Interface::CharacterDrawCallType, Interface::CharacterInstanceType, 1, 1024>(&mAllEntities.INtoIndex, 4);
	mCharacterAppearances[2] = Appearances
		<Interface::CharacterDrawCallType, Interface::CharacterInstanceType, 1, 1024>(&mAllEntities.INtoIndex, 4,0.3,0.3,0.3,0.2);


	mCharacterAppearances[0].DrawCall[0].Pos = {
		-0.4f,-0.6f,0.0f,1.0f
	};
	mCharacterAppearances[0].DrawCall[1].Pos = {
		-0.4f,0.2f,0.0f,1.0f
	};
	mCharacterAppearances[0].DrawCall[2].Pos = {
		0.4f,-0.6f,0.0f,1.0f
	};
	mCharacterAppearances[0].DrawCall[3].Pos = {
		0.4f,0.2f,0.0f,1.0f
	};


	mCharacterAppearances[2].DrawCall[0].Pos = {
		-0.25f,-0.25f,0.0f,1.0f
	};
	mCharacterAppearances[2].DrawCall[1].Pos = {
		-0.25f,0.25f,0.0f,1.0f
	};
	mCharacterAppearances[2].DrawCall[2].Pos = {
		0.25f,-0.25f,0.0f,1.0f
	};
	mCharacterAppearances[2].DrawCall[3].Pos = {
		0.25f,0.25f,0.0f,1.0f
	};

	mBulletAppearances = Appearances
		<Interface::BulletDrawCallType, Interface::BulletInstanceType, 1, 256 * 256>(&mAllEntities.INtoIndex, 4);
	SameFormatTextureArray(256, 256, 4, 32);//大4変数はArraySizeなので伸ばさないとオーバーする危険がある
	mBlockDrawCallBuffer = VertexBuffer<Interface::BlockDrawCallType, 4, 0>(mBlockAppearances.DrawCall);
	mBlockInstanceBuffer = VertexBuffer<Interface::BlockInstanceType, 256 * 256 * 2, 1>(mBlockAppearances.Instances);

	for (int i = 0; i < 3; i++) {
		mCharacterDrawCallBuffer[i] = VertexBuffer<Interface::CharacterDrawCallType, 4, 0>(mCharacterAppearances[i].DrawCall);
		mCharacterInstanceBuffer[i] = VertexBuffer<Interface::CharacterInstanceType, 1024, 1>(mCharacterAppearances[i].Instances);
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
		"Data/EntityData/Character.json"
	};
	Interface::EntNameHash = mAllEntities.LoadFromFile(EntityFileNames);

	int a = Interface::EntNameHash.size();
	mAllSystem = AllSystem(this);

	std::random_device RandSeedGen;
	RandEngine = std::mt19937(RandSeedGen());
	LoadMap("Data/MapData/TestMap.json");
	{
		Interface::RelationOfCoord initWorld;
		initWorld.Parallel = { 3.0f,3.0f,0.0f,1.0f };
		initWorld.Ratio = 1;
		initWorld.Rotate = 0;
		Interface::CombatUnitInitData initCombatUnit;
		initCombatUnit.CoreId = 0;
		initCombatUnit.Team = 0;
		Interface::PlayingCharacter = mAllEntities.AddFromEntPointer({ 3,0 }, &initCombatUnit, &initWorld, 1, true, -1);
	}
	{
		Interface::RelationOfCoord initWorld;
		initWorld.Parallel = { 1.0f,0.0f,0.0f,1.0f };
		initWorld.Ratio = 1;
		initWorld.Rotate = 0;
		Interface::CombatUnitInitData initCombatUnit;
		initCombatUnit.CoreId = Interface::PlayingCharacter;
		initCombatUnit.Team = 0;
		mAllEntities.AddFromEntPointer({ 3,1 }, &initCombatUnit, &initWorld, 1, false, Interface::PlayingCharacter);
	}

	Interface::EntId AnotherOne;
	{
		Interface::RelationOfCoord initWorld;
		initWorld.Parallel = { 8.0f,3.0f,0.0f,1.0f };
		initWorld.Ratio = 1;
		initWorld.Rotate = 0;
		Interface::CombatUnitInitData initCombatUnit;
		initCombatUnit.CoreId = 0;
		initCombatUnit.Team = 0;
		AnotherOne = mAllEntities.AddFromEntPointer({ 3,0 }, &initCombatUnit, &initWorld, 1, true, -1);
	}
	{
		Interface::RelationOfCoord initWorld;
		initWorld.Parallel = { 1.0f,0.0f,0.0f,1.0f };
		initWorld.Ratio = 1;
		initWorld.Rotate = 0;
		Interface::CombatUnitInitData initCombatUnit;
		initCombatUnit.CoreId = Interface::PlayingCharacter;
		initCombatUnit.Team = 0;
		mAllEntities.AddFromEntPointer({ 3,1 }, &initCombatUnit, &initWorld, 1, false, AnotherOne);
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
	// リソースとシェーダーリソースビューを作成
	if (FAILED(DirectX::CreateShaderResourceView(D3D.m_device.Get(), image->GetImages(), image->GetImageCount(), m_info, &m_srv)))
	{
		// 失敗
		m_info = {};
		return;
	}
	mTextureArray = TextureArray("Data/AppearanceData/Test.tif");
	D3D.m_deviceContext->CopySubresourceRegion(mTextureArray.m_srv.Get(), 0, 0, 0, 0, Text.m_srv.Get(), 0, nullptr);

	*/

	// MessageBoxA(hWnd,"テスト用","おそらく上のやつでしょ",MB_OK);
	// Win 32 APIのGUI系も呼べるのでゲーム内で使ってもいいしデバッグに使うのもよいだろう
	
	//_tex.Load("Data/ColorTable.png");
	//D3D.m_deviceContext->PSSetShaderResources(0, 1, m_tex.m_srv.GetAddressOf());
	//透過は処理されない(0,0,0,0)と認識されて黒くなる

	// テクスチャやサンプラーの渡しは一度で良い(ただしスロットにおそらく限りがあるのでそれ以上に使いたい場合は途中で入れ替える必要があるのだと思う)
	
	// こんな感じで、ひたすらデバイスコンテキストに情報を渡す
	// 
	// テクスチャを、ピクセルシェーダーのスロット0にセット
	//0番目から1つ設定という感じの用だ
	//複数同時にもできる？みたいだけどやり方は不明

	//サンプラーが結び付けられてない？というエラーが出るがここに本当は書かないといけないという感じだろうか
	//テクセル(テクスチャ上のピクセル)と画面上でのピクセルの位置が合わない場合が当然あり、その時に色をどう決めるか
	//線形では周りのテクセルの色から線形補間する
	//異方性(D3D11_FILTER_ANISOTROPIC)では台形を使う(だから正対していない面の描画に強いらしい)
}
//4つの点を指定できれば一意に他の点を指定できるはずなので一塊になった点群はその4点との関係性だけで記述することができ、4点の移動だけでその点群の移動を表現できる
//A,B,C,Dの4点としたらA→B,A→C,A→Dが独立であるならこれら3つの係数のだけで点を表現できる
//原点で生成して移動させるのが吉か
//トライアングルストリップでもあらゆるものが1つながりで描画できるわけではないだろうから
//頂点を求められる係数3つのデータのリストをいくつか格納できるようにしておいたほうが良いだろう



System::System(GameSystem* pGameSystem) {
	pHurtboxes = &pGameSystem->mHurtboxes;
	pBlockAppearance = &pGameSystem->mBlockAppearances;
	pCharacterAppearance = pGameSystem->mCharacterAppearances;
	pBulletAppearance = &pGameSystem->mBulletAppearances;
	pAllEntities = &pGameSystem->mAllEntities;
	TargetArchetypes = std::vector<Interface::ArchetypeIndex>();
	RequireComponents = Interface::RawArchetype();
	RequireComponents.reset();
}
// このゲームの時間を進める(処理を実行する)

//デバッグ時に注意！
//1ティック目には有効なエンティティ数が0なので何も表示されなくて正しい
void GameSystem::Execute(HWND hWnd)
{
	try {
		ApplyInput();
		mAllSystem.Update();
		float color[4] = { 0.8f, 0.8f, 0.8f, 1.0f };
		D3D.m_deviceContext->ClearRenderTargetView(D3D.m_backBufferView.Get(), color);
		D3D.m_deviceContext->ClearDepthStencilView(D3D.m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0.0f);
		/*
		// 三角形の描画
		{
			//テクスチャ内での位置を示す必要がある


			// 三角形を作るため、頂点を３つ作る
			//右上が正の座標系
			//ウィンドウの幅が2で中心が0
			//Posの3番目が0以下になると描画されなくなる
			//原点からZ正方向を見ているということなのかと考えられる
			//一方で1を超えても描画されなくなる
			//w座標は視野の幅を示しているらしい(定数扱いの場合もある)
			//なので大きくするとものが小さく見える
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
			//vbDescのByteWidthを変えれば頂点バッファのサイズは変えれる使うデータの量は制限できる

			//-----------------------------
			// 頂点バッファ作成
			// ・上記で作った３つの頂点はそのままでは描画に使用できないんす…
			// ・ビデオメモリ側に「頂点バッファ」という形で作る必要があります！
			// ・今回は効率無視して、その場で作って、使って、すぐ捨てます。
			//-----------------------------
			// 作成するバッファの仕様を決める
			// ・今回は頂点バッファにするぞ！って感じの設定
			D3D11_BUFFER_DESC vbDesc = {};
			vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;	// デバイスにバインドするときの種類(頂点バッファ、インデックスバッファ、定数バッファなど)
			vbDesc.ByteWidth = sizeof(v);					// 作成するバッファのバイトサイズ
			//起動時に確保して使いまわす場合ここのサイズをあらかじめ決める必要があり、それが描画可能量の限界となる
			vbDesc.MiscFlags = 0;							// その他のフラグ
			vbDesc.StructureByteStride = 0;					// 構造化バッファの場合、その構造体のサイズ
			vbDesc.Usage = D3D11_USAGE_DYNAMIC;				// 作成するバッファの使用法(DYNAMICにしないとあとから書き換えられない)
			vbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; //WRITEを指定してないと書き換えられない

			// 上の仕様を渡して頂点バッファを作ってもらう
			// もちろんデバイスさんにお願いする
			ComPtr<ID3D11Buffer> vb;
			D3D11_SUBRESOURCE_DATA initData = { &v[0], sizeof(v), 0 };	// 書き込むデータ
			// 頂点バッファの作成
			D3D.m_device->CreateBuffer(&vbDesc, &initData, &vb);
			//++++++++++++++++++++ ここから描画していきます ++++++++++++++++++++

			//書き換えの実験
			for (int i = 0; i < vCount; i++) {
				float x = std::cos(3.1415926535 * -2 * i / vCount) * (0.02 + 0.08 * (i % 2));
				float y = std::sin(3.1415926535 * -2 * i / vCount) * (0.02 + 0.08 * (i % 2));
				float U = ((i - i % 2) / 2) % 2;
				float V = (i % 2);
				v[i] = { {x,y,0.2},{U,V} };
			}


			D3D11_MAPPED_SUBRESOURCE mappedSubresource;
			//ここでGPUから頂点バッファへのアクセスを止め、CPUからのアクセスができるようにしている
			D3D.m_deviceContext->Map(vb.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
			//ここでCPUによる処理でバッファの内容を変えている
			memcpy(mappedSubresource.pData, v, sizeof(v));//第3変数を変えることで書き換える数を変えられる
			//ここでGPUにアクセスを許可してる
			D3D.m_deviceContext->Unmap(vb.Get(),0);

			//バッファを2つ作ってdeviceContext->CopyResource()を使ってデータを移すことも可能

			// ここからは「描画の職人」デバイスコンテキストくんが大活躍

			// 頂点バッファを描画で使えるようにセットする
			UINT stride = sizeof(VertexType);
			UINT offset = 0;
			//頂点バッファのうち何番から(第1引数)何個を(第2引数)を使うのか
			//vbは配列の先頭のアドレスを指すことになる
			//strideとoffsetも各頂点バッファごとに指定することになるのでこれらも配列
			D3D.m_deviceContext->IASetVertexBuffers(0, 1, vb.GetAddressOf(), &stride, &offset);
			// プリミティブ・トポロジーをセット
			//頂点座標の処理方法
			//時計回りから数え始める
			//時計→半時計→時計…
			//最新の頂点とその1つ前に作った頂点に面するように新しい三角形を作っていく
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
			// インデックスバッファというのを使うと一つの頂点を使いまわして処理ができるらしい？


			//-----------------------------
			// シェーダーをセット
			//-----------------------------
			D3D.m_deviceContext->VSSetShader(D3D.m_spriteVS.Get(), 0, 0);
			D3D.m_deviceContext->PSSetShader(D3D.m_spritePS.Get(), 0, 0);
			//こいつはよく分からん
			D3D.m_deviceContext->IASetInputLayout(D3D.m_spriteInputLayout.Get());


			//-----------------------------
			// 描画実行
			//-----------------------------
			// デバイスコンテキストくん、上記のセットした内容で描画してください、とお願いする
			//vertexCountを変えることでおそらく演算量を制限可能

			//前後関係はDrawの順序に依らず適切に処理されるので心配はいらない

			D3D.m_deviceContext->Draw(vCount, 0);
		}
		*/
		// カメラ・定数バッファ
		mCamera.Update();
		DirectX::XMStoreFloat4x4(&mCBuffer.Data.ViewProjection, mCamera.CameraMatrix);
		mCBuffer.UpdateAndSet();
		// テクスチャ
		mSameFormatTextureArray.SetToGraphicPipeLine();
		//IASetVertexBufferの順序が影響するみたい
		// 実際の描画

		mBlockInstanceBuffer.UpdateAndSet(mBlockAppearances.Instances, 0, mBlockAppearances.InstanceCount);
		mBlockDrawCallBuffer.UpdateAndSet(mBlockAppearances.DrawCall, 0, 4);
		mGraphicProcessSetter.SetAsBlock();
		D3D.m_deviceContext->DrawInstanced(4, mBlockAppearances.InstanceCount, 0, 0);

		for (int i = 0; i < 3; i++) {
			mCharacterDrawCallBuffer[i].UpdateAndSet(mCharacterAppearances[i].DrawCall, 0, 4);
			mCharacterInstanceBuffer[i].UpdateAndSet(mCharacterAppearances[i].Instances, 0, mCharacterAppearances[i].InstanceCount);
			mGraphicProcessSetter.SetAsCharacter();
			D3D.m_deviceContext->DrawInstanced(4, mCharacterAppearances[i].InstanceCount * 2, 0, 0);
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
//地面と壁は別オブジェクトとする
void GameSystem::LoadMap(std::string mapFileName)
{
	//マップデータの読み込み
	Json::Value root;
	std::ifstream file = std::ifstream(mapFileName);
	file >> root;
	file.close();
	//マップサイズの読み込み
	int width = root.get("width", "").asInt();
	int height = root.get("height", "").asInt();
	if (width > 256 || height > 256) {
		throw("マップのサイズが大きすぎます");
	}
	mHurtboxes = Hurtboxes(&mAllEntities, width, height);
	Json::Value areaList = root.get("areaList", "");
	//その場所に壁があるか
	std::bitset<256 * 256> isWallMap;
	//その場所の壁をすでに配置したか
	std::bitset<256 * 256> wallPlaced;
	//その場所の床をすでに配置したか
	std::bitset<256 * 256> floorPlaced;
	isWallMap.reset();
	wallPlaced.reset();
	floorPlaced.reset();
	//壁がある場所を記録する
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
	//実際にブロックのデータをエンティティリストに記録する
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
						wallPlaced[256 * y + x] = true;
						Interface::RelationOfCoord world;
						world.Identity();
						world.Parallel = { (float)x,(float)y,0.0f,1.0f };
						int maskIndex = 0;
						if (x != 0 && !isWallMap[y * 256 + x - 1]) {
							maskIndex += 1;
						}
						if (x != width - 1 && !isWallMap[y * 256 + x + 1]) {
							maskIndex += 2;
						}
						if (y != 0 && !isWallMap[(y - 1) * 256 + x]) {
							maskIndex += 4;
						}
						mAllEntities.AddFromEntPointer(blockPointer, nullptr, &world, maskIndex,true,-1);
					}
				}
			}
		}
		else {
			for (int x = left; x < right; x++) {
				for (int y = bottom; y < top; y++) {
					if (!floorPlaced[256 * y + x]) {
						floorPlaced[256 * y + x] = true;
						Interface::RelationOfCoord world;
						world.Identity();
						world.Parallel = { (float)x,(float)y,0.0f,1.0f };
						int maskIndex = 0;
						if (x != 0 && isWallMap[y * 256 + x - 1]) {
							maskIndex += 1;
						}
						if (x != width - 1 && isWallMap[y * 256 + x + 1]) {
							maskIndex += 2;
						}
						if (y != 0 && isWallMap[(y - 1) * 256 + x]) {
							maskIndex += 4;
						}
						if (y != height - 1 && isWallMap[(y + 1) * 256 + x]) {
							maskIndex += 8;
						}
						mAllEntities.AddFromEntPointer(blockPointer, nullptr, &world, maskIndex,true,-1);
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
void GameSystem::LoadUnits(std::vector<std::string> filePathes) {
	for (int i = 0; i < filePathes.size(); i++) {
		Json::Value root;
		std::ifstream file = std::ifstream(filePathes[i]);
		file >> root;
		file.close();
		auto unitNames = root.getMemberNames();
		for (int i = 0; i < unitNames.size(); i++) {
			Interface::UnitInfoTable.emplace(std::pair<std::string, Interface::UnitInfo>(unitNames[i],
				Interface::UnitInfo(root.get(unitNames[i], ""))));
		}
	}
}