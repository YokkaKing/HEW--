
//Player.cpp

#include	"keyboard.h"
#include	"Player.h"
#include	"Camera.h"
#include	"shader.h"
#include    "collision.h"
#include    "Evolution.h"
#include	"colliderFactory.h"


#define CLIMB_SPEED (2)

//ボールオブジェクト
PLAYER	g_Player;

ID3D11Device* g_pDevice;
ID3D11DeviceContext* g_pContext;

float g_StopTime = 0.0f; // ボールが制止するまでの時間



void	PlayerInitialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	g_pDevice = pDevice;
	g_pContext = pContext;

	g_Player.m_model = ModelLoad("asset\\model\\test.fbx");

	g_Player.m_position = XMFLOAT3(0.0f, 0.5f, 1.0f);
	g_Player.m_rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_Player.m_velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	
	g_Player.m_scale = XMFLOAT3(1.0f, 1.0f, 1.0f);

	g_Player.State = PLAYER_STATE::PLAYER_STATE_MOVE;

	g_Player.m_acceleration = XMFLOAT3(0.0f, -9.8f / 600.0f * 0.5f, 0.0f);
	g_Player.FrictionRate = 0.98f;
	g_Player.EvolutionType = EVOLUTION_TYPE::EVOLUTION_TYPE_NONE;

	g_StopTime = 0.0f;

	EvolutionInitialize();
}
void	PlayerFinalize()
{


	ModelRelease(g_Player.m_model);

}
void	PlayerUpdate()
{
	EvolvePlayer();           // Eキーで進化タイプを選択（一度だけ実行）
	ApplyEvolutionEffect();   // 進化タイプに応じたパラメータを適用


	switch (g_Player.State)
	{
	case PLAYER_STATE::PLAYER_STATE_IDLE:
		Player_Idle();

		Player_ManualMove();

		break;
	case PLAYER_STATE::PLAYER_STATE_MOVE:
		Player_Move();
		break;
	case PLAYER_STATE::PLAYER_STATE_DIRECTION:
		Player_Direction();
		Player_ManualMove();
		break;
	case PLAYER_STATE::PLAYER_STATE_POWER:
		Player_Power();
		break;
	case PLAYER_STATE::PLAYER_STATE_JUMP:
		Player_Jump();
		Player_ManualMove();
		break;
	}

	//常に実行される物理演算 (加速度->速度、速度->位置)

	g_Player.m_velocity.x += g_Player.m_acceleration.x;
	g_Player.m_velocity.y += g_Player.m_acceleration.y; //重力
	g_Player.m_velocity.z += g_Player.m_acceleration.z;


	// ManualMove中でない場合のみ、VelocityをPositionに適用する
	g_Player.m_position.x += g_Player.m_velocity.x;
	g_Player.m_position.y += g_Player.m_velocity.y;
	g_Player.m_position.z += g_Player.m_velocity.z;


	// 常に当たり判定を行う
	float hit=0;

	//地面に着地した際の処理 (ジャンプ/移動からの着地判定)
	if (hit > 0.001f)
	{
		// Y軸方向の速度をリセット（めり込み防止と反発防止）
		g_Player.m_velocity.y = 0.0f;

		// ジャンプ中またはMOVE中の場合、IDLEに戻す
		if (g_Player.State == PLAYER_STATE::PLAYER_STATE_JUMP || g_Player.State == PLAYER_STATE::PLAYER_STATE_MOVE)
		{
			g_Player.State = PLAYER_STATE::PLAYER_STATE_IDLE;
		}
	}

}

void Player_ManualMove() // 新しい手動移動関数として作成を推奨
{
	// カメラの情報を取得
	XMFLOAT3 v1 = GetCameraAtPosition();
	XMFLOAT3 v2 = GetCameraPosition();
	XMFLOAT3 Forward, Right;
	float MoveSpeed = 3.0f / 60.0f; // 毎フレームの移動速度 (調整が必要)
	float len;

	//前方ベクトル
	Forward.x = v1.x - v2.x;
	Forward.y = 0.0f;
	Forward.z = v1.z - v2.z;

	len = sqrtf(Forward.x * Forward.x + Forward.z * Forward.z);

	// 正規化
	if (len > 0.00001f) {
		Forward.x /= len;
		Forward.z /= len;

		//右方ベクトル (Right Vector) の計算
		Right.x = -Forward.z;
		Right.y = 0.0f;
		Right.z = Forward.x;
	}
	else {


		return;
	}

	// 当たり判定

	// 前後移動
	if (Keyboard_IsKeyDown(KK_W)) {
		g_Player.m_position.x += Forward.x * MoveSpeed; // 前進
		g_Player.m_position.z += Forward.z * MoveSpeed;
	}
	if (Keyboard_IsKeyDown(KK_S)) {
		g_Player.m_position.x -= Forward.x * MoveSpeed; // 後退
		g_Player.m_position.z -= Forward.z * MoveSpeed;
	}

	// 左右移動
	if (Keyboard_IsKeyDown(KK_A)) {
		g_Player.m_position.x += Right.x * MoveSpeed; // 左移動 (Rightの反対)
		g_Player.m_position.z += Right.z * MoveSpeed;
	}
	if (Keyboard_IsKeyDown(KK_D)) {
		g_Player.m_position.x -= Right.x * MoveSpeed; // 右移動
		g_Player.m_position.z -= Right.z * MoveSpeed;
	}
}

void	PlayerDraw() 
{
	//ワールド行列作成
	XMMATRIX	scale = XMMatrixScaling(
		g_Player.m_scale.x,
		g_Player.m_scale.y,
		g_Player.m_scale.z);
	XMMATRIX	rotation = XMMatrixRotationRollPitchYaw(
		g_Player.m_rotation.x,
		g_Player.m_rotation.y,
		g_Player.m_rotation.z);
	XMMATRIX	translation = XMMatrixTranslation(
		g_Player.m_position.x,
		g_Player.m_position.y,
		g_Player.m_position.z);
	XMMATRIX	world = scale * rotation * translation;

	//変換行列作成
	XMMATRIX	view = GetViewMatrix();
	XMMATRIX	projection = GetProjectionMatrix();
	XMMATRIX	wvp = world * view * projection;

	//シェーダーへ行列をセット
	Shader_SetWorldMatrix(world);
	Shader_SetMatrix(wvp);

	//モデルの描画リクエスト
	ModelDraw(g_Player.m_model);

}

XMFLOAT3 GetPlayerPosition()
{
	return g_Player.m_position;
}

void Player_Idle()
{
	if (Keyboard_IsKeyDownTrigger(KK_SPACE))
	{
		// 上向きに初速を設定 (この値でジャンプの高さが決まります)
		g_Player.m_velocity.y += 0.2f;

		// 状態をJUMPに切り替え
		g_Player.State = PLAYER_STATE::PLAYER_STATE_JUMP;
	}
}

void Player_Move()
{


	//g_Player.m_Position.x += g_Player.m_Velocity.x;
	//g_Player.m_Position.y += g_Player.m_Velocity.y;
	//g_Player.m_Position.z += g_Player.m_Velocity.z;

	//速度を徐々に減衰させていく

	g_Player.m_velocity.x *= g_Player.FrictionRate;
	g_Player.m_velocity.z *= g_Player.FrictionRate;

	//g_Player.m_Velocity.x *= 0.98;
	//g_Player.m_Velocity.z *= 0.98;

	//静止チェック
	float len = (g_Player.m_velocity.x * g_Player.m_velocity.x + g_Player.m_velocity.y * g_Player.m_velocity.y + g_Player.m_velocity.z * g_Player.m_velocity.z);
	if (len <= 0.0002f)//静止とみなす速度
	{
		g_StopTime++;
		if (g_StopTime > (60.0f * 2))//2秒間止まっている
		{
			g_Player.m_velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_Player.State = PLAYER_STATE::PLAYER_STATE_DIRECTION;
			g_StopTime = 0.0f;
		}
	}

	//// 当たり判定
	//float hit = PlayerField_Collision();

}

void Player_Power()
{
	//打ち出すパワーを決める
	float power = PLAYER_SPEED_MAX * 0.12f;

	g_Player.m_velocity.x *= power;
	g_Player.m_velocity.y *= power;
	g_Player.m_velocity.z *= power;

	g_Player.State = PLAYER_STATE::PLAYER_STATE_MOVE;
}

void Player_Direction()
{
	//とりあえずカメラの向いてる方向へ転がす
	//スペースキーで転がる
	if (Keyboard_IsKeyDownTrigger(KK_SPACE))
	{
		//カメラの向き
		XMFLOAT3 v1 = GetCameraAtPosition();
		XMFLOAT3 v2 = GetCameraPosition();
		XMFLOAT3 Direction;

		Direction.x = v1.x - v2.x;
		Direction.y = 0.0f;
		Direction.z = v1.z - v2.z;

		float len = sqrtf((Direction.x * Direction.x + Direction.y * Direction.y + Direction.z * Direction.z));
		Direction.x /= len;
		Direction.z /= len;

		g_Player.m_velocity = Direction;

		g_Player.State = PLAYER_STATE::PLAYER_STATE_POWER;
	}

}


void Player_Jump()
{

}

PLAYER* GetPlayer()
{
	return &g_Player;
}


void PLAYER::OnCollision(const CollisionInfo& info)
{
	if (!info.isHit) return;

	// --- まずタグで相手を識別 ---
	if (info.other)
	{
		// 例えば壁・木だけコリジョン有効
		if (info.other->m_tag == "Wall" ||
			info.other->m_tag == "Tree")
		{
			//================================================================
			//	押し戻し
			//================================================================
			m_position.x += info.normal.x * info.penetration;
			m_position.y += info.normal.y * info.penetration;
			m_position.z += info.normal.z * info.penetration;

			//================================================================
			//	地面判定
			//================================================================
			if (info.normal.y > 0.7f)
			{
				m_isGround = true;
				m_velocity.y = 0;
			}

			//================================================================
			//	壁判定
			//================================================================
			float horiz = fabs(info.normal.x) + fabs(info.normal.z);
			if (horiz > 0.7f)
			{
				m_velocity.x = 0;
				m_velocity.z = 0;
			}
		}
		else if (info.other->m_tag == "Lift")
		{
			//================================================================
			//	押し戻し
			//================================================================
			m_position.x += info.normal.x * info.penetration;
			m_position.y += info.normal.y * info.penetration;
			m_position.z += info.normal.z * info.penetration;

			//================================================================
			//	地面判定
			//================================================================
			if (info.normal.y > 0.7f)
			{
				m_isGround = true;
				m_velocity.y = 0;
			}

			//================================================================
			//	壁判定
			//================================================================
			float horiz = fabs(info.normal.x) + fabs(info.normal.z);
			if (horiz > 0.7f)
			{
				m_velocity.x = 0;
				m_velocity.z = 0;
				m_velocity.y = CLIMB_SPEED;
			}
		}
		else
		{
			return; // 他は無視
		}
	}
}

void PLAYER::SetObject(XMFLOAT3 pos, XMFLOAT3 scl, std::string tag, int lay)
{
	GameObject* obj = ColliderFactory::CreateBoxObject(
		pos,
		scl,
		tag,
		lay
	);

	g_Player.m_gameObject = obj;

	for (auto& col : obj->GetColliders<>())
	{
		col->owner = &g_Player;
	}
}
