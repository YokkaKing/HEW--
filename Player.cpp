
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

	g_Player.Position = XMFLOAT3(0.0f, 0.5f, 1.0f);
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

	g_Player.Velocity.x += g_Player.Acceleration.x;
	g_Player.Velocity.y += g_Player.Acceleration.y; //重力
	g_Player.Velocity.z += g_Player.Acceleration.z;


	// ManualMove中でない場合のみ、VelocityをPositionに適用する
	g_Player.Position.x += g_Player.Velocity.x;
	g_Player.Position.y += g_Player.Velocity.y;
	g_Player.Position.z += g_Player.Velocity.z;


	// 常に当たり判定を行う
	float hit = PlayerField_Collision();

	//地面に着地した際の処理 (ジャンプ/移動からの着地判定)
	if (hit > 0.001f) // PlayerField_Collision()が地面衝突時に非ゼロを返すことを想定
	{
		// Y軸方向の速度をリセット（めり込み防止と反発防止）
		g_Player.Velocity.y = 0.0f;

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
	float hit = PlayerField_Collision();

	// 前後移動
	if (Keyboard_IsKeyDown(KK_W)) {
		g_Player.Position.x += Forward.x * MoveSpeed; // 前進
		g_Player.Position.z += Forward.z * MoveSpeed;
	}
	if (Keyboard_IsKeyDown(KK_S)) {
		g_Player.Position.x -= Forward.x * MoveSpeed; // 後退
		g_Player.Position.z -= Forward.z * MoveSpeed;
	}

	// 左右移動
	if (Keyboard_IsKeyDown(KK_A)) {
		g_Player.Position.x += Right.x * MoveSpeed; // 左移動 (Rightの反対)
		g_Player.Position.z += Right.z * MoveSpeed;
	}
	if (Keyboard_IsKeyDown(KK_D)) {
		g_Player.Position.x -= Right.x * MoveSpeed; // 右移動
		g_Player.Position.z -= Right.z * MoveSpeed;
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
	Shader_SetMatrix(wvp);

	//モデルの描画リクエスト
	ModelDraw(g_Player.Model);

}

XMFLOAT3 GetPlayerPosition()
{
	return g_Player.Position;
}

void Player_Idle()
{
	if (Keyboard_IsKeyDownTrigger(KK_SPACE))
	{
		// 上向きに初速を設定 (この値でジャンプの高さが決まります)
		g_Player.Velocity.y += 0.2f;

		// 状態をJUMPに切り替え
		g_Player.State = PLAYER_STATE::PLAYER_STATE_JUMP;
	}
}

void Player_Move()
{


	//g_Player.Position.x += g_Player.Velocity.x;
	//g_Player.Position.y += g_Player.Velocity.y;
	//g_Player.Position.z += g_Player.Velocity.z;

	//速度を徐々に減衰させていく
	g_Player.Velocity.x *= 0.98;
	g_Player.Velocity.z *= 0.98;

	//静止チェック
	float len = (g_Player.Velocity.x * g_Player.Velocity.x + g_Player.Velocity.y * g_Player.Velocity.y + g_Player.Velocity.z * g_Player.Velocity.z);
	if (len <= 0.0002f)//静止とみなす速度
	{
		g_StopTime++;
		if (g_StopTime > (60.0f * 2))//2秒間止まっている
		{
			g_Player.Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
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

	g_Player.Velocity.x *= power;
	g_Player.Velocity.y *= power;
	g_Player.Velocity.z *= power;

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

		g_Player.Velocity = Direction;

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



