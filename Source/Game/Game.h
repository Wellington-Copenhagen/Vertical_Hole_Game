#pragma once

#include "framework.h"
#include "Source/DirectX/DirectX.h"
#include "Camera.h"
#include "Appearance.h"
#include "Input.h"
#include "HitBox.h"
#include "System.h"
#include "Entity.h"
#include "Source/DirectX/Buffers.h"
#include "Source/DirectX/Texture.h"
#include "Source/DirectX/DrawPipe.h"

//=========================================
// GameSystemクラス
// ・このゲームの土台となるもの
//=========================================
class GameSystem
{
public :
	AllEntities mAllEntities;
	AllSystem mAllSystem;
	//マップ最大サイズに床と壁全部配置した場合の大きさ
	Appearances<Interface::BlockDrawCallType, Interface::BlockInstanceType, 1, 256 * 256 * 2> mBlockAppearances;
	//影、本体、模様
	Appearances<Interface::BallDrawCallType, Interface::BallInstanceType, 1, MaxBallCount> mBallAppearances[3];
	Appearances<Interface::BulletDrawCallType, Interface::BulletInstanceType, 1, 256 * 256> mBulletAppearances;
	Hurtboxes mHurtboxes;
	ConstantBuffer mCBuffer;
	VertexBuffer<Interface::BlockDrawCallType, 4,0> mBlockDrawCallBuffer;
	VertexBuffer<Interface::BlockInstanceType, 256 * 256 * 2,1> mBlockInstanceBuffer;
	//影、本体、模様、影
	VertexBuffer<Interface::BallDrawCallType, 4,0> mBallDrawCallBuffer[3];
	//影、本体、模様
	VertexBuffer<Interface::BallInstanceType, MaxBallCount,1> mBallInstanceBuffer[3];
	VertexBuffer<Interface::BulletDrawCallType, 4,0> mBulletDrawCallBuffer;
	VertexBuffer<Interface::BulletInstanceType, 256 * 256,1> mBulletInstanceBuffer;
	GraphicProcessSetter mGraphicProcessSetter;
	int DrawCallUsed;
	Camera mCamera;
	int Tick;


	SameFormatTextureArray mSameFormatTextureArray;
	// このゲームの初期設定を行う
	void Initialize(HWND hWnd);

	// このゲーム世界の時間を進める(処理を実行する)
	void Execute(HWND hWnd);

	void LoadMap(std::string mapFileName);

	void ApplyInput();

	// その他、ゲーム用のデータなどをココに書く

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


