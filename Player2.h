#pragma once

//Player2.h

#include	<d3d11.h>
#include	<DirectXMath.h>
#include	"direct3d.h"
using namespace DirectX;

#include	"model.h"

#define PLAYER2_SPEED_MAX (1.0f)
#define PLAYER2_RADIUS    (0.2f)


void	Player2Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
void	Player2Finalize();
void	Player2Update();
void	Player2Draw();

XMFLOAT3 GetPlayer2Position();

void Player2_Idle();
void Player2_Move();
void Player2_Power();
void Player2_Direction();


//プレイヤーの状態
enum PLAYER2_STATE
{
	PLAYER2_STATE_IDLE = 0,	//何もしない
	PLAYER2_STATE_MOVE,		//移動
	PLAYER2_STATE_DIRECTION,	//方向指示
	PLAYER2_STATE_POWER,		//威力指示
};

//プレイヤー構造体
class PLAYER2
{
public:
	XMFLOAT3	Position;	//表示座標
	XMFLOAT3	Rotation;	//回転角
	XMFLOAT3	Scaling;	//拡大率
	XMFLOAT3	Velocity;	//速度
	XMFLOAT3    Acceleration; //落下速度



	PLAYER2_STATE	State;		//状態
	MODEL* Model;		//モデルデータ

};

PLAYER2* GetPlayer2();
