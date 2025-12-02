#pragma once

//Player.h

#include	<d3d11.h>
#include	<DirectXMath.h>
#include	"direct3d.h"
using namespace DirectX;

#include	"model.h"
#include "gameObject.h"
#include "EvolutionType.h"
class IWeapon;


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


	float           m_FrictionRate;   // 速度減衰率
	EVOLUTION_TYPE  m_EvolutionType;  // 進化タイプ (A or B or NONE)

	PLAYER_STATE	State;		//状態
	float m_Health;        // プレイヤーの現在の体力
	float m_MaxHealth;     // プレイヤーの最大体力
	bool m_IsAttacking;    // 攻撃中かどうか
	IWeapon* m_CurrentWeapon; // 装備している武器へのポインタ
	int m_AttackTimer;

public:
	void OnCollision(const CollisionInfo& info)override;
	void SetObject(XMFLOAT3 pos, XMFLOAT3 scl, std::string tag, int lay);
};


void	PlayerInitialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
void	PlayerFinalize();
void	PlayerUpdate();
void	PlayerDraw();
void PlayerTakeDamage(float damageAmount);
XMFLOAT3 GetPlayerPosition();
BOOL PlayerDie();

void Player_Idle();
void Player_Move();
void Player_Power();
void Player_Direction();
void Player_Jump();


void Player_ManualMove();



PLAYER* GetPlayer();





