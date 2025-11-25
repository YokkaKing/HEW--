#pragma once

//Player.h

#include	<d3d11.h>
#include	<DirectXMath.h>
#include	"direct3d.h"
using namespace DirectX;

#include	"model.h"
#include "gameObject.h"


enum class EVOLUTION_TYPE
{
	EVOLUTION_TYPE_A, // 機動力特化
	EVOLUTION_TYPE_B, // 制動・防御特化
	EVOLUTION_TYPE_NONE // 未進化
};

#define PLAYER_SPEED_MAX (1.0f)
#define PLAYER_RADIUS    (0.2f)
#define PLAYER_JUMP		 (1.0f)


//プレイヤーの状態
enum PLAYER_STATE
{
	PLAYER_STATE_IDLE = 0,	//何もしない
	PLAYER_STATE_MOVE,		//移動
	PLAYER_STATE_DIRECTION,	//方向指示
	PLAYER_STATE_POWER,		//威力指示
	PLAYER_STATE_JUMP,		//ジャンプ
};

//プレイヤー構造体
class PLAYER: public GameObject
{
public:
	//XMFLOAT3	Position;	//表示座標
	//XMFLOAT3	Rotation;	//回転角
	//XMFLOAT3	Scaling;	//拡大率
	//XMFLOAT3	Velocity;	//速度
	//XMFLOAT3    Acceleration; //落下速度

	float           FrictionRate;   // 速度減衰率
	EVOLUTION_TYPE  EvolutionType;  // 進化タイプ (A or B or NONE)

	PLAYER_STATE	State;		//状態
	//MODEL* Model;		//モデルデータ

public:
	void OnCollision(const CollisionInfo& info)override;
	void SetObject(XMFLOAT3 pos, XMFLOAT3 scl, std::string tag, int lay);
};


void	PlayerInitialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
void	PlayerFinalize();
void	PlayerUpdate();
void	PlayerDraw();

XMFLOAT3 GetPlayerPosition();

void Player_Idle();
void Player_Move();
void Player_Power();
void Player_Direction();
void Player_Jump();


void Player_ManualMove();



PLAYER* GetPlayer();





