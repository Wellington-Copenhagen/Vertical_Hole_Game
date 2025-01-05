#pragma once

#include "framework.h"
#include "Source/DirectX/DirectX.h"



#include "Source/Game/Game.h"
#include "Source/Game/Camera.h"
//=========================================
// GameSystemクラス
// ・このゲームの土台となるもの
//=========================================
class GameSystem
{
public:
	int timeCount;


	Object Root;
	Object Sphere;
	Object Ground;
	//頂点バッファを使いまわせないか確かめているもの



	// このゲームの初期設定を行う
	void Initialize();

	// このゲーム世界の時間を進める(処理を実行する)
	void Execute();

	// その他、ゲーム用のデータなどをココに書く

	Texture m_tex;
	/*
	struct ConstantBufferData {
		DirectX::XMFLOAT4X4 ViewProjection;
	};
	ComPtr<ID3D11Buffer> ConstantBuffer;
	*/
//=========================================
// 今回このクラスも、どこからでもアクセスできるように
// シングルトンパターンにしておきます。
// ↓↓↓以下、シングルトンパターンのコード
//=========================================
private:
	// 唯一のインスタンス用のポインタ
	static inline GameSystem* s_instance;
	// コンストラクタはprivateにする
	GameSystem();
public:
	// インスタンス作成
	static void CreateInstance()
	{
		DeleteInstance();

		s_instance = new GameSystem();
	}
	// インスタンス削除
	static void DeleteInstance()
	{
		if (s_instance != nullptr)
		{
			delete s_instance;
			s_instance = nullptr;
		}
	}
	// 唯一のインスタンスを取得
	static GameSystem& GetInstance()
	{
		return *s_instance;
	}
};

// GameSystemの唯一のインスタンスを簡単に取得するためのマクロ
#define GAMESYS GameSystem::GetInstance()


