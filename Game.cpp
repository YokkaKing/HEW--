
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
#include "managerCollider.h"

#include	"direct3d.h"//<<<<<<<<<<<<<<<<<<<

LIGHTOBJECT		Light;//<<<<<<ライト管理オブジェクト


static	int		g_BgmID = NULL;	//サウンド管理ID

void Game_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	Controller_Initialize();

	Field_Initialize(pDevice, pContext); // フィールドの初期化
	PlayerInitialize(pDevice, pContext); // ボールの初期化

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
	PlayerFinalize();	// ボールの終了処理
	Camera_Finalize();	//カメラ終了処理

	UnloadAudio(g_BgmID);//サウンドの解放
}

void Game_Update()
{
	//更新処理
	PlayerUpdate();
	Field_Update();
	Camera_Update();	//カメラ更新処理
	ManagerCollider::UpdateAllCollisions();
	//キー入力チェック
//スタートボタンが押されたらシーンを切り替え
//フェード処理中はキーを受け付けない
	if (Keyboard_IsKeyDownTrigger(KK_ENTER) && (GetFadeState() == FADE_NONE))
	{
		//フェードアウトさせてシーンを切り替える
		XMFLOAT4	color(0.0f, 0.0f, 0.0f, 1.0f);
		SetFade(40.0f, color, FADE_OUT, SCENE_RESULT);
	}

}

void Game_Draw()
{ 
	Light.SetEnable(TRUE);			//ライティングON
	Shader_SetLight(Light.Light);	//ライト構造体をシェーダーへセット
	SetDepthTest(TRUE);

	Camera_Draw();		//Drawの最初で呼ぶ！
	Field_Draw();
	PlayerDraw();

	//2D描画
	Light.SetEnable(FALSE);			//ライティングOFF
	Shader_SetLight(Light.Light);	//ライト構造体をシェーダーへセット
	SetDepthTest(FALSE);
}

