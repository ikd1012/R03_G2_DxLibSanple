//ヘッダファイルの読み込み
#include "DxLib.h"	//DxLibを使うときは必要
#include "keyboard.h"	//キーボードの処理


//マクロ定義
#define GAME_TITLE "ゲームタイトル"		//ゲームタイトル
#define GAME_WIDTH 1280					//ゲーム画面の幅(ウィドス)
#define GAME_HEIGHT 720					//ゲーム画面の高さ(ハイト)
#define GAME_COLOR 32					//ゲームの色域

#define GAME_ICON_ID 333				//ゲームのICONのID

#define GAME_WINDOW_BAR	0				//ウィンドウバーの状態

// プログラムは WinMain から始まります
//Windowsのプログラミング方法=(WinAPI)で動いている！
//DxLibは、DirectXという、ゲームプログラミングをかんたんに使える仕組み
int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	SetOutApplicationLogValidFlag(FALSE);					//Log.txtを出力しない
	ChangeWindowMode(TRUE);									//ウィンドウモードに設定
	SetMainWindowText(GAME_TITLE);							//ウィンドウのタイトルの文字
	SetGraphMode(GAME_WIDTH, GAME_HEIGHT, GAME_COLOR);		//ウィンドウの解像度を設定
	SetWindowSize(GAME_WIDTH, GAME_HEIGHT);					//ウィンドウの大きさを設定
	SetBackgroundColor(255, 255, 255);						//デフォルトの背景の色
	SetWindowIconID(GAME_ICON_ID);							//アイコンファイルを読み込み
	SetWindowStyleMode(GAME_WINDOW_BAR);					//ウィンドウバーの状態
	SetWaitVSyncFlag(TRUE);									//ディスプレイの垂直同期を有効にする
	SetAlwaysRunFlag(TRUE);									//ウィンドウをずっとアクティブにする


	if (DxLib_Init() == -1)		// ＤＸライブラリ初期化処理
	{
		return -1;				// エラーが起きたら直ちに終了
	}
	//ダブルバッファリング有効化
	SetDrawScreen(DX_SCREEN_BACK);

	//円の中心点
	int X = GAME_WIDTH / 2;
	int Y = GAME_HEIGHT / 2;
	//円の半径
	int radius = 60;

	int speed = 10;
	//無限ループ
	while (1)
	{

		if (ProcessMessage() != 0) { break; }	//メッセージを受け取り続ける
		if (ClearDrawScreen() != 0) { break; }	//画面を削除する

		//キーボード入力の更新
		AllKeyUpdate();
		//キー入力
		if (KeyDown(KEY_INPUT_W) == TRUE)
		{
			Y--;		//上に移動
			if (KeyDown(KEY_INPUT_LSHIFT) == TRUE || KeyDown(KEY_INPUT_RSHIFT) == TRUE)
			{
				Y -= speed;
			}
		}
		if (KeyDown(KEY_INPUT_S) == TRUE)
		{
			Y++;		//下に移動
			if (KeyDown(KEY_INPUT_LSHIFT) == TRUE || KeyDown(KEY_INPUT_RSHIFT) == TRUE)
			{
				Y += speed;
			}
		}
		if (KeyDown(KEY_INPUT_A) == TRUE)
		{
			X--;		//左に移動
			if (KeyDown(KEY_INPUT_LSHIFT) == TRUE || KeyDown(KEY_INPUT_RSHIFT) == TRUE)
			{
				X -= speed;
			}
		}
		if (KeyDown(KEY_INPUT_D) == TRUE)
		{
			X++;		//右に移動
			if (KeyDown(KEY_INPUT_LSHIFT) == TRUE || KeyDown(KEY_INPUT_RSHIFT) == TRUE)
			{
				X += speed;
			}
		}

		DrawCircle(X, Y, radius, GetColor(255, 255, 0), TRUE);
		ScreenFlip();		//ダブルバッファリングした画面を描画
	}
	// ＤＸライブラリ使用の終了処理
		DxLib_End();

		return 0;				// ソフトの終了 
}