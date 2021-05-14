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

//列挙型
enum GAME_SCENE {
	GAME_SCENE_TITLE,
	GAME_SCENE_PLAY,
	GAME_SCENE_END,
	GAME_SCENE_CHANGE
};		//ゲームのシーン

//グローバル変数
//シーンを管理する変数
GAME_SCENE GameScene;		//現在のゲームシーン
GAME_SCENE OldGameScene;	//前回のゲームシーン
GAME_SCENE NextGameScene;	//次のゲームシーン

//画面の切り替え
BOOL IsFadeOut = FALSE;		//フェードアウト
BOOL IsFadeIn = FALSE;		//フェードイン

int FadeTimeMill = 2000;						//切り替えミリ秒
int FadeTimeMax = FadeTimeMill / 1000 * 60;		//ミリ秒をフレーム秒に変換

//フェードアウト
int FadeOutCntInit = 0;				//初期値
int FadeOutCnt = FadeOutCntInit;	//フェードアウトのカウンタ
int FadeOutCntMax = FadeTimeMax;	//フェードアウトのカウンタMAX

//フェードイン
int FadeInCntInit = FadeTimeMax;	//初期値
int FadeInCnt = FadeInCntInit;		//フェードインのカウンタ
int FadeInCntMax = FadeTimeMax;		//フェードインのカウンタMAX

//プロトタイプ宣言
VOID Title(VOID);		//タイトル画面
VOID TitleProc(VOID);	//タイトル画面(処理)
VOID TitleDraw(VOID);	//タイトル画面(描画)

VOID Play(VOID);		//プレイ画面
VOID PlayProc(VOID);	//プレイ画面(処理)
VOID PlayDraw(VOID);	//プレイ画面(描画)

VOID End(VOID);		//エンド画面
VOID EndProc(VOID);	//エンド画面(処理)
VOID EndDraw(VOID);	//エンド画面(描画)

VOID Change(VOID);		//切り替え画面
VOID ChangeProc(VOID);	//切り替え画面(処理)
VOID ChangeDraw(VOID);	//切り替え画面(描画)

VOID ChangeScene(GAME_SCENE scene);	//シーン切り替え

// プログラムは WinMain から始まります
//Windowsのプログラミング方法=(WinAPI)で動いている！
//DxLibは、DirectXという、ゲームプログラミングをかんたんに使える仕組み
int WINAPI WinMain(
	HINSTANCE hInstance,
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
	int radius1 = 10;

	int speed = 8;

	//最初のシーンはタイトルから
	GameScene = GAME_SCENE_TITLE;
	//無限ループ
	while (1)
	{

		if (ProcessMessage() != 0) { break; }	//メッセージを受け取り続ける
		if (ClearDrawScreen() != 0) { break; }	//画面を削除する

		//キーボード入力の更新
		AllKeyUpdate();

		//ESCキーで強制終了
		if (KeyDown(KEY_INPUT_ESCAPE) == TRUE) { break; }

		//
		if (GameScene != GAME_SCENE_CHANGE)
		{
			OldGameScene = GameScene;
		}

		//シーンごとに処理を行う
		switch (GameScene)
		{
		case GAME_SCENE_TITLE:
			Title();
			break;
		case GAME_SCENE_PLAY:
			Play();
			break;
		case GAME_SCENE_END:
			End();
			break;
		case GAME_SCENE_CHANGE:
			Change();
			break;
		default:
			break;
		}

		//シーンを切り替える
		if (OldGameScene != GameScene)
		{
			//現在のシーンが切り替え画面でないとき
			if (GameScene != GAME_SCENE_CHANGE)
			{
				NextGameScene = GameScene;		//次のシーンを保存
				GameScene = GAME_SCENE_CHANGE;	//画面切り替えシーンに変える
			}
		}

		//キー入力
		if (KeyDown(KEY_INPUT_W) == TRUE || KeyDown(KEY_INPUT_UP) == TRUE)
		{
			Y--;		//上に移動
			if (KeyDown(KEY_INPUT_LSHIFT) == TRUE || KeyDown(KEY_INPUT_RSHIFT) == TRUE)
			{
				Y -= speed;
			}
		}
		if (KeyDown(KEY_INPUT_S) == TRUE || KeyDown(KEY_INPUT_DOWN) == TRUE)
		{
			Y++;		//下に移動
			if (KeyDown(KEY_INPUT_LSHIFT) == TRUE || KeyDown(KEY_INPUT_RSHIFT) == TRUE)
			{
				Y += speed;
			}
		}
		if (KeyDown(KEY_INPUT_A) == TRUE || KeyDown(KEY_INPUT_LEFT) == TRUE)
		{
			X--;		//左に移動
			if (KeyDown(KEY_INPUT_LSHIFT) == TRUE || KeyDown(KEY_INPUT_RSHIFT) == TRUE)
			{
				X -= speed;
			}
		}
		if (KeyDown(KEY_INPUT_D) == TRUE || KeyDown(KEY_INPUT_RIGHT) == TRUE)
		{
			X++;		//右に移動
			if (KeyDown(KEY_INPUT_LSHIFT) == TRUE || KeyDown(KEY_INPUT_RSHIFT) == TRUE)
			{
				X += speed;
			}
		}
		if (KeyClick(KEY_INPUT_SPACE) == TRUE)
		{
			DrawCircle(X, Y, radius1, GetColor(255, 0, 255), TRUE);
			ScreenFlip();
		}

		DrawCircle(X, Y, radius, GetColor(0, 255, 0), TRUE);
		ScreenFlip();		//ダブルバッファリングした画面を描画
	}
	// ＤＸライブラリ使用の終了処理
		DxLib_End();

		return 0;				// ソフトの終了 
}
/// <summary>
/// シーンを切り替える関数
/// </summary>
/// <param name="scene">シーン</param>
VOID ChangeScene(GAME_SCENE scene)
{
	GameScene = scene;				//画面へ切り替え
	IsFadeIn = FALSE;				//フェードインしない
	IsFadeOut = TRUE;				//フェードアウトする
}
/// <summary>
/// タイトル画面
/// </summary>
VOID Title(VOID)
{
	TitleProc();	//処理
	TitleDraw();	//描画

	return;
}

/// <summary>
/// タイトル画面の処理
/// </summary>
VOID TitleProc(VOID)
{
	
	if (KeyClick(KEY_INPUT_RETURN) == TRUE)
	{
		//シーンへ切り替え
		//次のシーンの初期化をここで行うと楽

		//プレイ画面に切り替え
		ChangeScene(GAME_SCENE_PLAY);
	}
	return;
}
/// <summary>
/// タイトル画面の描画
/// </summary>
VOID TitleDraw(VOID)
{
	DrawString(0, 0, "タイトル画面", GetColor(0, 0, 0));
	return;
}
/// <summary>
/// プレイ画面
/// </summary>
VOID Play(VOID)
{
	PlayProc();	//処理
	PlayDraw();	//描画

	return;
}

/// <summary>
/// プレイ画面の処理
/// </summary>
VOID PlayProc(VOID)
{
	if (KeyClick(KEY_INPUT_RETURN) == TRUE)
	{
		//シーンへ切り替え
		//次のシーンの初期化をここで行うと楽

		//エンド画面に切り替え
		ChangeScene(GAME_SCENE_END);
	}
	return;
}
/// <summary>
/// プレイ画面の描画
/// </summary>
VOID PlayDraw(VOID)
{
	DrawString(0, 0, "プレイ画面", GetColor(0, 0, 0));
	return;
}

/// <summary>
/// エンド画面
/// </summary>
VOID End(VOID)
{
	EndProc();	//処理
	EndDraw();	//描画

	return;
}

/// <summary>
/// エンド画面の処理
/// </summary>
VOID EndProc(VOID)
{
	if (KeyClick(KEY_INPUT_RETURN) == TRUE)
	{
		//シーンへ切り替え
		//次のシーンの初期化をここで行うと楽

		//タイトル画面に切り替え
		ChangeScene(GAME_SCENE_TITLE);
	}
	return;
}
/// <summary>
/// エンド画面の描画
/// </summary>
VOID EndDraw(VOID)
{
	DrawString(0, 0, "エンド画面", GetColor(0, 0, 0));
	return;
}

/// <summary>
/// 切り替え画面
/// </summary>
VOID Change(VOID)
{
	ChangeProc();	//処理
	ChangeDraw();	//描画

	return;
}

/// <summary>
/// 切り替え画面の処理
/// </summary>
VOID ChangeProc(VOID)
{
	//フェードイン
	if (IsFadeIn)
	{

		if (FadeInCnt > FadeInCntMax)
		{
			FadeInCnt--;		//カウンタを減らす
		}
		else
		{
			//フェードイン処理が終わった

			FadeInCnt = FadeInCntInit;	//カウンタ初期化
			IsFadeIn = FALSE;			//フェードイン処理終了
		}
	}
	//フェードアウト
	if (IsFadeOut)
	{

		if (FadeOutCnt > FadeOutCntMax)
		{
			FadeOutCnt--;		//カウンタを増やす
		}
		else
		{
			//フェードアウト処理が終わった

			FadeOutCnt = FadeOutCntInit;	//カウンタ初期化
			IsFadeOut = FALSE;			//フェードアウト処理終了
		}
	}

	//切り替え処理終了
	if (IsFadeIn == FALSE && IsFadeOut == FALSE)
	{
		//フェードインしてない、フェードアウトしてないとき
		GameScene = NextGameScene;		//次のシーンに切り替え
		OldGameScene = GameScene;		//以前のゲームシーン更新

	}
	return;
}
/// <summary>
/// 切り替え画面の描画
/// </summary>
VOID ChangeDraw(VOID)
{
	//以前のシーンを描画
	switch (OldGameScene)
	{
	case GAME_SCENE_TITLE:
		TitleDraw();		//タイトル画面の描画
		break;
	case GAME_SCENE_PLAY:
		PlayDraw();			//プレイ画面の描画
		break;
	case GAME_SCENE_END:
		EndDraw();			//エンド画面の描画
		break;
	default:
		break;
	}

	//フェードイン
	if (IsFadeIn == TRUE)
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, ((float)FadeInCnt / FadeInCntMax) * 255);
	}
	//フェードアウト
	if (IsFadeOut == TRUE)
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, ((float)FadeOutCnt / FadeOutCntMax) * 255);
	}
	//四角を描画
	DrawBox(0, 0, GAME_WIDTH, GAME_HEIGHT, GetColor(0, 0, 0), TRUE);

	//半透明終了
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	DrawString(0, 0, "切り替え画面", GetColor(0, 0, 0));
	return;
}