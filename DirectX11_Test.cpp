// DirectX11_Test.cpp : アプリケーションのエントリ ポイントを定義します。
//


#include "iostream"
#include "framework.h"
#include "Interfaces.h"
#include "DirectX11_Test.h"
#include "Source/DirectX/Direct3D.h"
#include "Source/Game/Game.h"
#include "Source/Game/Camera.h"
#define MAX_LOADSTRING 100

// グローバル変数:
HINSTANCE hInst;                                // 現在のインターフェイス
//szTitleは左上に表示される名前
//被所有ウィンドウだとボタンに書かれる言葉もこれになるらしい
WCHAR szTitle[MAX_LOADSTRING];                  // タイトル バーのテキスト
//こっちはよく分からん
WCHAR szWindowClass[MAX_LOADSTRING];            // メイン ウィンドウ クラス名

// このコード モジュールに含まれる関数の宣言を転送します:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

//こいつ自体はWindowsが呼んでる(エントリーポイントなので)
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    //使ってない変数を明示してるだけ？
    //互換性のためにあるだけのいらない子
    UNREFERENCED_PARAMETER(hPrevInstance);
    //実行時に後ろ側に指定された文字列
    //とはいっても、コマンドプロンプトから起動でもしない限りは使えない機能
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: ここにコードを挿入してください。

    // グローバル文字列を初期化する
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_DIRECTX11TEST, szWindowClass, MAX_LOADSTRING);
    //プロシージャやらアイコンやらのウィンドウの情報を登録している
    MyRegisterClass(hInstance);

    // アプリケーション初期化の実行:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }
    //Acceleratorsはショートカットを追加する機能
    //これはResource.hにあるAcceleratorsの定義を読みだしてハンドルをとってる
    //というかおそらくICONとかもリソースに書いてwWinMainで呼ぶ感じだと思う
    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DIRECTX11TEST));

    MSG msg;

    //GameSystemの方の初期化
    GameSystem:: CreateInstance();
    GAMESYS.Initialize();

    /*
    // メイン メッセージ ループ:
    //GetMessageはWM_QUITを受け取るまではtrueを返し続ける
    //第2変数でメッセージを取るウィンドウを決めたり、第3，4変数で返すメッセージの種類を決めたりできる
    //だけどGetmessageは何かメッセージが来るまでは関数から出ないのでゲームだと操作してない間画面が動かなくなる
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        //hAccelTableにある入力だった場合はtrueを返す
        //変換できた場合はそのままこれ自体がプロシージャを呼ぶのでifの中の処理でプロシージャは呼ばない
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            //翻訳してプロシージャを呼ぶ
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    */
    while (true)
    {
        //peekMessageの場合はメッセージがなかったらfalseを返して関数から出る
        //第1～4変数はGetMessageと同様、5は出力したメッセージをキューから消すとかそういう
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT) {
                Direct3D::DeleteInstance();
                //返り値が何に使われるのかは不明
                return (int)msg.wParam;
            }
            else {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        //メッセージの有無にかかわらず処理することはここに書く
        /*
        //色の指定
        float color[4] = {0.0f,0.0f,1.0f,1.0f};//R,G,B,A(透明度か？)
        //GetはComPtrに付いてる関数で中身のポインタをポインタ型で返してる
        //関数それ自体はClearとあることから塗りつぶし系だろう
        D3D.m_deviceContext->ClearRenderTargetView(D3D.m_backBufferView.Get(), color);
        //表示させる部分
        D3D.m_swapChain->Present(1, 0);
        */
        GAMESYS.Execute();
    }
    GameSystem::DeleteInstance();
    Direct3D::DeleteInstance();
    //返り値が何に使われるのかは不明
    return (int) msg.wParam;
}



//
//  関数: MyRegisterClass()
//
//  目的: ウィンドウ クラスを登録します。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DIRECTX11TEST));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_DIRECTX11TEST);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
    //実際に登録している関数
    return RegisterClassExW(&wcex);
}

//
//   関数: InitInstance(HINSTANCE, int)
//
//   目的: インスタンス ハンドルを保存して、メイン ウィンドウを作成します
//
//   コメント:
//
//        この関数で、グローバル変数でインスタンス ハンドルを保存し、
//        メイン プログラム ウィンドウを作成および表示します。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    //大本はwWinMainの引数から引っ張ってきたもの
   hInst = hInstance; // グローバル変数にインスタンス ハンドルを格納する
   //ウィンドウクラスの名前
   //ウィンドウの名前
   //スタイル
   //初期位置(デフォルト)
   //初期サイズ(デフォルト)
   //親ウィンドウ、所有ウィンドウのハンドル
   //メニュー
   //インスタンスのハンドル
   //CREATESTRUCTをどうもここで渡すらしい…
   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }
   //Direct3Dクラス唯一のインスタンスの生成及び初期化
   Direct3D::CreateInstance();
   D3D.Initialize(hWnd, 1280, 720);
   //表示するだけ
   //nCmdShowをSW_~に帰れば初期表示の大きさとかが変わる
   ShowWindow(hWnd, nCmdShow);
   //全部描画しなおせ！
   //とはいっても今は作りたてのウィンドウなので当然である
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  関数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的: メイン ウィンドウのメッセージを処理します。
//
//  WM_COMMAND  - アプリケーション メニューの処理
//  WM_PAINT    - メイン ウィンドウを描画する
//  WM_DESTROY  - 中止メッセージを表示して戻る
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    //要はイベントの処理が書いてあるだけ
    switch (message)
    {
    case WM_KEYDOWN:
        {
            DirectX::XMMATRIX rotation = CAMERA.GetRotationMatrix();
            int vkId = wParam;
            switch (vkId)
            {
            case 0x53://s
                CAMERA.ParallelMovement = DirectX::XMVectorAdd(CAMERA.ParallelMovement
                    ,DirectX::XMVector3Transform({ 0,0,-1 }, rotation));
                break;
            case 0x57://w
                CAMERA.ParallelMovement = DirectX::XMVectorAdd(CAMERA.ParallelMovement
                    , DirectX::XMVector3Transform({ 0,0,1 }, rotation));
                break;
            case 0x41://a
                CAMERA.ParallelMovement = DirectX::XMVectorAdd(CAMERA.ParallelMovement
                    , DirectX::XMVector3Transform({ -1,0,0 }, rotation));
                break;
            case 0x44://d
                CAMERA.ParallelMovement = DirectX::XMVectorAdd(CAMERA.ParallelMovement
                    , DirectX::XMVector3Transform({ 1,0,0 }, rotation));
                break;
            case 0x43://c
                CAMERA.FOV = CAMERA.FOV * 1.03;
                break;
            case 0x5A://z
                CAMERA.FOV = CAMERA.FOV / 1.03;
                break;
            case VK_SPACE:
                CAMERA.ParallelMovement = DirectX::XMVectorAdd(CAMERA.ParallelMovement, {0,1,0});
                break;
            case VK_CONTROL:
                CAMERA.ParallelMovement = DirectX::XMVectorAdd(CAMERA.ParallelMovement, { 0,-1,0 });
                break;
            case VK_LEFT:
                CAMERA.rotationYaxis -= PI / 90;
                break;
            case VK_RIGHT:
                CAMERA.rotationYaxis += PI / 90;
                break;
            case VK_UP:
                CAMERA.rotationXaxis -= PI / 90;
                break;
            case VK_DOWN:
                CAMERA.rotationXaxis += PI / 90;
                break;
            default:
                break;
            }
        }
        break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 選択されたメニューの解析:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: HDC を使用する描画コードをここに追加してください...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// バージョン情報ボックスのメッセージ ハンドラーです。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
