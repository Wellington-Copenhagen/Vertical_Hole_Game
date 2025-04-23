// GameSystemクラスを使えるようにする
#include "Game.h"
// Direct3Dクラスを使えるようにする
//#include "Source/DirectX/Direct3D.h"


//地形はありにしたいが3Dモデルは作れないので俯角を制限して
//ただのアクションゲーでもいいができれば戦略アクションにしたい
//設定を作りやすそうな意味ではダンジョン
//

// この辺の監視も極力entityで行いたい
// 決まるタイミング
// 開発による指定
// ・Entity.jsonとUnit.jsonに指定
// ・多くの性質
// ミッションによる指定
// ・Mission.jsonに指定
// ・攻撃、耐久などの倍率
// ・出現位置、出現タイミング
// プレイヤーによるカスタマイズ
// ・Unit.jsonによる指定
// ・色、Ballの組み方
// 

DirectX::XMMATRIX rotation;
// こいつらはデバッグ用ということで
GraphicalStringDraw globalStringDraw;
LineDraw globalLineDraw;
int Tick;
// ゲームの初期設定を行う
GameSystem::GameSystem() {
}
void terminateHandle() {
	DebugAssist::Save();
	std::abort();
}
void GameSystem::Initialize(HWND hWnd)
{
	DebugAssist::Init();
	//SameFormatTextureArray::SaveShadowTexture(64, "texWallShadow.tif", 0, 16, 8, 8);
	//SameFormatTextureArray::SaveShadowTexture(64, "texFloorShadow.tif", 8, 8, 8, 8);
	//SameFormatTextureArray::SaveWallMask(64, "texWallShadow.tif");
	//SameFormatTextureArray::SaveLensMask(64, "texLensMask.tif");
	std::set_terminate(terminateHandle);










	Tick = 0;
	mHurtboxes = Hurtboxes(&mEntities.Registry,&mEntities.HostilityTable);
	// テクスチャの初期化
	BlockTextureArray = SameFormatTextureArray(2048, true, 64);
	VariableTextureArray = SameFormatTextureArray(2048, true, 64);
	ConstantTextureArray = SameFormatTextureArray(2048, true, 64);
	DebugLogOutput("Texture buffer Initialization succeeded.");

	// 頂点バッファ、頂点データの初期化
	mFloorAppearances = EntityBindAppearances
		<Interface::BlockDCType, Interface::BlockIType,Component::BlockAppearance, 1>(WorldWidth * WorldHeight);
	Interface::InitDrawCallUV(mFloorAppearances.DrawCall, 0);
	Interface::InitDrawCallPos(mFloorAppearances.DrawCall, 0);
	mWallAppearances = EntityBindAppearances
		<Interface::BlockDCType, Interface::BlockIType, Component::BlockAppearance, 1>(WorldWidth * WorldHeight);
	Interface::InitDrawCallUV(mWallAppearances.DrawCall, 0);
	Interface::InitDrawCallPos(mWallAppearances.DrawCall, 0);
	mFloorDCBuffer = VertexBuffer<Interface::BlockDCType>(mFloorAppearances.GetDrawCallPointer(0), 4, 0);
	mFloorIBuffer = VertexBuffer<Interface::BlockIType>(mFloorAppearances.GetInstancePointer(0), WorldWidth * WorldHeight, 1);
	mWallDCBuffer = VertexBuffer<Interface::BlockDCType>(mWallAppearances.GetDrawCallPointer(0), 4, 0);
	mWallIBuffer = VertexBuffer<Interface::BlockIType>(mWallAppearances.GetInstancePointer(0), WorldWidth * WorldHeight, 1);

	/*
	mBallAppearances = EntityBindAppearances
		<Interface::BallDCType, Interface::BallIType,Component::BallAppearance, 3>(MaxBallCount);
	Interface::InitDrawCallUV(mBallAppearances.DrawCall, 0);
	Interface::InitDrawCallPos(mBallAppearances.DrawCall, 0, 0.4, 0.4, 0.4, -0.01);
	Interface::InitDrawCallUV(mBallAppearances.DrawCall, 1);
	Interface::InitDrawCallPos(mBallAppearances.DrawCall, 1, 0.5, 0.5, 0.5, 0);
	Interface::InitDrawCallUV(mBallAppearances.DrawCall, 2);
	Interface::InitDrawCallPos(mBallAppearances.DrawCall, 2, 0.2, 0.2, 0.2, -0.1);

	mBallDCBuffer = VertexBuffer<Interface::BallDCType>(mBallAppearances.GetDrawCallPointer(0), 4, 0);
	mBallIBuffer = VertexBuffer<Interface::BallIType>(mBallAppearances.GetInstancePointer(0), MaxBallCount, 1);

	mBulletAppearances = EntityBindAppearances
		<Interface::BulletDCType, Interface::BulletIType, Component::BulletAppearance, 1>(MaxBulletCount);
	Interface::InitDrawCallUV(mBulletAppearances.DrawCall, 0);
	Interface::InitDrawCallPos(mBulletAppearances.DrawCall, 0);
	mBulletDCBuffer = VertexBuffer<Interface::BulletDCType>(mBulletAppearances.GetDrawCallPointer(0), 4, 0);
	mBulletIBuffer = VertexBuffer<Interface::BulletIType>(mBulletAppearances.GetInstancePointer(0), MaxBulletCount, 1);

	mEffectAppearances = EntityBindAppearances
		<Interface::EffectDCType, Interface::EffectIType, Component::EffectAppearance, 1>(MaxEffectCount);
	Interface::InitDrawCallUV(mEffectAppearances.DrawCall, 0);
	Interface::InitDrawCallPos(mEffectAppearances.DrawCall, 0);
	mEffectDCBuffer = VertexBuffer<Interface::EffectDCType>(mEffectAppearances.GetDrawCallPointer(0), 4, 0);
	mEffectIBuffer = VertexBuffer<Interface::EffectIType>(mEffectAppearances.GetInstancePointer(0), MaxEffectCount, 1);
	//mLineAppearances = Appearances
		//<Interface::LineDCType, Interface::LineIType, 1, MaxLineCount>(4);
	//mLineDCBuffer = VertexBuffer<Interface::LineDCType, 4, 0>(mLineAppearances.DrawCall);
	//mLineIBuffer = VertexBuffer<Interface::LineIType, MaxLineCount, 1>(mLineAppearances.Instances);

	*/

	int appCount = 65536;
	mVariableAppearances = EntityBindAppearances
		<Interface::EffectDCType, Interface::EffectIType, Component::VariableAppearance, 1>(appCount);
	Interface::InitDrawCallUV(mVariableAppearances.DrawCall, 0);
	Interface::InitDrawCallPos(mVariableAppearances.DrawCall, 0);
	mConstantAppearances = EntityBindAppearances
		<Interface::GeneralDCType, Interface::GeneralIType, Component::ConstantAppearance, 1>(appCount);
	Interface::InitDrawCallUV(mConstantAppearances.DrawCall, 0);
	Interface::InitDrawCallPos(mConstantAppearances.DrawCall, 0);
	mVariableDCBuffer = VertexBuffer<Interface::EffectDCType>(mVariableAppearances.GetDrawCallPointer(0), 4, 0);
	mVariableIBuffer = VertexBuffer<Interface::EffectIType>(mVariableAppearances.GetInstancePointer(0), appCount, 1);
	mConstantDCBuffer = VertexBuffer < Interface::GeneralDCType > (mConstantAppearances.GetDrawCallPointer(0), 4, 0);
	mConstantIBuffer = VertexBuffer<Interface::GeneralIType>(mConstantAppearances.GetInstancePointer(0), appCount, 1);


	mVariableShadowAppearances = EntityBindAppearances
		<Interface::GeneralDCType, Interface::GeneralIType, Component::ShadowAppearance, 1>(appCount);
	Interface::InitDrawCallUV(mVariableAppearances.DrawCall, 0);
	Interface::InitDrawCallPos(mVariableAppearances.DrawCall, 0);
	mConstantShadowAppearances = EntityBindAppearances
		<Interface::GeneralDCType, Interface::GeneralIType, Component::ShadowAppearance, 1>(appCount);
	Interface::InitDrawCallUV(mConstantAppearances.DrawCall, 0);
	Interface::InitDrawCallPos(mConstantAppearances.DrawCall, 0);
	mShadowDCBuffer = VertexBuffer<Interface::GeneralDCType>(mVariableShadowAppearances.GetDrawCallPointer(0), 4, 0);
	mShadowIBuffer = VertexBuffer<Interface::GeneralIType>(mVariableShadowAppearances.GetInstancePointer(0), appCount, 1);

	DebugLogOutput("Drawing System Initialization succeeded.");





	int w = D3D.Width;
	int h = D3D.Height;
	mCamera = Camera();

	mCBuffer = ConstantBuffer();


	HFONT hFont = CreateFontW(28,//文字の高さ
		0,//文字の幅、0だと高さに合わせて調整される？
		0,//文の傾き？
		0,//文字の傾き？
		1000,//太さ　大きいほど太い
		FALSE,// 斜体
		FALSE,// 下線
		FALSE,// 取り消し線
		SHIFTJIS_CHARSET,//文字セット SHIFT_JISもある
		OUT_TT_ONLY_PRECIS,//精度　よくわからん
		CLIP_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY,//品質 とりあえずアンチエイリアスは使う
		DEFAULT_PITCH || FF_MODERN,//見た目のグループ？
		L"ＭＳ 明朝"//フォントの名前
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

	DebugLogOutput("Generic Drawers Initialization succeeded.");

	std::vector<std::string> EntityFileNames = {
		"Data/EntityData/Block.json",
		"Data/EntityData/Effect.json",
		"Data/EntityData/Bullet.json",
		"Data/EntityData/Ball.json"
	};
	mEntities.LoadEntities(EntityFileNames);

	std::vector<std::string> BallFileNames = {
		"Data/BallData/Ball.json",
	};
	mEntities.LoadBalls(BallFileNames);


	std::vector<std::string> UnitFileNames = {
		"Data/UnitData/Unit.json",
	};
	mEntities.LoadUnits(UnitFileNames);

	mAllSystem = AllSystem(this);

	std::random_device RandSeedGen;
	Interface::RandEngine = std::mt19937(RandSeedGen());

	GraphicProcessSetter(D3D.Width, D3D.Height);

	SetUnitThumbnail();

	std::string missionName = "Data/MissionData/Mission_Test.json";

	mEntities.LoadMission(missionName);

	mEntities.LoadPlayerCorps("Data/SaveData/PlayerCorps.json");



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
	pFloorAppearances = &pGameSystem->mFloorAppearances;
	pWallAppearances = &pGameSystem->mWallAppearances;
	
	pConstantAppearances = &pGameSystem->mConstantAppearances;
	pVariableAppearances = &pGameSystem->mVariableAppearances;

	pConstantShadowAppearances = &pGameSystem->mConstantShadowAppearances;
	pVariableShadowAppearances = &pGameSystem->mVariableShadowAppearances;
	pEntities = &pGameSystem->mEntities;
}
// このゲームの時間を進める(処理を実行する)
SameFormatTextureArray testArray;
int timeSum = 0;
int timeMin = 0;
int timeMax = 0;
void GameSystem::Execute(HWND hWnd,int o) {
	try {
		bool useConstant = true;
		int count = 65536;
		int zoomRate = 256;
		int width = 256;
		if (Tick == 0) {
			timeSum = 0;
			timeMin = 1000000000;
			timeMax = 0;
			testArray = SameFormatTextureArray(1, true, width);
			std::vector<byte> colorVector(width * width * 4);
			for (int i = 0; i < width * width; i++) {
				colorVector[i * 4 + 0] = Interface::UniformRandInt(0, 255);
				colorVector[i * 4 + 1] = Interface::UniformRandInt(0, 255);
				colorVector[i * 4 + 2] = Interface::UniformRandInt(0, 255);
				colorVector[i * 4 + 3] = 255;
			}
			testArray.AppendImage(SameFormatTextureArray::MakeImageFromColorVector(colorVector,width,width,4,1));
			// カメラ・定数バッファ
			mCamera.CenterCoord = { 0,0,0,1 };
			mCamera.ZoomRate = zoomRate;
			mCamera.Update();
			mCBuffer.UpdateAndSet(&mCamera.CameraMatrix, &testArray.BlackboxMatrices);

			if (useConstant) {
				DebugLogOutput("Constant Draw,texture size:{},object count:{},zoomRate:{}",
					width,
					count,
					zoomRate);
			}
			else {
				DebugLogOutput("Variable Draw,texture size:{},object count:{},zoomRate:{}",
					width,
					count,
					zoomRate);
			}
			testArray.SetToGraphicPipeLine();
		}
		float color[4] = { 0.9f, 0.9f, 0.9f, 1.0f };
		D3D.m_deviceContext->ClearRenderTargetView(D3D.m_backBufferView.Get(), color);
		D3D.m_deviceContext->ClearDepthStencilView(D3D.m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0.0f);
		mConstantAppearances.InstanceCount = count;
		for (int i = 0; i < 65536; i++) {
			DirectX::XMMATRIX matrix = DirectX::XMMatrixIdentity();
			matrix.r[3] = { Interface::UniformRandFloat((zoomRate - 1) / -2.0f,(zoomRate - 1) / 2.0f),
				Interface::UniformRandFloat((zoomRate - 1) / -2.0f,(zoomRate - 1) / 2.0f),(float)i / (float)count,1 };
			mConstantAppearances.Instances[i].Set(&matrix, 0);
		}
		mVariableAppearances.InstanceCount = count;
		for (int i = 0; i < 65536; i++) {
			DirectX::XMMATRIX matrix = DirectX::XMMatrixIdentity();
			matrix.r[3] = { Interface::UniformRandFloat((zoomRate - 1) / -2.0f,(zoomRate - 1) / 2.0f),
				Interface::UniformRandFloat((zoomRate - 1) / -2.0f,(zoomRate - 1) / 2.0f),
				1.0f - (float)i / (float)count,1 };
			DirectX::XMVECTOR color1 = { 1,0,0,1 };
			DirectX::XMVECTOR color2 = { 0,1,0,1 };
			mVariableAppearances.Instances[i].Set(&matrix, 0, &color1, &color2);
		}
		if (useConstant) {
			GraphicProcessSetter::SetAsBullet();
			mConstantIBuffer.UpdateAndSet(mConstantAppearances.GetInstancePointer(0), 0, mConstantAppearances.InstanceCount);
			mConstantDCBuffer.UpdateAndSet(mConstantAppearances.GetDrawCallPointer(0), 0, 4);
		}
		else {
			GraphicProcessSetter::SetAsEffect();
			mVariableIBuffer.UpdateAndSet(mVariableAppearances.GetInstancePointer(0), 0, mVariableAppearances.InstanceCount);
			mVariableDCBuffer.UpdateAndSet(mVariableAppearances.GetDrawCallPointer(0), 0, 4);
		}


		auto start = std::chrono::system_clock::now();
		if (useConstant) {
			D3D.m_deviceContext->DrawInstanced(4, mConstantAppearances.InstanceCount, 0, 0);
		}
		else {
			D3D.m_deviceContext->DrawInstanced(4, mVariableAppearances.InstanceCount, 0, 0);
		}
		D3D.m_swapChain->Present(1, 0);
		std::chrono::system_clock::time_point end = std::chrono::system_clock::now();
		int time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
		timeSum = timeSum + time;
		timeMax = max(timeMax, time);
		timeMin = min(timeMin, time);
		if (Tick % 600 == 599) {
			DebugLogOutput("Average process time(ms/tick):{}", timeSum / (600.0f * 1000.0f));
			DebugLogOutput("Max process time(ms/tick):{}", timeMax / (1000.0f));
			DebugLogOutput("Min process time(ms/tick):{}", timeMin / (1000.0f));
			timeSum = 0;
			timeMin = 1000000000;
			timeMax = 0;
			throw("");
		}
		OutputDebugStringA(("Tick:" + std::to_string(Tick) + "\n").c_str());
		Tick++;
	}
	catch (char* e) {
		OutputDebugStringA(e);
	}
}
void GameSystem::Execute(HWND hWnd)
{
	try {
		ApplyInput();
		mAllSystem.Update();
		float color[4] = { 0.9f, 0.9f, 0.9f, 1.0f };
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

		mFloorAppearances.EraseDeletedEntities(&mEntities.Registry);
		mWallAppearances.EraseDeletedEntities(&mEntities.Registry);
		mVariableAppearances.EraseDeletedEntities(&mEntities.Registry);
		mConstantAppearances.EraseDeletedEntities(&mEntities.Registry);


		// カメラ・定数バッファ
		mCamera.Update();
		// テクスチャ
		BlockTextureArray.SetToGraphicPipeLine();
		mCBuffer.UpdateAndSet(&mCamera.CameraMatrix, &BlockTextureArray.BlackboxMatrices);
		//IASetVertexBufferの順序が影響するみたい
		// 実際の描画
		GraphicProcessSetter::SetAsBlock();
		mFloorIBuffer.UpdateAndSet(mFloorAppearances.GetInstancePointer(0), 0, mFloorAppearances.InstanceCount);
		mFloorDCBuffer.UpdateAndSet(mFloorAppearances.GetDrawCallPointer(0), 0, 4);
		D3D.m_deviceContext->DrawInstanced(4, mFloorAppearances.InstanceCount, 0, 0);
		mWallIBuffer.UpdateAndSet(mWallAppearances.GetInstancePointer(0), 0, mWallAppearances.InstanceCount);
		mWallDCBuffer.UpdateAndSet(mWallAppearances.GetDrawCallPointer(0), 0, 4);
		D3D.m_deviceContext->DrawInstanced(4, mWallAppearances.InstanceCount, 0, 0);


		ConstantTextureArray.SetToGraphicPipeLine();
		mCBuffer.UpdateAndSet(&mCamera.CameraMatrix, &ConstantTextureArray.BlackboxMatrices);
		GraphicProcessSetter::SetAsBullet();
		mConstantIBuffer.UpdateAndSet(mConstantAppearances.GetInstancePointer(0), 0, mConstantAppearances.InstanceCount);
		mConstantDCBuffer.UpdateAndSet(mConstantAppearances.GetDrawCallPointer(0), 0, 4);
		D3D.m_deviceContext->DrawInstanced(4, mConstantAppearances.InstanceCount, 0, 0);

		VariableTextureArray.SetToGraphicPipeLine();
		mCBuffer.UpdateAndSet(&mCamera.CameraMatrix, &VariableTextureArray.BlackboxMatrices);
		GraphicProcessSetter::SetAsEffect();
		mVariableIBuffer.UpdateAndSet(mVariableAppearances.GetInstancePointer(0), 0, mVariableAppearances.InstanceCount);
		mVariableDCBuffer.UpdateAndSet(mVariableAppearances.GetDrawCallPointer(0), 0, 4);
		D3D.m_deviceContext->DrawInstanced(4, mVariableAppearances.InstanceCount, 0, 0);


		for (int i = 0; i < mVariableAppearances.InstanceCount; i++) {
			DirectX::XMMATRIX pos = DirectX::XMLoadFloat4x4(&mVariableAppearances.Instances[i].World);
			globalStringDraw.SimpleAppend(std::to_string(i), 1, 0, 0, pos.r[3], 0.3, 1, StrDrawPos::AsBottomLeftCorner);
		}
		GraphicProcessSetter::SetAsGeneral();
		VariableTextureArray.SetToGraphicPipeLine();
		mCBuffer.UpdateAndSet(&mCamera.CameraMatrix, &VariableTextureArray.BlackboxMatrices);
		mShadowIBuffer.UpdateAndSet(mVariableShadowAppearances.GetInstancePointer(0), 0, mVariableShadowAppearances.InstanceCount);
		mShadowDCBuffer.UpdateAndSet(mVariableShadowAppearances.GetDrawCallPointer(0), 0, 4);
		D3D.m_deviceContext->DrawInstanced(4, mVariableShadowAppearances.InstanceCount, 0, 0);
		ConstantTextureArray.SetToGraphicPipeLine();
		mCBuffer.UpdateAndSet(&mCamera.CameraMatrix, &ConstantTextureArray.BlackboxMatrices);
		mShadowIBuffer.UpdateAndSet(mConstantShadowAppearances.GetInstancePointer(0), 0, mConstantShadowAppearances.InstanceCount);
		mShadowDCBuffer.UpdateAndSet(mConstantShadowAppearances.GetDrawCallPointer(0), 0, 4);
		D3D.m_deviceContext->DrawInstanced(4, mConstantShadowAppearances.InstanceCount, 0, 0);



		StringDrawTest.Update();
		StringDrawTest.Draw(&mCBuffer);
		globalStringDraw.Update();
		globalStringDraw.Draw(&mCBuffer);
		globalLineDraw.Update();
		//globalLineDraw.Draw(&mCBuffer);

		// ここで定数バッファが書き換えられているので注意
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
void GameSystem::SetUnitThumbnail() {
	for (int i = 0; i < mEntities.UnitInfos.size(); i++) {
		mEntities.Registry.clear();
		mFloorAppearances.EraseDeletedEntities(&mEntities.Registry);
		mWallAppearances.EraseDeletedEntities(&mEntities.Registry);
		mVariableAppearances.EraseDeletedEntities(&mEntities.Registry);
		mConstantAppearances.EraseDeletedEntities(&mEntities.Registry);
		DirectX::XMMATRIX pos{
			1,0,0,0,
			0,1,0,0,
			0,0,1,0,
			0,0,0,1
		};
		Interface::EntityInitData initData;
		initData.Team = 0;
		mEntities.EmplaceUnitWithInitData(i, &initData, &pos);
		Systems::DrawVariable variableDraw(this);
		Systems::DrawConstant constantDraw(this);
		Systems::DrawShadow shadowDraw(this);
		variableDraw.Update();
		constantDraw.Update();
		shadowDraw.Update();
		// カメラ・定数バッファ
		mCamera.MoveCamera({ 0,0,0,1 });
		mCamera.ZoomRate = D3D.Height / 48;
		mCamera.Update();



		float color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		D3D.m_deviceContext->ClearRenderTargetView(D3D.m_backBufferView.Get(), color);

		ConstantTextureArray.SetToGraphicPipeLine();
		mCBuffer.UpdateAndSet(&mCamera.CameraMatrix, &ConstantTextureArray.BlackboxMatrices);
		GraphicProcessSetter::SetAsBullet();
		mConstantIBuffer.UpdateAndSet(mConstantAppearances.GetInstancePointer(0), 0, mConstantAppearances.InstanceCount);
		mConstantDCBuffer.UpdateAndSet(mConstantAppearances.GetDrawCallPointer(0), 0, 4);
		D3D.m_deviceContext->DrawInstanced(4, mConstantAppearances.InstanceCount, 0, 0);

		VariableTextureArray.SetToGraphicPipeLine();
		mCBuffer.UpdateAndSet(&mCamera.CameraMatrix, &VariableTextureArray.BlackboxMatrices);
		GraphicProcessSetter::SetAsEffect();
		mVariableIBuffer.UpdateAndSet(mVariableAppearances.GetInstancePointer(0), 0, mVariableAppearances.InstanceCount);
		mVariableDCBuffer.UpdateAndSet(mVariableAppearances.GetDrawCallPointer(0), 0, 4);
		D3D.m_deviceContext->DrawInstanced(4, mVariableAppearances.InstanceCount, 0, 0);

		GraphicProcessSetter::SetAsGeneral();
		mShadowIBuffer.UpdateAndSet(mVariableShadowAppearances.GetInstancePointer(0), 0, mVariableShadowAppearances.InstanceCount);
		mShadowDCBuffer.UpdateAndSet(mVariableShadowAppearances.GetDrawCallPointer(0), 0, 4);
		D3D.m_deviceContext->DrawInstanced(4, mVariableShadowAppearances.InstanceCount, 0, 0);

		ConstantTextureArray.SetToGraphicPipeLine();
		mCBuffer.UpdateAndSet(&mCamera.CameraMatrix, &ConstantTextureArray.BlackboxMatrices);
		mShadowIBuffer.UpdateAndSet(mConstantShadowAppearances.GetInstancePointer(0), 0, mConstantShadowAppearances.InstanceCount);
		mShadowDCBuffer.UpdateAndSet(mConstantShadowAppearances.GetDrawCallPointer(0), 0, 4);
		D3D.m_deviceContext->DrawInstanced(4, mConstantShadowAppearances.InstanceCount, 0, 0);
		std::unique_ptr<DirectX::ScratchImage> pBlackImage = SameFormatTextureArray::MakeImageFromRTV(D3D.Height / 2 - 32, D3D.Width / 2 - 32, 64, 64);



		color[0] = 1.0f;
		color[1] = 1.0f;
		color[2] = 1.0f;
		D3D.m_deviceContext->ClearRenderTargetView(D3D.m_backBufferView.Get(), color);

		ConstantTextureArray.SetToGraphicPipeLine();
		mCBuffer.UpdateAndSet(&mCamera.CameraMatrix, &ConstantTextureArray.BlackboxMatrices);
		GraphicProcessSetter::SetAsBullet();
		mConstantIBuffer.UpdateAndSet(mConstantAppearances.GetInstancePointer(0), 0, mConstantAppearances.InstanceCount);
		mConstantDCBuffer.UpdateAndSet(mConstantAppearances.GetDrawCallPointer(0), 0, 4);
		D3D.m_deviceContext->DrawInstanced(4, mConstantAppearances.InstanceCount, 0, 0);

		VariableTextureArray.SetToGraphicPipeLine();
		mCBuffer.UpdateAndSet(&mCamera.CameraMatrix, &VariableTextureArray.BlackboxMatrices);
		GraphicProcessSetter::SetAsEffect();
		mVariableIBuffer.UpdateAndSet(mVariableAppearances.GetInstancePointer(0), 0, mVariableAppearances.InstanceCount);
		mVariableDCBuffer.UpdateAndSet(mVariableAppearances.GetDrawCallPointer(0), 0, 4);
		D3D.m_deviceContext->DrawInstanced(4, mVariableAppearances.InstanceCount, 0, 0);


		GraphicProcessSetter::SetAsGeneral();
		mShadowIBuffer.UpdateAndSet(mVariableShadowAppearances.GetInstancePointer(0), 0, mVariableShadowAppearances.InstanceCount);
		mShadowDCBuffer.UpdateAndSet(mVariableShadowAppearances.GetDrawCallPointer(0), 0, 4);
		D3D.m_deviceContext->DrawInstanced(4, mVariableShadowAppearances.InstanceCount, 0, 0);

		ConstantTextureArray.SetToGraphicPipeLine();
		mCBuffer.UpdateAndSet(&mCamera.CameraMatrix, &ConstantTextureArray.BlackboxMatrices);
		mShadowIBuffer.UpdateAndSet(mConstantShadowAppearances.GetInstancePointer(0), 0, mConstantShadowAppearances.InstanceCount);
		mShadowDCBuffer.UpdateAndSet(mConstantShadowAppearances.GetDrawCallPointer(0), 0, 4);
		D3D.m_deviceContext->DrawInstanced(4, mConstantShadowAppearances.InstanceCount, 0, 0);
		std::unique_ptr<DirectX::ScratchImage> pWhiteImage = SameFormatTextureArray::MakeImageFromRTV(D3D.Height / 2 - 32, D3D.Width / 2 - 32, 64, 64);



		SameFormatTextureArray::SaveImage(SameFormatTextureArray::GetTransparentFromOpaque(std::move(pBlackImage), std::move(pWhiteImage)), "thumbnail/" + std::to_string(i) + ".tif");
		//mEntities.UnitInfos[i].ThumbnailTextureIndex = mUserInterface.mFreeShapeDraw.mTextureArray.AppendImage(SameFormatTextureArray::MakeImageFromRTV(D3D.Height / 2 - 32, D3D.Width / 2 - 32, 64, 64));
	}
	mEntities.Registry.clear();
	mFloorAppearances.EraseDeletedEntities(&mEntities.Registry);
	mWallAppearances.EraseDeletedEntities(&mEntities.Registry);
	mVariableAppearances.EraseDeletedEntities(&mEntities.Registry);
	mConstantAppearances.EraseDeletedEntities(&mEntities.Registry);
}