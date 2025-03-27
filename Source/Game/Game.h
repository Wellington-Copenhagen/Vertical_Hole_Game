#pragma once

#include "framework.h"
#include "Source/DirectX/DirectX.h"
#include "Camera.h"
#include "Input.h"
#include "HitBox.h"
#include "System.h"
#include "Component.h"
#include "Entity.h"
#include "Source/DirectX/Buffers.h"
#include "Source/DirectX/Texture.h"
#include "Source/DirectX/DrawPipe.h"
#include "Source/DirectX/Appearance.h"
#include "Routing.h"
#include "Source/DirectX/StringDraw.h"

//=========================================
// GameSystemクラス
// ・このゲームの土台となるもの
//=========================================
extern SameFormatTextureArray<256> BlockTextureArray;
extern SameFormatTextureArray<256> BallTextureArray;
extern SameFormatTextureArray<256> BulletTextureArray;
extern SameFormatTextureArray<256> LineTextureArray;
class GameSystem
{
public :
	AllSystem mAllSystem;
	Entities mEntities;


	Hurtboxes mHurtboxes;
	ConstantBuffer mCBuffer;

	// 頂点バッファと頂点のデータを記録するAppearance

	//マップ最大サイズに床と壁全部配置した場合の大きさ
	VertexBuffer<Interface::BlockDrawCallType, 4,0> mFloorDrawCallBuffer;
	VertexBuffer<Interface::BlockDrawCallType, 4, 0> mWallDrawCallBuffer;
	Appearances<Interface::BlockDrawCallType, Interface::BlockInstanceType, 1, WorldWidth* WorldHeight> mFloorAppearances;
	VertexBuffer<Interface::BlockInstanceType, WorldWidth* WorldHeight,1> mFloorInstanceBuffer;
	VertexBuffer<Interface::BlockInstanceType, WorldWidth* WorldHeight, 1> mWallInstanceBuffer;
	Appearances<Interface::BlockDrawCallType, Interface::BlockInstanceType, 1, WorldWidth* WorldHeight> mWallAppearances;


	//影、本体、模様
	VertexBuffer<Interface::BallDrawCallType, 4,0> mBallDrawCallBuffer[3];
	VertexBuffer<Interface::BallInstanceType, MaxBallCount,1> mBallInstanceBuffer[3];
	Appearances<Interface::BallDrawCallType, Interface::BallInstanceType, 1, MaxBallCount> mBallAppearances[3];



	VertexBuffer<Interface::BulletDrawCallType, 4,0> mBulletDrawCallBuffer;
	VertexBuffer<Interface::BulletInstanceType, MaxBulletCount,1> mBulletInstanceBuffer;
	Appearances<Interface::BulletDrawCallType, Interface::BulletInstanceType, 1, MaxBulletCount> mBulletAppearances;

	VertexBuffer<Interface::LineDrawCallType, 4, 0> mLineDrawCallBuffer;
	VertexBuffer<Interface::LineInstanceType, MaxLineCount, 1> mLineInstanceBuffer;
	Appearances<Interface::LineDrawCallType, Interface::LineInstanceType, 1, MaxLineCount> mLineAppearances;



	int DrawCallUsed;
	Camera mCamera;
	std::chrono::system_clock::time_point RefreshedTimeStamp;

	HDC hdc;
	// 32*32*64
	GraphicalStringDraw<65536,2048,32> StringDrawTest;

	// このゲームの初期設定を行う
	void Initialize(HWND hWnd);

	// このゲーム世界の時間を進める(処理を実行する)
	void Execute(HWND hWnd);


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

