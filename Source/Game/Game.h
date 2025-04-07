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
#include "Source/DirectX/LineDraw.h"
#include "UserInterface.h"

//=========================================
// GameSystemクラス
// ・このゲームの土台となるもの
//=========================================
extern SameFormatTextureArray BlockTextureArray;
extern SameFormatTextureArray BallTextureArray;
extern SameFormatTextureArray BulletTextureArray;
extern SameFormatTextureArray EffectTextureArray;
class GameSystem
{
public :


	AllSystem mAllSystem;
	Entities mEntities;


	Hurtboxes mHurtboxes;
	ConstantBuffer mCBuffer;

	// 頂点バッファと頂点のデータを記録するAppearance

	//マップ最大サイズに床と壁全部配置した場合の大きさ
	VertexBuffer<Interface::BlockDrawCallType> mFloorDrawCallBuffer;
	VertexBuffer<Interface::BlockDrawCallType> mWallDrawCallBuffer;
	EntityBindAppearances<Interface::BlockDrawCallType, Interface::BlockInstanceType,Component::BlockAppearance, 1> mFloorAppearances;
	VertexBuffer<Interface::BlockInstanceType> mFloorInstanceBuffer;
	VertexBuffer<Interface::BlockInstanceType> mWallInstanceBuffer;
	EntityBindAppearances<Interface::BlockDrawCallType, Interface::BlockInstanceType, Component::BlockAppearance, 1> mWallAppearances;


	//影、本体、模様
	VertexBuffer<Interface::BallDrawCallType> mBallDrawCallBuffer[3];
	VertexBuffer<Interface::BallInstanceType> mBallInstanceBuffer[3];
	EntityBindAppearances<Interface::BallDrawCallType, Interface::BallInstanceType, Component::BallAppearance, 3> mBallAppearances;



	VertexBuffer<Interface::BulletDrawCallType> mBulletDrawCallBuffer;
	VertexBuffer<Interface::BulletInstanceType> mBulletInstanceBuffer;
	EntityBindAppearances<Interface::BulletDrawCallType, Interface::BulletInstanceType,Component::BulletAppearance, 1> mBulletAppearances;

	VertexBuffer<Interface::EffectDrawCallType> mEffectDrawCallBuffer;
	VertexBuffer<Interface::EffectInstanceType> mEffectInstanceBuffer;
	EntityBindAppearances<Interface::EffectDrawCallType, Interface::EffectInstanceType,Component::EffectAppearance, 1> mEffectAppearances;



	int DrawCallUsed;
	Camera mCamera;
	std::chrono::system_clock::time_point RefreshedTimeStamp;

	HDC hdc;
	// 32*32*64
	GraphicalStringDraw StringDrawTest;
	UserInterface mUserInterface;
 
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

