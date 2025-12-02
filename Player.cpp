
//Player.cpp

#include	"keyboard.h"
#include	"Player.h"
#include	"Camera.h"
#include	"shader.h"
#include    "collision.h"

//ボールオブジェクト
PLAYER	g_Player;

ID3D11Device* g_pDevice;
ID3D11DeviceContext* g_pContext;

float g_StopTime = 0.0f; // ボールが制止するまでの時間

void	PlayerInitialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	g_pDevice = pDevice;
	g_pContext = pContext;

	g_Player.Model = ModelLoad("asset\\model\\test.fbx");

	g_Player.Position = XMFLOAT3(0.0f, 0.5f, 0.0f);
	g_Player.Rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_Player.Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	
	g_Player.Scaling = XMFLOAT3(1.0f, 1.0f, 1.0f);

	g_Player.State = PLAYER_STATE::PLAYER_STATE_MOVE;

	g_Player.Acceleration = XMFLOAT3(0.0f, -9.8f / 600.0f * 0.5f, 0.0f);

	g_StopTime = 0.0f;
}
void	PlayerFinalize()
{

	ModelRelease(g_Player.Model);

}
void	PlayerUpdate()
{
	switch (g_Player.State)
	{
		Player_Direction();

	case PLAYER_STATE::PLAYER_STATE_IDLE:
		Player_Idle();
		break;
	case PLAYER_STATE::PLAYER_STATE_MOVE:
		Player_Move();
		break;
	case PLAYER_STATE::PLAYER_STATE_POWER:
		Player_Power();
		break;
	}

}
void	PlayerDraw() 
{
	//ワールド行列作成
	XMMATRIX	scale = XMMatrixScaling(
		g_Player.Scaling.x,
		g_Player.Scaling.y,
		g_Player.Scaling.z);
	XMMATRIX	rotation = XMMatrixRotationRollPitchYaw(
		g_Player.Rotation.x,
		g_Player.Rotation.y,
		g_Player.Rotation.z);
	XMMATRIX	translation = XMMatrixTranslation(
		g_Player.Position.x,
		g_Player.Position.y,
		g_Player.Position.z);
	XMMATRIX	world = scale * rotation * translation;

	//変換行列作成
	XMMATRIX	view = GetViewMatrix();
	XMMATRIX	projection = GetProjectionMatrix();
	XMMATRIX	wvp = world * view * projection;

	//シェーダーへ行列をセット
	Shader_SetWorldMatrix(world);
	//Shader_SetMatrix(wvp);

	//モデルの描画リクエスト
	ModelDraw(g_Player.Model);

}

XMFLOAT3 GetPlayerPosition()
{
	return g_Player.Position;
}

void Player_Idle()
{
	g_Player.State = PLAYER_STATE::PLAYER_STATE_MOVE;
}

void Player_Move()
{
	float PlayerMoveSpeed = 2.0f / 60.0f;

	XMFLOAT3 CameraPos = GetCameraPosition();
	XMFLOAT3 CameraAtPos = GetCameraAtPosition();
	XMFLOAT3 Forward, LR;//前後左右
	Forward.x = CameraPos.x - CameraAtPos.x;
	Forward.z = CameraPos.z - CameraAtPos.z;
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
	//g_Player.Velocity.x += g_Player.Acceleration.x;
	//g_Player.Velocity.y += g_Player.Acceleration.y;
	//g_Player.Velocity.z += g_Player.Acceleration.z;

	if (Keyboard_IsKeyDown(KK_W)) //前方方向移動
	{
		g_Player.Position.x -= Forward.x * PlayerMoveSpeed;
		g_Player.Position.z -= Forward.z * PlayerMoveSpeed;
	}
	if (Keyboard_IsKeyDown(KK_A)) //左方向移動
	{
		g_Player.Position.x -= LR.x * PlayerMoveSpeed;
		g_Player.Position.z -= LR.z * PlayerMoveSpeed;
	}
	if (Keyboard_IsKeyDown(KK_S)) //後方向移動
	{
		g_Player.Position.x += Forward.x * PlayerMoveSpeed;
		g_Player.Position.z += Forward.z * PlayerMoveSpeed;
	}
	if (Keyboard_IsKeyDown(KK_D)) //右方向移動
	{
		g_Player.Position.x += LR.x * PlayerMoveSpeed;
		g_Player.Position.z += LR.z * PlayerMoveSpeed;
	}

	g_Player.Velocity.x *= PlayerMoveSpeed;
	g_Player.Velocity.z *= PlayerMoveSpeed;

	//プレイヤーの座標計算
	g_Player.Position.x += g_Player.Velocity.x;
	g_Player.Position.y += g_Player.Velocity.y;
	g_Player.Position.z += g_Player.Velocity.z;

	//速度を徐々に減衰させていく
	g_Player.Velocity.x *= 0.98;
	g_Player.Velocity.z *= 0.98;

	//静止チェック
	float slen = (g_Player.Velocity.x * g_Player.Velocity.x + g_Player.Velocity.y * g_Player.Velocity.y + g_Player.Velocity.z * g_Player.Velocity.z);
	if (slen <= 0.0002f)//静止とみなす速度
	{
		g_StopTime++;
		if (g_StopTime > (60.0f * 2))//2秒間止まっている
		{
			g_Player.Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_Player.State = PLAYER_STATE::PLAYER_STATE_IDLE;
			g_StopTime = 0.0f;
		}
	}

	// 当たり判定
	float hit = PlayerField_Collision();

}

void Player_Power()
{
	//打ち出すパワーを決める
	float power = PLAYER_SPEED_MAX * 0.12f;

	g_Player.Velocity.x *= power;
	g_Player.Velocity.y *= power;
	g_Player.Velocity.z *= power;

	g_Player.State = PLAYER_STATE::PLAYER_STATE_MOVE;
}

void Player_Direction()
{
	//とりあえずカメラの向いてる方向へ転がす
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

		g_Player.Velocity = Direction;


}

PLAYER* GetPlayer()
{
	return &g_Player;
}



