#pragma once

//Player.h

#include	<d3d11.h>
#include	<DirectXMath.h>
#include	"direct3d.h"
using namespace DirectX;

#include	"model.h"

#define PLAYER_SPEED_MAX (1.0f)
#define PLAYER_RADIUS    (0.2f)

void	PlayerInitialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
void	PlayerFinalize();
void	PlayerUpdate();
void	PlayerDraw();

XMFLOAT3 GetPlayerPosition();

void Player_Idle();
void Player_Move();
void Player_Power();
void Player_Direction();


//ボールの状態
enum PLAYER_STATE
{
	PLAYER_STATE_IDLE = 0,	//何もしない
	PLAYER_STATE_MOVE,		//移動
	PLAYER_STATE_DIRECTION,	//方向指示
	PLAYER_STATE_POWER,		//威力指示
};

//ボール構造体
class PLAYER
{
	public:
		XMFLOAT3	Position;	//表示座標
		XMFLOAT3	Rotation;	//回転角
		XMFLOAT3	Scaling;	//拡大率
		XMFLOAT3	Velocity;	//速度
		XMFLOAT3    Acceleration; //落下速度

		PLAYER_STATE	State;		//状態
		MODEL*		Model;		//モデルデータ

};

PLAYER* GetPlayer();





