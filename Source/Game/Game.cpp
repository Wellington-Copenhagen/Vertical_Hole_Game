// GameSystemクラスを使えるようにする
#include "Game.h"
// Direct3Dクラスを使えるようにする
//#include "Source/DirectX/Direct3D.h"

#include "framework.h"

#include "Source/DirectX/DirectX.h"
#include "Source/Game/Game.h"
#include "Source/DirectX/DrawPipe.h"

//地形はありにしたいが3Dモデルは作れないので俯角を制限して
//ただのアクションゲーでもいいができれば戦略アクションにしたい
//設定を作りやすそうな意味ではダンジョン
//

DirectX::XMMATRIX rotation;
// ゲームの初期設定を行う
GameSystem::GameSystem() {

}
void GameSystem::Initialize()
{
	timeCount = 0;
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

		//回転して移動

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
//4つの点を指定できれば一意に他の点を指定できるはずなので一塊になった点群はその4点との関係性だけで記述することができ、4点の移動だけでその点群の移動を表現できる
//A,B,C,Dの4点としたらA→B,A→C,A→Dが独立であるならこれら3つの係数のだけで点を表現できる
//原点で生成して移動させるのが吉か
//トライアングルストリップでもあらゆるものが1つながりで描画できるわけではないだろうから
//頂点を求められる係数3つのデータのリストをいくつか格納できるようにしておいたほうが良いだろう
// このゲームの時間を進める(処理を実行する)
void GameSystem::Execute()
{
	// 画面を青色で塗りつぶす
	float color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	D3D.m_deviceContext->ClearRenderTargetView(D3D.m_backBufferView.Get(), color);
	D3D.m_deviceContext->ClearDepthStencilView(D3D.m_depthStencilView.Get(),D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,1.0f,0.0f);
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
	DP.cBuffer.Data.SunDirection = { 0,-1,0 ,0};
	Sphere.Draw();
	Root.Draw();
	Ground.Draw();
	//Root.Positions[0].rotationXaxis = Root.Positions[0].rotationXaxis + (3.1415926535 / 180.0);
	//CAMERA.rotationYaxis = CAMERA.rotationYaxis + (3.1415926535 / 270.0);
	// バックバッファの内容を画面に表示
	D3D.m_swapChain->Present(1, 0);
	timeCount++;
}