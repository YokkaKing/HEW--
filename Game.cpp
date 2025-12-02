
//Game.cpp

#include	"Manager.h"
#include	"sprite.h"
#include	"Game.h"
#include	"keyboard.h"
#include	"Controller.h"

#include	"field.h"
#include	"Effect.h"
#include	"Audio.h"

#include	"Camera.h"
#include "fade.h"
#include "Player.h"
#include "Player2.h"
#include "managerCollider.h"

#include	"direct3d.h"//<<<<<<<<<<<<<<<<<<<

LIGHTOBJECT		Light;//<<<<<<ライト管理オブジェクト

static Controller g_PlayerController(0); // 0はプレイヤー1のインデックス
static Controller g_Player2Controller(1); // 0はプレイヤー1のインデックス

static	int		g_BgmID = NULL;	//サウンド管理ID

void Game_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	//Controller_Initialize();

	Field_Initialize(pDevice, pContext); // フィールドの初期化
	PlayerInitialize(pDevice, pContext); // ボールの初期化
	Player2Initialize(pDevice, pContext); // Player2の初期化

	Camera_Initialize();	//カメラ初期化



	g_BgmID = LoadAudio("asset\\Audio\\bgm.wav");	//サウンドロード
	PlayAudio(g_BgmID, true);	//再生開始（ループあり）
	//PlayAudio(g_BgmID);			//再生開始（ループなし）
	//PlayAudio(g_BgmID, false);	//再生開始（ループなし）

	//ライト初期化
	XMFLOAT4	para;

	para = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);//環境光の色
	Light.SetAmbient(para);

	para = XMFLOAT4(0.6f, 0.6f, 0.6f, 1.0f);//光の色
	Light.SetDiffuse(para);

	para = XMFLOAT4(0.5f, -1.0f, 0.0f, 1.0f);//光方向
	float	len = sqrtf(para.x * para.x + para.y * para.y + para.z * para.z);
	para.x /= len;
	para.y /= len;
	para.z /= len;
	Light.SetDirection(para);//光の方向（正規化済）

}

void Game_Finalize()
{
	Field_Finalize();	// フィールドの終了処理
	PlayerFinalize();	// プレイヤーの終了処理
	Player2Finalize();	// プレイヤー2の終了処理
	Camera_Finalize();	//カメラ終了処理

	UnloadAudio(g_BgmID);//サウンドの解放
}

void Game_Update()
{
	//毎フレーム、コントローラーの状態を更新する★
	g_PlayerController.UpdateState();
	g_Player2Controller.UpdateState(); // Player2コントローラーの状態を更新
	//更新処理
	PlayerUpdate();
	Player2Update(); // Player2の更新処理
	Field_Update();
	ManagerCollider::UpdateAllCollisions();
	Camera_Update();	//カメラ更新処理


}

void Game_Draw()
{ 
	Light.SetEnable(TRUE);			//ライティングON
	Shader_SetLight(Light.Light);	//ライト構造体をシェーダーへセット
	SetDepthTest(TRUE);

	Camera_Draw();		//Drawの最初で呼ぶ！
	Field_Draw();
	PlayerDraw();
	Player2Draw(); // Player2の描画処理

	//2D描画
	Light.SetEnable(FALSE);			//ライティングOFF
	Shader_SetLight(Light.Light);	//ライト構造体をシェーダーへセット
	SetDepthTest(FALSE);
}


// Controllerインスタンスを返すGetter関数
Controller* GetPlayerController()
{
	return &g_PlayerController;
}


Controller* GetPlayer2Controller()
{
	return &g_Player2Controller;
}
