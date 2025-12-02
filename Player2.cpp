//Player2.cpp

#include	"keyboard.h"
#include	"Player2.h"
#include	"Camera.h"
#include	"shader.h"
#include    "collision.h"

//ボールオブジェクト
PLAYER2	g_Player2;

ID3D11Device* g_pDevice2;
ID3D11DeviceContext* g_pContext2;

float g_StopTime2 = 0.0f; // ボールが制止するまでの時間


void	Player2Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	g_pDevice2 = pDevice;
	g_pContext2 = pContext;

	g_Player2.Model = ModelLoad("asset\\model\\ball.fbx");

	g_Player2.Position = XMFLOAT3(2.0f, 0.5f, 0.0f);
	g_Player2.Rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_Player2.Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);

	g_Player2.Scaling = XMFLOAT3(1.0f, 1.0f, 1.0f);

	g_Player2.State = PLAYER2_STATE::PLAYER2_STATE_MOVE;

	g_Player2.Acceleration = XMFLOAT3(0.0f, -9.8f / 600.0f * 0.5f, 0.0f);

	g_StopTime2 = 0.0f;
}
void	Player2Finalize()
{

	ModelRelease(g_Player2.Model);

}
void	Player2Update()
{
	switch (g_Player2.State)
	{
		Player2_Direction();

	case PLAYER2_STATE::PLAYER2_STATE_IDLE:
		Player2_Idle();
		break;
	case PLAYER2_STATE::PLAYER2_STATE_MOVE:
		Player2_Move();
		break;
	case PLAYER2_STATE::PLAYER2_STATE_POWER:
		Player2_Power();
		break;
	}

}
void	Player2Draw()
{
	//ワールド行列作成
	XMMATRIX	scale = XMMatrixScaling(
		g_Player2.Scaling.x,
		g_Player2.Scaling.y,
		g_Player2.Scaling.z);
	XMMATRIX	rotation = XMMatrixRotationRollPitchYaw(
		g_Player2.Rotation.x,
		g_Player2.Rotation.y,
		g_Player2.Rotation.z);
	XMMATRIX	translation = XMMatrixTranslation(
		g_Player2.Position.x,
		g_Player2.Position.y,
		g_Player2.Position.z);
	XMMATRIX	world = scale * rotation * translation;

	//変換行列作成
	XMMATRIX	view = GetViewMatrix2();
	XMMATRIX	projection = GetProjectionMatrix2();
	XMMATRIX	wvp = world * view * projection;

	//シェーダーへ行列をセット
	Shader_SetWorldMatrix(world);
	//Shader_SetMatrix(wvp);

	//モデルの描画リクエスト
	ModelDraw(g_Player2.Model);

}

XMFLOAT3 GetPlayer2Position()
{
	return g_Player2.Position;
}

void Player2_Idle()
{
	g_Player2.State = PLAYER2_STATE::PLAYER2_STATE_MOVE;
}

void Player2_Move()
{
	float Player2MoveSpeed = 2.0f / 60.0f;

	XMFLOAT3 Camera2Pos = GetCamera2Position();
	XMFLOAT3 Camera2AtPos = GetCamera2AtPosition();
	XMFLOAT3 Forward, LR;//前後左右
	Forward.x = Camera2Pos.x - Camera2AtPos.x;
	Forward.z = Camera2Pos.z - Camera2AtPos.z;
	Forward.y = 0.0f;

	float len = sqrtf(Forward.x * Forward.x + Forward.z * Forward.z);
	if (len > 0.00001f)
	{
		Forward.x /= len;
		Forward.z /= len;

		//左右ベクトルの計算
		LR.x = -Forward.z;
		LR.y = 0.0f;
		LR.z = Forward.x;

	}

	//g_Player2.Velocity.x += g_Player2.Acceleration.x;
	//g_Player2.Velocity.y += g_Player2.Acceleration.y;
	//g_Player2.Velocity.z += g_Player2.Acceleration.z;

	if (Keyboard_IsKeyDown(KK_U)) //前方方向移動
	{
		g_Player2.Position.x -= Forward.x * Player2MoveSpeed;
		g_Player2.Position.z -= Forward.z * Player2MoveSpeed;
	}
	if (Keyboard_IsKeyDown(KK_H)) //左方向移動
	{
		g_Player2.Position.x -= LR.x * Player2MoveSpeed;
		g_Player2.Position.z -= LR.z * Player2MoveSpeed;
	}
	if (Keyboard_IsKeyDown(KK_J)) //後方向移動
	{
		g_Player2.Position.x += Forward.x * Player2MoveSpeed;
		g_Player2.Position.z += Forward.z * Player2MoveSpeed;
	}
	if (Keyboard_IsKeyDown(KK_K)) //右方向移動
	{
		g_Player2.Position.x += LR.x * Player2MoveSpeed;
		g_Player2.Position.z += LR.z * Player2MoveSpeed;
	}

	g_Player2.Velocity.x *= Player2MoveSpeed;
	g_Player2.Velocity.z *= Player2MoveSpeed;

	//プレイヤーの座標計算
	g_Player2.Position.x += g_Player2.Velocity.x;
	g_Player2.Position.y += g_Player2.Velocity.y;
	g_Player2.Position.z += g_Player2.Velocity.z;

	//速度を徐々に減衰させていく
	g_Player2.Velocity.x *= 0.98;
	g_Player2.Velocity.z *= 0.98;

	//静止チェック
	float slen = (g_Player2.Velocity.x * g_Player2.Velocity.x + g_Player2.Velocity.y * g_Player2.Velocity.y + g_Player2.Velocity.z * g_Player2.Velocity.z);
	if (slen <= 0.0002f)//静止とみなす速度
	{
		g_StopTime2++;
		if (g_StopTime2 > (60.0f * 2))//2秒間止まっている
		{
			g_Player2.Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_Player2.State = PLAYER2_STATE::PLAYER2_STATE_IDLE;
			g_StopTime2 = 0.0f;
		}
	}

	// 当たり判定
	float hit = Player2Field_Collision();

}

void Player2_Power()
{
	//打ち出すパワーを決める
	float power = PLAYER2_SPEED_MAX * 0.12f;

	g_Player2.Velocity.x *= power;
	g_Player2.Velocity.y *= power;
	g_Player2.Velocity.z *= power;

	g_Player2.State = PLAYER2_STATE::PLAYER2_STATE_MOVE;
}

void Player2_Direction()
{
	//とりあえずカメラの向いてる方向へ転がす
		//カメラの向き
	XMFLOAT3 v1 = GetCamera2AtPosition();
	XMFLOAT3 v2 = GetCamera2Position();
	XMFLOAT3 Direction;

	Direction.x = v1.x - v2.x;
	Direction.y = 0.0f;
	Direction.z = v1.z - v2.z;

	float len = sqrtf((Direction.x * Direction.x + Direction.y * Direction.y + Direction.z * Direction.z));
	Direction.x /= len;
	Direction.z /= len;

	g_Player2.Velocity = Direction;


}

PLAYER2* GetPlayer2()
{
	return &g_Player2;
}
