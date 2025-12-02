#pragma once

//Player2.h

#include	<d3d11.h>
#include	<DirectXMath.h>
#include	"direct3d.h"
using namespace DirectX;

#include	"model.h"
#include "gameObject.h"
#include "EvolutionType.h"
class IWeapon;



#define PLAYER2_SPEED_MAX (1.0f)
#define PLAYER2_RADIUS    (0.2f)
#define PLAYER2_JUMP		 (1.0f)


//プレイヤーの状態
enum PLAYER2_STATE
{
	PLAYER2_STATE_IDLE = 0,	//何もしない
	PLAYER2_STATE_MOVE,		//移動
	PLAYER2_STATE_DIRECTION,	//方向指示
	PLAYER2_STATE_POWER,		//威力指示
	PLAYER2_STATE_JUMP,		//ジャンプ
};

//プレイヤー構造体
class PLAYER2 : public GameObject
{
public:


	float           m_FrictionRate;   // 速度減衰率
	EVOLUTION_TYPE  m_EvolutionType;  // 進化タイプ (A or B or NONE)

	PLAYER2_STATE	State;		//状態
	float m_Health;        // プレイヤーの現在の体力
	float m_MaxHealth;     // プレイヤーの最大体力
	bool m_IsAttacking;    // 攻撃中かどうか
	IWeapon* m_CurrentWeapon; // 装備している武器へのポインタ
	int m_AttackTimer;

public:
	void OnCollision(const CollisionInfo& info)override;
	void SetObject(XMFLOAT3 pos, XMFLOAT3 scl, std::string tag, int lay);
};


void	Player2Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
void	Player2Finalize();
void	Player2Update();
void	Player2Draw();
void Player2TakeDamage(float damageAmount);
XMFLOAT3 GetPlayer2Position();
BOOL Player2Die();

void Player2_Idle();
void Player2_Move();
void Player2_Power();
void Player2_Direction();
void Player2_Jump();


void Player2_ManualMove();



PLAYER2* GetPlayer2();






