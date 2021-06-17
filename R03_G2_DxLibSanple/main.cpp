
//ヘッダファイル読み込み
#include "game.h"		//ゲーム全体のヘッダファイル
#include "keyboard.h"	//キーボードの処理
#include "FPS.h"		//FPSの処理

//構造体の定義

//画像の構造体
struct IMAGE
{
	int handle = -1;	//画像のハンドル(管理番号)
	char path[255];		//画像の場所(パス)

	int x;				//X位置
	int y;				//Y位置
	int width;			//幅
	int height;			//高さ

	BOOL IsDraw = FALSE;//画像が描画できる？
};


//キャラクタの構造体
struct CHARACTOR
{
	IMAGE img;			//画像構造体
	int speed = 1;		//移動速度
	RECT coll;			//当たり判定の領域（四角）
};

//動画の構造体
struct MOVIE
{
	int handle = -1;	//動画のハンドル
	char path[255];		//動画の場所

	int x;				//X位置
	int y;				//Y位置
	int width;			//幅
	int height;			//高さ

	int Volume;			//ボリューム(最小)0～255(最大)
};

//音楽の構造体
struct AUDIO
{
	int handle = -1;		//音楽のハンドル
	char path[255];			//音楽のパス

	int Volume = -1;		//ボリューム(最小)0～255(最大)
	int playType = -1;		
};

//グローバル変数
//シーンを管理する変数
GAME_SCENE GameScene;		//現在のゲームのシーン
GAME_SCENE OldGameScene;	//前回のゲームのシーン
GAME_SCENE NextGameScene;	//次のゲームのシーン

//プレイ背景の動画
MOVIE playMovie;

//プレイヤー
CHARACTOR Player;

//敵
CHARACTOR Enemy;
//ゴール
CHARACTOR Goal;

//画像を読み込む
IMAGE TitleLogo;		//タイトルロゴ
IMAGE TitleEnter;		//エンターキーを押してね
IMAGE EndClear;			//クリアロゴ
IMAGE GameOverLogo;			//ゲームオーバーロゴ
IMAGE title;
IMAGE gameover_haikei;

//音楽
AUDIO TitleBGM;
AUDIO PlayBGM;
AUDIO ClearBGM;
AUDIO GameoverBGM;
AUDIO EndBGM;
AUDIO BattleBGM;

//効果音
AUDIO PlayerSE;



//画面の切り替え
BOOL IsFadeOut = FALSE;		//フェードアウト
BOOL IsFadeIn = FALSE;		//フェードイン

int fadeTimeMill = 2000;					//切り替えミリ秒
int fadeTimeMax = fadeTimeMill / 1000 * 60;	//ミリ秒をフレーム秒に変換

//フェードアウト
int fadeOutCntInit = 0;				//初期値
int fadeOutCnt = fadeOutCntInit;	//フェードアウトのカウンタ
int fadeOutCntMax = fadeTimeMax;	//フェードアウトのカウンタMAX

//フェードイン
int fadeInCntInit = fadeTimeMax;	//初期値
int fadeInCnt = fadeInCntInit;		//フェードアウトのカウンタ
int fadeInCntMax = fadeTimeMax;		//フェードアウトのカウンタMAX

//PushEnterの点滅
int PushEnterCnt = 0;			//カウンタ
const int PushEnterCntMAX = 60;		//カウンタMAX値
BOOL PushEnterBrink = FALSE;	//点滅してるか？
	
//プロトタイプ宣言
VOID Title(VOID);		//タイトル画面
VOID TitleProc(VOID);	//タイトル画面(処理)
VOID TitleDraw(VOID);	//タイトル画面(描画)

VOID Play(VOID);		//プレイ画面
VOID PlayProc(VOID);	//プレイ画面(処理)
VOID PlayDraw(VOID);	//プレイ画面(描画)

//VOID Battle(VOID);		//バトル画面
//VOID BattleProc(VOID);	//バトル画面(処理)
//VOID BattleDraw(VOID);	//バトル画面(描画)

VOID Clear(VOID);			//クリア画面
VOID ClearProc(VOID);		//クリア画面(処理)
VOID ClearDraw(VOID);		//クリア画面(描画)

VOID Gameover(VOID);			//ゲームオーバー画面
VOID GameoverProc(VOID);		//ゲームオーバー画面(処理)
VOID GameoverDraw(VOID);		//ゲームオーバー画面(描画)

//VOID End(VOID);			//エンド画面
//VOID EndProc(VOID);		//エンド画面(処理)
//VOID EndDraw(VOID);		//エンド画面(描画)

VOID Change(VOID);		//切り替え画面
VOID ChangeProc(VOID);	//切り替え画面(処理)
VOID ChangeDraw(VOID);	//切り替え画面(描画)

VOID ChangeScene(GAME_SCENE scene);	//シーン切り替え

VOID CollUpdatePlayer(CHARACTOR* chara);	//当たり判定の領域を更新
VOID CollUpdateEnemy(CHARACTOR* chara);
VOID CollUpdate(CHARACTOR* chara);
BOOL OncollRect(RECT Player, RECT Goal);	//矩形と矩形の当たり判定

BOOL GameLoad(VOID);		//ゲームのデータを読み込み

BOOL LoadAudio(AUDIO* audio, const char* path, int volume, int playType);	//ゲームの音楽を読み込み
BOOL LoadImageMem(IMAGE* image, const char* path);								//ゲーム画像読み込み

VOID GameInit(VOID);		//ゲームのデータを初期化

// プログラムは WinMain から始まります
//Windowsのプログラミング方法 = (WinAPI)で動いている！
//DxLibは、DirectXという、ゲームプログラミングをかんたんに使える仕組み
int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	SetOutApplicationLogValidFlag(FALSE);				//Log.txtを出力しない
	ChangeWindowMode(TRUE);								//ウィンドウモードに設定
	SetMainWindowText(GAME_TITLE);						//ウィンドウのタイトルの文字
	SetGraphMode(GAME_WIDTH, GAME_HEIGHT, GAME_COLOR);	//ウィンドウの解像度を設定
	SetWindowSize(GAME_WIDTH, GAME_HEIGHT);				//ウィンドウの大きさを設定
	SetBackgroundColor(255, 255, 255);					//デフォルトの背景の色
	SetWindowIconID(GAME_ICON_ID);						//アイコンファイルを読込
	SetWindowStyleMode(GAME_WINDOW_BAR);				//ウィンドウバーの状態
	SetWaitVSyncFlag(TRUE);								//ディスプレイの垂直同期を有効にする
	SetAlwaysRunFlag(TRUE);								//ウィンドウをずっとアクティブにする

	//ＤＸライブラリ初期化処理
	if (DxLib_Init() == -1)
	{
		// エラーが起きたら直ちに終了
		return -1;
	}

	//ダブルバッファリング有効化
	SetDrawScreen(DX_SCREEN_BACK);

	//最初のシーンは、タイトル画面から
	GameScene = GAME_SCENE_TITLE;

	//ゲーム全体の初期化

	//ゲーム読み込み
	if (!GameLoad())
	{
		//データを読み込みに失敗したとき
		DxLib_End();	//DxLib終了
		return -1;		//異常終了
	}

	//ゲームの初期化
	GameInit();


	//無限ループ
	while (1)
	{
		if (ProcessMessage() != 0) { break; }	//メッセージを受け取り続ける
		if (ClearDrawScreen() != 0) { break; }	//画面を消去する

		//キーボード入力の更新
		AllKeyUpdate();

		//FPS値の更新
		FPSUpdate();

		//ESCキーで強制終了
		if (KeyClick(KEY_INPUT_ESCAPE) == TRUE) { break; }

		//以前のシーンを取得
		if (GameScene != GAME_SCENE_CHANGE)
		{
			OldGameScene = GameScene;
		}

		//シーンごとに処理を行う
		switch (GameScene)
		{
		case GAME_SCENE_TITLE:
			Title();			//タイトル画面
			break;
		case GAME_SCENE_PLAY:
			Play();				//プレイ画面
			break;
		//case GAME_SCENE_BATTLE:
			//Battle();				//バトル画面
			//break;
		case GAME_SCENE_CLEAR:
			Clear();				//クリア画面
			break;
		case GAME_SCENE_GAMEOVER:
			Gameover();				//ゲームオーバー画面
			break;
		//case GAME_SCENE_END:
			//End();				//エンド画面
			//break;
		case GAME_SCENE_CHANGE:
			Change();			//切り替え画面
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

		//FPS値を描画
		FPSDraw();

		//FPS値を待つ
		FPSWait();

		ScreenFlip();	//ダブルバッファリングした画面を描画
	}

	//終わるときの処理
	DeleteGraph(playMovie.handle);	//画像をメモリ上から削除
	DeleteGraph(Player.img.handle);	//画像をメモリ上から削除
	DeleteGraph(Goal.img.handle);	//画像をメモリ上から削除
	DeleteGraph(title.handle);	//画像をメモリ上から削除
	DeleteGraph(gameover_haikei.handle);	//画像をメモリ上から削除

	DeleteGraph(TitleLogo.handle);		//画像をメモリ上から削除
	DeleteGraph(TitleEnter.handle);		//画像をメモリ上から削除
	DeleteGraph(EndClear.handle);		//画像をメモリ上から削除
	DeleteGraph(GameOverLogo.handle);		//画像をメモリ上から削除

	DeleteSoundMem(TitleBGM.handle);	//音楽をメモリ上から削除
	DeleteSoundMem(PlayBGM.handle);	//音楽をメモリ上から削除
	DeleteSoundMem(ClearBGM.handle);	//音楽をメモリ上から削除
	DeleteSoundMem(GameoverBGM.handle);	//音楽をメモリ上から削除
	//DeleteSoundMem(BattleBGM.handle);	//音楽をメモリ上から削除
	//DeleteSoundMem(EndBGM.handle);	//音楽をメモリ上から削除
	DeleteSoundMem(PlayerSE.handle);	//効果音をメモリ上から削除


	//ＤＸライブラリ使用の終了処理
	DxLib_End();

	return 0;	// ソフトの終了 
}

/// <summary>
/// ゲームデータを読み込み
/// </summary>
/// <param name="chara">読み込めたらTRUE / 読み込めなかったらFALSE</param>
BOOL GameLoad()
{
	//プレイ動画の背景を読み込み
	strcpyDx(playMovie.path, ".\\Movie\\playMovie.mp4");	//パスのコピー
	playMovie.handle = LoadGraph(playMovie.path);	//画像の読み込み

	//画像が読み込めなかったときは、エラー(-1)が入る
	if (playMovie.handle == -1)
	{
		MessageBox(
			GetMainWindowHandle(),	//メインのウィンドウハンドル
			playMovie.path,			//メッセージ本文
			"画像読み込みエラー！",		//メッセージタイトル
			MB_OK					//ボタン
		);

		return FALSE;		//読み込み失敗
	}

	//画像の幅と高さを取得
	GetGraphSize(playMovie.handle, &playMovie.width, &playMovie.height);

	//動画のボリューム
	playMovie.Volume = 255;

	//画像を読み込む
	if (!LoadImageMem(&Player.img, ".\\image\\Player.png")) { return FALSE; }
	if (!LoadImageMem(&Goal.img, ".\\image\\Goal.png")) { return FALSE; }
	if (!LoadImageMem(&Enemy.img, ".\\image\\Enemy.png")) { return FALSE; }
	if (!LoadImageMem(&title, ".\\image\\Title_Haikei.jpg")) { return FALSE; }
	if (!LoadImageMem(&gameover_haikei, ".\\image\\GameOver_Haikei.jpg")) { return FALSE; }

	//ロゴを読み込む
	if (!LoadImageMem(&TitleLogo, ".\\image\\Logo.png")) { return FALSE; }
	if (!LoadImageMem(&TitleEnter, ".\\image\\PushEnter.png")) { return FALSE; }
	if (!LoadImageMem(&EndClear, ".\\image\\Clear.png")) { return FALSE; }
	if (!LoadImageMem(&GameOverLogo, ".\\image\\GameOver.png")) { return FALSE; }

	//音楽を読み込む
	if (!LoadAudio(&TitleBGM, ".\\Audio\\TitleBGM.mp3", 255, DX_PLAYTYPE_LOOP)) { return FALSE; }
	if (!LoadAudio(&PlayBGM, ".\\Audio\\PlayBGM.mp3", 255, DX_PLAYTYPE_LOOP)) { return FALSE; }
	if (!LoadAudio(&ClearBGM, ".\\Audio\\ClearBGM.mp3", 255, DX_PLAYTYPE_LOOP)) { return FALSE; }
	if (!LoadAudio(&GameoverBGM, ".\\Audio\\GameoverBGM.mp3", 255, DX_PLAYTYPE_LOOP)) { return FALSE; }
	//if (!LoadAudio(&EndBGM, ".\\Audio\\EndBGM.mp3", 255, DX_PLAYTYPE_LOOP)) { return FALSE; }
	//if (!LoadAudio(&BattleBGM, ".\\Audio\\BattleBGM.mp3", 255, DX_PLAYTYPE_LOOP)) { return FALSE; }
	if (!LoadAudio(&PlayerSE, ".\\Audio\\PlayerSE.mp3", 255, DX_PLAYTYPE_BACK)) { return FALSE; }
	

	return TRUE;	//全て読み込みた！
}



/// <summary>
/// 画像をメモリに読み込み
/// </summary>
/// <param name="image">画像構造体のアドレス</param>
/// <param name="path">画像のパス</param>
/// <returns></returns>
BOOL LoadImageMem(IMAGE* image, const char* path)
{
	//プレイヤーの画像を読み込み
	strcpyDx(image->path, path);	//パスのコピー
	image->handle = LoadGraph(image->path);	//画像の読み込み

	//画像が読み込めなかったときは、エラー(-1)が入る
	if (image->handle == -1)
	{
		MessageBox(
			GetMainWindowHandle(),	//メインのウィンドウハンドル
			image->path,			//メッセージ本文
			"画像読み込みエラー！",		//メッセージタイトル
			MB_OK					//ボタン
		);

		return FALSE;		//読み込み失敗
	}

	GetGraphSize(image->handle, &image->width, &image->height);

	return TRUE;  //よみこめた
}
/// <summary>
/// 音楽をメモリに読み込み
/// </summary>
/// <param name="audio">Audio構造体変数のアドレス</param>
/// <param name="path">Audioの音楽パス</param>
/// <param name="volume">ボリューム</param>
/// <param name="playType">DX_PLAYTYPE_LOOP or DX_PLAYTYPE_BACK</param>
/// <returns></returns>
BOOL LoadAudio(AUDIO* audio, const char* path, int volume, int playType)
{
	//音楽の読み込み
	strcpyDx(audio->path, path);					//パスのコピー
	audio->handle = LoadSoundMem(audio->path);		//音楽の読み込み

	//音楽が読み込めなかったときは、エラー(-1)が入る
	if (audio->handle == -1)
	{
		MessageBox(
			GetMainWindowHandle(),	//メインのウィンドウハンドル
			audio->path,			//メッセージ本文
			"音楽読み込みエラー！",		//メッセージタイトル
			MB_OK					//ボタン
		);

		return FALSE;	//読み込み失敗
	}

	//その他の設定
	audio->Volume = volume;
	audio->playType = playType;

	return TRUE;
}

/// <summary>
/// ゲームデータを初期化
/// </summary>
/// <param name="chara"></param>
VOID GameInit(VOID)
{
	//敵を初期化
	Enemy.img.x = GAME_WIDTH - Enemy.img.width;
	Enemy.img.y = GAME_HEIGHT - Enemy.img.height;
	//Enemy.speed = 500;		//スピード
	Enemy.img.IsDraw = TRUE;	//描画できる！

	//当たり判定を更新する
	CollUpdateEnemy(&Enemy);	//プレイヤーの当たり判定のアドレス

	//プレイヤーを初期化
	Player.img.x = GAME_WIDTH / 2 - Player.img.width / 2;	//中央寄せ
	Player.img.y = GAME_HEIGHT / 2 - Player.img.height / 2;	//中央寄せ
	Player.speed = 500;		//スピード
	Player.img.IsDraw = TRUE;	//描画できる！

	//当たり判定を更新する
	CollUpdatePlayer(&Player);	//プレイヤーの当たり判定のアドレス

	//ゴールを初期化
	Goal.img.x = GAME_WIDTH - Goal.img.width;
	Goal.img.y = 0;
	Goal.speed = 500;		//スピード
	Goal.img.IsDraw = TRUE;	//描画できる！

	//当たり判定を更新する
	CollUpdate(&Goal);	//ゴールの当たり判定のアドレス

	//タイトルロゴの位置を決める
	TitleLogo.x = GAME_WIDTH / 2 - TitleLogo.width / 2;	//中央揃え
	TitleLogo.y = 0;
	
	//PushEnterの位置を決める
	TitleEnter.x = GAME_WIDTH / 2 - TitleLogo.width / 2;
	TitleEnter.y = GAME_HEIGHT / 2 - TitleEnter.height + 250;

	//PushEnterの点滅
	PushEnterCnt = 0;
	//PushEnterCntMAX = 60;
	PushEnterBrink = FALSE;

	//クリアロゴの位置を決める
	EndClear.x = GAME_WIDTH / 2 - TitleLogo.width / 2;
	EndClear.y = GAME_HEIGHT / 2 - TitleEnter.height / 2;
}
/// <summary>
/// シーンを切り替える関数
/// </summary>
/// <param name="scene">シーン</param>
VOID ChangeScene(GAME_SCENE scene)
{
	GameScene = scene;	//シーンを切り替え
	IsFadeIn = FALSE;	//フェードインしない
	IsFadeOut = TRUE;	//フェードアウトする

	return;
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
		//BGMを止める
		StopSoundMem(TitleBGM.handle);

		//シーン切り替え
		//次のシーンの初期化をここで行うと楽

		//ゲームの初期化
		GameInit();

		//プレイ画面に切り替え
		ChangeScene(GAME_SCENE_PLAY);

		return;
	}

	//BGMが流れてないとき
	if (CheckSoundMem(TitleBGM.handle) == 0)
	{
		//BGMを流す
		PlaySoundMem(TitleBGM.handle, TitleBGM.playType);
	}

	return;
}

/// <summary>
/// タイトル画面の描画
/// </summary>
VOID TitleDraw(VOID)
{
	DrawGraph(title.x, title.y, title.handle, TRUE);
	DrawGraph(TitleLogo.x, TitleLogo.y, TitleLogo.handle, TRUE);
	

	//MAX値まで待つ
	if (PushEnterCnt < PushEnterCntMAX) { PushEnterCnt++; }
	else
	{
		if (PushEnterBrink == TRUE)PushEnterBrink = FALSE;
		else if(PushEnterBrink == FALSE)PushEnterBrink = TRUE;

		PushEnterCnt = 0;	//カウンタ初期化
	}
	if (PushEnterBrink == TRUE)
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, ((float)PushEnterCnt / PushEnterCntMAX) * 255);

		DrawGraph(TitleEnter.x, TitleEnter.y, TitleEnter.handle, TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

	if (PushEnterBrink == FALSE)
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, ((float)PushEnterCntMAX - PushEnterCnt / PushEnterCntMAX) * 255);

		DrawGraph(TitleEnter.x, TitleEnter.y, TitleEnter.handle, TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
	
	DrawString(0, 0, "タイトル画面", GetColor(0, 0, 0));
	DrawString(GAME_WIDTH - 210,GAME_HEIGHT - 120, "○操作方法 \n・↑キーで上に進む \n・←キーで左に進む \n・↓キーで下に進む \n・→キーで右に進む", GetColor(0, 0, 0));
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
	/*if (KeyClick(KEY_INPUT_RETURN) == TRUE)
	{
		//シーン切り替え
		//次のシーンの初期化をここで行うと楽

	

		//エンド画面に切り替え
		ChangeScene(GAME_SCENE_END);
	}*/

	//BGMが流れてないとき
	if (CheckSoundMem(PlayBGM.handle) == 0)
	{
		//BGMを流す
		PlaySoundMem(PlayBGM.handle, PlayBGM.playType);
	}


	

	//プレイヤーの操作
	if (KeyDown(KEY_INPUT_UP) == TRUE) 
	{
		Player.img.y -= Player.speed * fps.DeltaTime;

		//効果音が流れてないとき
		if (CheckSoundMem(PlayerSE.handle) == 0)
		{
		//動く時の効果音を追加
		PlaySoundMem(PlayerSE.handle, PlayerSE.playType);
		}

	}
	if (KeyDown(KEY_INPUT_DOWN) == TRUE)
	{
		Player.img.y += Player.speed * fps.DeltaTime;

		//効果音が流れてないとき
		if (CheckSoundMem(PlayerSE.handle) == 0)
		{
			//動く時の効果音を追加
			PlaySoundMem(PlayerSE.handle, PlayerSE.playType);
		}
		
	}
	if (KeyDown(KEY_INPUT_LEFT) == TRUE)
	{
		Player.img.x -= Player.speed * fps.DeltaTime;
		
		//効果音が流れてないとき
		if (CheckSoundMem(PlayerSE.handle) == 0)
		{
			//動く時の効果音を追加
			PlaySoundMem(PlayerSE.handle, PlayerSE.playType);
		}
		
	}
	if (KeyDown(KEY_INPUT_RIGHT) == TRUE)
	{
		Player.img.x += Player.speed * fps.DeltaTime;

		//効果音が流れてないとき
		if (CheckSoundMem(PlayerSE.handle) == 0)
		{
			//動く時の効果音を追加
			PlaySoundMem(PlayerSE.handle, PlayerSE.playType);
		}

	}
	/*
	//敵の操作
	if (KeyDown(KEY_INPUT_W) == TRUE)
	{
		Enemy.img.y -= Enemy.speed * fps.DeltaTime;
	}
	if (KeyDown(KEY_INPUT_S) == TRUE)
	{
		Enemy.img.y += Enemy.speed * fps.DeltaTime;
	}
	if (KeyDown(KEY_INPUT_A) == TRUE)
	{
		Enemy.img.x -= Enemy.speed * fps.DeltaTime;
	}
	if (KeyDown(KEY_INPUT_D) == TRUE)
	{
		Enemy.img.x += Enemy.speed * fps.DeltaTime;
	}
	*/
	//当たり判定を更新する
	CollUpdatePlayer(&Player);

	//ゴールの当たり判定を更新する
	CollUpdate(&Goal);

	//当たり判定を更新する
	CollUpdateEnemy(&Enemy);

	//プレイヤーがゴールに当たった時は
	if (OncollRect(Player.coll, Goal.coll) == TRUE)
	{
		//BGMを止める
		StopSoundMem(PlayBGM.handle);

		//エンド画面に切り替え
		ChangeScene(GAME_SCENE_CLEAR);
		return;
	}

	//プレイヤーがエネミーに当たった時は
	if (OncollRect(Enemy.coll,Player.coll) == TRUE)
	{
		//BGMを止める
		StopSoundMem(PlayBGM.handle);

		//エンド画面に切り替え
		ChangeScene(GAME_SCENE_GAMEOVER);

		return;
	}

	//プレイヤーがエネミーに当たった時は
	
	//if (OncollRect(Player.coll, Enemy.coll) == TRUE)
	//{
		//BGMを止める
		//StopSoundMem(PlayBGM.handle);
		//バトル画面に切り替え
		//ChangeScene(GAME_SCENE_BATTLE);
		//return;
	//}

	return;
}

/// <summary>
/// プレイ画面の描画
/// </summary>
VOID PlayDraw(VOID)
{
	//背景動画を描画

	//もし、動画が再生されてないとき
	if (GetMovieStateToGraph(playMovie.handle) == 0)
	{
		//再生する
		SeekMovieToGraph(playMovie.handle, 0);	//シークバーを最初に戻す
		PlayMovieToGraph(playMovie.handle);		//動画を再生
	}

	//動画を描画(画面に合わせて画像を引き延ばす)
	DrawExtendGraph(0, 0, GAME_WIDTH, GAME_HEIGHT, playMovie.handle, TRUE);
	//プレイヤーを描画
	if (Player.img.IsDraw == TRUE)
	{
		//画像を描画
		DrawGraph(Player.img.x, Player.img.y, Player.img.handle,TRUE);
	
		//デバッグのときは、当たり判定の領域を描画
		if (GAME_DEBUG == TRUE)
		{
			//四角を描画
			DrawBox(Player.coll.left, Player.coll.top, Player.coll.right, Player.coll.bottom,
				GetColor(255, 0, 0), FALSE);
		}

	}

	//ゴールを描画
	if (Enemy.img.IsDraw == TRUE)
	{
		//画像を描画
		DrawGraph(Enemy.img.x, Enemy.img.y, Enemy.img.handle, TRUE);

		//デバッグのときは、当たり判定の領域を描画
		if (GAME_DEBUG == TRUE)
		{
			//四角を描画
			DrawBox(Enemy.coll.left, Enemy.coll.top, Enemy.coll.right, Enemy.coll.bottom,
				GetColor(255, 0, 0), FALSE);
		}
	}

	//ゴールを描画
	if (Goal.img.IsDraw == TRUE)
	{
		//画像を描画
		DrawGraph(Goal.img.x, Goal.img.y, Goal.img.handle, TRUE);

		//デバッグのときは、当たり判定の領域を描画
		if (GAME_DEBUG == TRUE)
		{
			//四角を描画
			DrawBox(Goal.coll.left, Goal.coll.top, Goal.coll.right, Goal.coll.bottom,
				GetColor(255, 0, 0), FALSE);
		}
	}

	
	DrawString(0, 0, "プレイ画面", GetColor(0, 0, 0));
	return;
}
/*
/// <summary>
/// バトル画面
/// </summary>
VOID Battle(VOID)
{
	BattleProc();	//処理
	BattleDraw();	//描画

	return;
}

/// <summary>
/// バトル画面の処理
/// </summary>
VOID BattleProc(VOID)
{

	if (KeyClick(KEY_INPUT_RETURN) == TRUE)
	{
		//BGMを止める
		StopSoundMem(BattleBGM.handle);
		//シーン切り替え
		//次のシーンの初期化をここで行うと楽
		
		//ゲームの初期化
		GameInit();
		//プレイ画面に切り替え
		ChangeScene(GAME_SCENE_PLAY);
		return;
	}
		//BGMが流れてないとき
	if (CheckSoundMem(EndBGM.handle) == 0)
	{
		//BGMを流す
		PlaySoundMem(EndBGM.handle, EndBGM.playType);
	}
	return;
}

/// <summary>
/// バトル画面の描画
/// </summary>
VOID BattleDraw(VOID)
{

	DrawString(0, 0, "バトル画面", GetColor(0, 0, 0));
	return;
}
*/
/*
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

		//BGMを止める
		StopSoundMem(EndBGM.handle);

		//シーン切り替え
		//次のシーンの初期化をここで行うと楽

		//タイトル画面に切り替え
		ChangeScene(GAME_SCENE_TITLE);

		return;
	}

	//BGMが流れてないとき
	if (CheckSoundMem(EndBGM.handle) == 0)
	{
		//BGMを流す
		PlaySoundMem(EndBGM.handle, EndBGM.playType);
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
*/
/// <summary>
/// クリア画面
/// </summary>
VOID Clear(VOID)
{
	ClearProc();	//処理
	ClearDraw();	//描画

	return;
}

/// <summary>
/// クリア画面の処理
/// </summary>
VOID ClearProc(VOID)
{
	if (KeyClick(KEY_INPUT_RETURN) == TRUE)
	{
		//BGMを止める
		StopSoundMem(ClearBGM.handle);
		//シーン切り替え
		//次のシーンの初期化をここで行うと楽

		//エンド画面に切り替え
		ChangeScene(GAME_SCENE_TITLE);

		return;
	}
	//BGMが流れてないとき
	if (CheckSoundMem(ClearBGM.handle) == 0)
	{
		//BGMを流す
		PlaySoundMem(ClearBGM.handle, ClearBGM.playType);
	}

	return;
}

/// <summary>
/// クリア画面の描画
/// </summary>
VOID ClearDraw(VOID)
{


	DrawGraph(EndClear.x, EndClear.y, EndClear.handle, TRUE);
	DrawString(0, 0, "クリア画面", GetColor(0, 0, 0));
	
	return;
}

/// <summary>
/// ゲームオーバー画面
/// </summary>
VOID Gameover(VOID)
{
	GameoverProc();	//処理
	GameoverDraw();	//描画

	return;
}

/// <summary>
/// ゲームオーバー画面の処理
/// </summary>
VOID GameoverProc(VOID)
{
	if (KeyClick(KEY_INPUT_RETURN) == TRUE)
	{
		//BGMを止める
		StopSoundMem(GameoverBGM.handle);
		//シーン切り替え
		//次のシーンの初期化をここで行うと楽

		//タイトル画面に切り替え
		ChangeScene(GAME_SCENE_TITLE);

		return;
	}

	//BGMが流れてないとき
	if (CheckSoundMem(GameoverBGM.handle) == 0)
	{
		//BGMを流す
		PlaySoundMem(GameoverBGM.handle, GameoverBGM.playType);
	}
	return;
}

/// <summary>
/// ゲームオーバー画面の描画
/// </summary>
VOID GameoverDraw(VOID)
{
	DrawGraph(gameover_haikei.x, gameover_haikei.y, gameover_haikei.handle, TRUE);
	DrawGraph(GameOverLogo.x, GameOverLogo.y, GameOverLogo.handle, TRUE);
	DrawString(0, 0, "ゲームオーバー画面", GetColor(0, 0, 0));
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
	if (IsFadeIn == TRUE)
	{
		if (fadeInCnt > fadeInCntMax)
		{
			fadeInCnt--;	//カウンタを減らす
		}
		else
		{
			//フェードイン処理が終わった

			fadeInCnt = fadeInCntInit;	//カウンタ初期化
			IsFadeIn = FALSE;			//フェードイン処理終了
		}
	}

	//フェードアウト
	if (IsFadeOut == TRUE)
	{
		if (fadeOutCnt < fadeOutCntMax)
		{
			fadeOutCnt++;	//カウンタを増やす
		}
		else
		{
			//フェードアウト処理が終わった

			fadeOutCnt = fadeOutCntInit;	//カウンタ初期化
			IsFadeOut = FALSE;				//フェードアウト処理終了
		}
	}

	//切り替え処理終了
	if (IsFadeIn == FALSE && IsFadeOut == FALSE)
	{
		//フェードインしていない、フェードアウトもしていないとき
		GameScene = NextGameScene;	//次のシーンに切り替え
		OldGameScene = GameScene;	//以前のゲームシーン更新
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
		TitleDraw();	//タイトル画面の描画
		break;
	case GAME_SCENE_PLAY:
		PlayDraw();		//プレイ画面の描画
		break;
	
	//case GAME_SCENE_BATTLE:
		//BattleDraw();		//バトル画面の描画
		//break;
	case GAME_SCENE_CLEAR:
		ClearDraw();		//エンド画面の描画
		break;
	case GAME_SCENE_GAMEOVER:
		GameoverDraw();		//ゲームオーバー画面の描画
		break;
	//case GAME_SCENE_END:
		//EndDraw();		//エンド画面の描画
		//break;
	default:
		break;
	}

	//フェードイン
	if (IsFadeIn == TRUE)
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, ((float)fadeInCnt / fadeInCntMax) * 255);
	}

	//フェードアウト
	if (IsFadeOut == TRUE)
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, ((float)fadeOutCnt / fadeOutCntMax) * 255);
	}

	//四角を描画
	DrawBox(0, 0, GAME_WIDTH, GAME_HEIGHT, GetColor(0, 0, 0), TRUE);

	//半透明終了
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	DrawString(0, 0, "切り替え画面", GetColor(0, 0, 0));
	return;
}

/// <summary>
/// 当たり判定の領域更新
/// </summary>
/// <param name="chara">当たり判定の領域</param>
VOID CollUpdatePlayer(CHARACTOR* chara)
{
	chara->coll.left = chara->img.x;
	chara->coll.top = chara->img.y;
	chara->coll.right = chara->img.x + chara->img.width;
	chara->coll.bottom = chara->img.y + chara->img.height;

	return;
}

/// <summary>
/// 当たり判定の領域更新
/// </summary>
/// <param name="chara">当たり判定の領域</param>
VOID CollUpdateEnemy(CHARACTOR* chara)
{
	chara->coll.left = chara->img.x;
	chara->coll.top = chara->img.y;
	chara->coll.right = chara->img.x + chara->img.width;
	chara->coll.bottom = chara->img.y + chara->img.height;

	return;
}

/// <summary>
/// 当たり判定の領域更新
/// </summary>
/// <param name="chara">当たり判定の領域</param>
VOID CollUpdate(CHARACTOR* chara)
{
	chara->coll.left = chara->img.x;
	chara->coll.top = chara->img.y;
	chara->coll.right = chara->img.x + chara->img.width;
	chara->coll.bottom = chara->img.y + chara->img.height;

	return;
}

/// <summary>
/// 矩形と矩形の当たり判定
/// </summary>
/// <param name="chara"></param>
BOOL OncollRect(RECT a, RECT b)
{
	if (a.left < b.right &&		//矩形Aの左辺x座標 < 矩形Bの右辺x座標 かつ
		a.right > b.left &&		//矩形Aの右辺x座標 < 矩形Bの左辺x座標 かつ
		a.top < b.bottom &&		//矩形Aの左辺y座標 < 矩形Bの右辺y座標 かつ
		a.bottom > b.top)		//矩形Aの右辺y座標 < 矩形Bの左辺y座標
	{
		//当たっているとき
		return TRUE;
	}
	else
	{
		//当たってないとき
		return FALSE;
	}
	
}
