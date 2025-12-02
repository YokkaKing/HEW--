
//Game.cpp

#include	"Manager.h"
#include	"sprite.h"
#include	"Game.h"
#include	"keyboard.h"

#include	"field.h"
#include	"Effect.h"
#include	"Audio.h"

#include	"Camera.h"

#include "Player.h"
#include "Player2.h"

#include "Viewport.h"

#include	"direct3d.h"//<<<<<<<<<<<<<<<<<<<

LIGHTOBJECT		Light;//<<<<<<ライト管理オブジェクト


static	int		g_BgmID = NULL;	//サウンド管理ID

void Game_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{

	Field_Initialize(pDevice, pContext); // フィールドの初期化
	PlayerInitialize(pDevice, pContext); // ボールの初期化
	Player2Initialize(pDevice, pContext);
	Camera_Initialize();	//カメラ初期化

	//ビューポートの初期化
	Viewport_Initialize(Direct3D_GetWindowHandle());

	//g_BgmID = LoadAudio("asset\\Audio\\bgm.wav");	//サウンドロード
	//PlayAudio(g_BgmID, true);	//再生開始（ループあり）
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
	Player2Finalize();
	Camera_Finalize();	//カメラ終了処理

	//UnloadAudio(g_BgmID);//サウンドの解放
}

void Game_Update()
{
	//更新処理
	PlayerUpdate();
	Player2Update();
	Field_Update();
	Camera_Update();	//カメラ更新処理
	Camera2_Update();   //カメラ2更新処理
}

void Game_Draw()
{ 
	//=================================================
	//	1つのフィールドで2人のプレイヤーを描画する場合、
	//	シェーダーの行列関数を両画面の処理で呼ぶことで
	//	別々のカメラを描画することができる
	//=================================================
	Light.SetEnable(TRUE);			//ライティングON
	Shader_SetLight(Light.Light);	//ライト構造体をシェーダーへセット
	SetDepthTest(TRUE);

	ID3D11DeviceContext* g_pContext = Direct3D_GetDeviceContext();
	
	//画面分割用関数(左画面)
	g_pContext->RSSetViewports(1, &g_LeftViewPort);

	Camera_Draw();		//Drawの最初で呼ぶ！
	Shader_SetMatrix(GetViewMatrix() * GetProjectionMatrix());
	Field_Draw();
	PlayerDraw();
	Player2Draw();

	//画面分割用関数(右画面)
	g_pContext->RSSetViewports(1, &g_RightViewPort);

	Camera2_Draw();
	Shader_SetMatrix(GetViewMatrix2() * GetProjectionMatrix2());
	Field_Draw();
	PlayerDraw();
	Player2Draw();


	//2D描画
	Light.SetEnable(FALSE);			//ライティングOFF
	Shader_SetLight(Light.Light);	//ライト構造体をシェーダーへセット
	SetDepthTest(FALSE);
}

