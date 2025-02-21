#pragma once
#include "framework.h"
#define PI 3.1415926535
#define TeamCount 2
#define StatusCount 3
//DirectX系のファイルは全部こいつ経由でincludeする

// Direct3Dのライブラリを使用できるようにする
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

// Direct3Dの型・クラス・関数などを呼べるようにする
#include <d3d11.h>
#include <d3dcompiler.h>

// DirectXMath(数学ライブラリ)を使用できるようにする
#include <DirectXMath.h>

// DirectXテクスチャライブラリを使用できるようにする
#include <DirectXTex.h>
//NuGetからインストールする必要あり

// ComPtrを使用できるようにする
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

// 自作のDirectX関係のヘッダーをインクルード

#include "Direct3D.h"