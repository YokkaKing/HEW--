
#include	"Controller.h"
#include	"Player.h"
#include	"Game.h"
#include	"Camera.h"
#include	"shader.h"
#include	"collision.h"
#include	"Evolution.h"
#include	"colliderFactory.h"
#include "debug_ostream.h"
#include "fade.h"

#include "IWeapon.h"
#include "WeaponFactory.h"

#define CLIMB_SPEED (0.05f)
#define JUMP_FORCE (0.15)

//ボールオブジェクト
PLAYER	g_Player;

static ID3D11Device* g_pDevice;
static ID3D11DeviceContext* g_pContext;

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
	g_Player.m_FrictionRate = 0.98f;
	g_Player.m_EvolutionType = EVOLUTION_TYPE::EVOLUTION_TYPE_NONE;


	//追加処理: 体力と武器の初期化
	g_Player.m_MaxHealth = 100.0f; // 最大体力を100に設定
	g_Player.m_Health = g_Player.m_MaxHealth; // 現在の体力を最大体力に
	g_Player.m_IsAttacking = false;

	// 武器の初期化（例としてCreateWeaponという関数を使用）
	// 実際には、他のcppで定義されている武器のインスタンスを生成し、ポインタをセットします。
	// g_Player.m_CurrentWeapon = CreateWeapon(/* 武器のタイプ */);


	g_Player.m_CurrentWeapon = WeaponFactory::CreateWeapon
	(
		WeaponType::SWORD, // 初期装備は剣
		pDevice,
		pContext
	);
	// m_AttackTimerの初期化をPlayer.hへの追加を前提に追記
	g_Player.m_AttackTimer = 0;




	g_StopTime = 0.0f;
	g_Player.SetObject(g_Player.m_position, g_Player.m_scale, "Player", 0);
	EvolutionInitialize();
}
void	PlayerFinalize()
{
	if (g_Player.m_CurrentWeapon)
	{
		g_Player.m_CurrentWeapon->Finalize();
		delete g_Player.m_CurrentWeapon;
		g_Player.m_CurrentWeapon = nullptr;
	}

	ModelRelease(g_Player.m_model);

}
void	PlayerUpdate()
{
	EvolvePlayer();			// Eキーで進化タイプを選択（一度だけ実行）
	ApplyEvolutionEffect();// 進化タイプに応じたパラメータを適用



	// ★ プレイヤーのHPと武器の状態を出力 (追加)
	hal::dout << "--- Player Status ---\n";
	hal::dout << "Health: " << g_Player.m_Health << "\n";
	if (g_Player.m_CurrentWeapon)
	{
		hal::dout << "IsAttacking: " << (g_Player.m_CurrentWeapon->IsAttacking() ? "TRUE" : "FALSE") << "\n";
	}
	hal::dout << "---------------------\n";









	// 1. 武器の更新とタイマー進行
	if (g_Player.m_CurrentWeapon)
	{
		g_Player.m_CurrentWeapon->Update(0.0f); // タイマーが進む
	}

	// 2. 武器が攻撃終了を要求しているかチェック
	if (g_Player.m_CurrentWeapon && g_Player.m_CurrentWeapon->IsAttacking())
	{
		if (g_Player.m_CurrentWeapon->ShouldEndAttack()) // 剣自身が30フレームを超えたと判断
		{
			g_Player.m_CurrentWeapon->EndAttack(); // 攻撃終了処理を実行
			// プレイヤー側の g_Player.m_IsAttacking も武器側の IsAttacking() に置き換えることで
			// プレイヤー側のタイマーは完全に不要になります。
			hal::dout << "攻撃終了 (武器側タイマーで完了)\n";
			g_Player.m_Health -= 50;
		
		
		
		
		
		}
	}
	

	//体力チェック
	if (g_Player.m_Health <= 0.0f)
	{
		// プレイヤーが倒れた（ゲームオーバーまたは勝利条件達成）
		// 例: ゲームオーバー処理、勝利判定フラグのセットなど
		PlayerDie(); // 死亡処理関数作成も可
		return; // 倒れていたら操作を停止
	}

	Player_ManualMove();

}

void Player_ManualMove()
{
	// Game.hで宣言したGetPlayerController()を使い、インスタンスを取得
	Controller* playerController = GetPlayerController();

	// コントローラーが接続されているか確認 (IsConnected()は汎用チェック、UpdateState()はGame_Updateで呼ばれる前提)
	// 【修正】playerControllerがNULLでなく、かつ接続されているかを確認
	bool isControllerConnected = playerController && playerController->IsConnected();

	// 【修正】デバッグコードを、接続されていれば入力値を出力するように変更
	if (isControllerConnected)
	{
		float thumbX = playerController->GetLeftThumbX();
		float thumbY = playerController->GetLeftThumbY();
		bool a_pressed = playerController->IsButtonPressed(XINPUT_GAMEPAD_A);

		// 接続されているときだけ詳細を出力
		hal::dout << "--- Controller Check (CONNECTED) ---\n";
		hal::dout << "LStick X: " << thumbX << ", Y: " << thumbY << "\n";
		hal::dout << "A Button: " << (a_pressed ? "PRESSED" : "UP") << "\n";
		hal::dout << "------------------------------------\n";
	}
	else
	{
		// 接続されていないときは簡潔に出力
		hal::dout << "--- Controller Check (DISCONNECTED) ---\n";
	}


	g_Player.m_gameObject->m_position = g_Player.m_position;

	// カメラの前方向ベクトル
	float forwardX = GetCameraPosition().x - GetCameraAtPosition().x;
	float forwardZ = GetCameraPosition().z - GetCameraAtPosition().z;

	hal::dout << g_Player.m_velocity.y << "\n";

	if (!g_Player.m_isGround) // 地面についてないときに重力発動
	{
		g_Player.m_velocity.x += g_Player.m_acceleration.x;
		g_Player.m_velocity.y += g_Player.m_acceleration.y;
		g_Player.m_velocity.z += g_Player.m_acceleration.z;
		hal::dout << "おちてる\n";
	}

	// 地面についているときにコヨーテタイムが1.0fになる
	if (g_Player.m_isGround)
	{
		g_Player.m_koyoteTime = 1.0f;
	}
	else
	{
		g_Player.m_koyoteTime -= 0.1f;
	}

	float len = sqrtf(forwardX * forwardX + forwardZ * forwardZ);
	forwardX /= len;
	forwardZ /= len;

	// カメラの右方向ベクトル
	float rightX = forwardZ;// 右方向は前方向ベクトルを90度回転
	float rightZ = -forwardX;


	// 移動量初期化
	float moveX = 0.0f;
	float moveZ = 0.0f;

	float speed = 0.0f;//前後移動量
	float strafe = 0.0f;//左右移動量

	//ゲームパッド入力 (左スティック) の統合
	if (isControllerConnected)
	{
		float thumbX = playerController->GetLeftThumbX(); // X軸 (-1.0f ～ 1.0f)
		float thumbY = playerController->GetLeftThumbY(); // Y軸 (-1.0f ～ 1.0f)

		// Y軸入力が優先されるように、キーボードの入力値に加算ではなく上書き/代入する
		if (std::abs(thumbY) > 0.001f)
		{
			// Y軸は上に押すと正の値。移動方向と合わせるため符号を調整
			speed = thumbY * -0.1f;
		}
		// X軸入力が優先されるように、キーボードの入力値に加算ではなく上書き/代入する
		if (std::abs(thumbX) > 0.001f)
		{
			strafe = thumbX * -0.1f;
		}
	}

	// 移動量の計算 (カメラベクトルに応じて移動量を決定)
	// ここはキーボードとパッドの入力が合算された状態で処理される

	moveX += forwardX * speed;
	moveZ += forwardZ * speed;

	moveX += rightX * strafe;
	moveZ += rightZ * strafe;

	// 最終速度
	g_Player.m_velocity.x = moveX;
	g_Player.m_velocity.z = moveZ;

	//ジャンプ
	bool jumpInput = false;

	if (isControllerConnected)
	{
		// Bボタン (XINPUT_GAMEPAD_B) が押されているかをチェック
		jumpInput |= playerController->IsButtonPressed(XINPUT_GAMEPAD_B);
	}

	// スペース or Bボタンが押され、かつコヨーテタイムが残っている
	if (jumpInput && g_Player.m_koyoteTime > 0.0f)
	{
		g_Player.m_velocity.y = (JUMP_FORCE);
		g_Player.m_isGround = false;
		g_Player.m_koyoteTime = 0.0f;
	}
	else
	{
		g_Player.m_isGround = false;
	}

	//攻撃操作
	bool attackInput = false;

	if (isControllerConnected)
	{
		// Aボタン (XINPUT_GAMEPAD_A) が押されているかをチェック
		attackInput |= playerController->IsButtonPressed(XINPUT_GAMEPAD_A);
	}

	if (attackInput)
	{
		hal::dout << "攻撃した " << "\n";
		// 攻撃を開始する
		if (g_Player.m_CurrentWeapon && !g_Player.m_CurrentWeapon->IsAttacking())
		{
		
			if (g_Player.m_CurrentWeapon)
			{
				// プレイヤーの位置と回転を渡して攻撃開始
				g_Player.m_CurrentWeapon->StartAttack(g_Player.m_position, g_Player.m_rotation);
			}
		}
	}


	// 攻撃アニメーションの終了やクールダウンの処理は
	// 別の場所（例：武器のUpdate関数やPlayerUpdate内）で行う


	//最終的な位置更新
	g_Player.m_position.x += g_Player.m_velocity.x;
	g_Player.m_position.z += g_Player.m_velocity.z;
	g_Player.m_position.y += g_Player.m_velocity.y;


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

	if (g_Player.m_CurrentWeapon)
	{
		g_Player.m_CurrentWeapon->Draw(g_Player.m_position, g_Player.m_rotation);
	}


}

void PlayerTakeDamage(float damageAmount)
{
	// 体力を減らす
	g_Player.m_Health -= damageAmount;

	// 体力が0以下にならないようにクランプ
	if (g_Player.m_Health < 0.0f)
	{
		g_Player.m_Health = 0.0f;
	}

	// デバッグ出力
	hal::dout << "Player Damaged! Current Health: " << g_Player.m_Health << "\n";

	// 体力0になった場合の処理は PlayerUpdate
}

BOOL PlayerDie()
{
	if (GetFadeState() != FADE_NONE)
	{
		return FALSE; // 既に処理中なので何もしない
	}
	//フェードアウトさせてシーンを切り替える
	XMFLOAT4	color(0.0f, 0.0f, 0.0f, 1.0f);
	SetFade(40.0f, color, FADE_OUT, SCENE_RESULT);
	return TRUE;
}












XMFLOAT3 GetPlayerPosition()
{
	return g_Player.m_position;
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