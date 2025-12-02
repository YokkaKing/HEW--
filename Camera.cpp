
//Camera.cpp


#include	"Camera.h"
#include	"keyboard.h"
#include	"Player.h"
#include	"Controller.h"

//グローバル変数
static	CAMERA	CameraObject;
static	Controller* g_pController = nullptr;

XMFLOAT3		g_PlayerPosOld;//<<<<<<<<<<<<<<

void	Camera_Initialize()
{ 
	CameraObject.Position = XMFLOAT3(0.0f, 3.0f, -4.0f);
	CameraObject.AtPosition = XMFLOAT3(0.0f, 1.0f, 0.0f);
	CameraObject.UpVector = XMFLOAT3(0.0f, 1.0f, 0.0f);

	CameraObject.Fov = 45.0f;
	float width = (float)Direct3D_GetBackBufferWidth();
	float height = (float)Direct3D_GetBackBufferHeight();
	CameraObject.Aspect = width / height;
	CameraObject.NearClip = 0.5f;
	CameraObject.FarClip = 1000.0f;

	g_PlayerPosOld = GetPlayerPosition();

	if (g_pController == nullptr)
	{
		// ユーザーインデックス 0 のコントローラーを使用
		g_pController = new Controller(0);
	}
}

void	Camera_Finalize()
{
	if (g_pController != nullptr)
	{
		delete g_pController;
		g_pController = nullptr;
	}
	return;
}
void	Camera_Update()
{
	//ボールの座標取得<<<<<<<<<<<<<<<<<<<<<<
	XMFLOAT3	pos = g_PlayerPosOld;
	g_PlayerPosOld = GetPlayerPosition();

	//前回のボールと現在のボールの座標の差分<<<<<<<<<<<<<<<
	pos.x = g_PlayerPosOld.x - pos.x;
	pos.y = g_PlayerPosOld.y - pos.y;
	pos.z = g_PlayerPosOld.z - pos.z;

	//カメラを移動
	CameraObject.Position.x += pos.x;
	CameraObject.Position.y += pos.y;
	CameraObject.Position.z += pos.z;

	//注視点としてセット<<<<<<<<<<<<<<<<<<<<<<<
	CameraObject.AtPosition.x = g_PlayerPosOld.x;
	CameraObject.AtPosition.y = g_PlayerPosOld.y;
	CameraObject.AtPosition.z = g_PlayerPosOld.z;

	// コントローラーの状態を更新し、右スティックのX軸の入力を取得
	float rightThumbX = 0.0f;
	if (g_pController != nullptr && g_pController->UpdateState())
	{
		// Controller.h/.cpp に GetRightThumbX() を追加済みを前提
		rightThumbX = g_pController->GetRightThumbX();
	}
	//注視点を中心にカメラの位置を回転（Y軸回転）
	float	Rotation = 0.0f;
	//if (Keyboard_IsKeyDown(KK_Q))
	//{
	//	Rotation = 1.0f;
	//}
	//if (Keyboard_IsKeyDown(KK_E))
	//{
	//	Rotation = -1.0f;
	//}

	if (std::abs(rightThumbX) > 0.0f)
	{
		// スティックの入力値に応じて回転量を決定
		// 例: 入力を約 2.0 倍して、回転速度を調整（この値は調整可能です）
		// 右スティックを右に倒すと (rightThumbX > 0) -> Rotation が負 (左回転) になるように調整
		Rotation = -rightThumbX * 2.0f;
	}



	//注視点からカメラへのベクトル
	XMFLOAT2	vec;
	vec.x = CameraObject.Position.x - CameraObject.AtPosition.x;
	vec.y = CameraObject.Position.z - CameraObject.AtPosition.z;
	//ベクトルの回転
	float	co = cosf(XMConvertToRadians(Rotation));
	float	si = sinf(XMConvertToRadians(Rotation));
	CameraObject.Position.x = (vec.x * co - vec.y * si);
	CameraObject.Position.z = (vec.x * si + vec.y * co);
	CameraObject.Position.x += CameraObject.AtPosition.x;
	CameraObject.Position.z += CameraObject.AtPosition.z;

	////vecを正規化する
	//float len = sqrtf(vec.x * vec.x + vec.y * vec.y);
	//vec.x /= len;
	//vec.y /= len;

	////注視点の方向へ移動する
	//float	speed = 0.0f;
	//if (Keyboard_IsKeyDown(KK_W))
	//{
	//	speed = -0.1f;
	//}
	//if (Keyboard_IsKeyDown(KK_S))
	//{
	//	speed = 0.1f;
	//}

	////今回の移動量ベクトル
	//vec.x *= speed;
	//vec.y *= speed;

	////座標と注視点へ移動量を加算
	//CameraObject.Position.x += vec.x;
	//CameraObject.Position.z += vec.y;
	//CameraObject.AtPosition.x += vec.x;
	//CameraObject.AtPosition.z += vec.y;


	//FOVの変更
	if (Keyboard_IsKeyDown(KK_Z))
	{
		CameraObject.Fov += 0.3f;
		if (CameraObject.Fov > 160.0f)
		{
			CameraObject.Fov = 160.0f;
		}

	}
	if (Keyboard_IsKeyDown(KK_X))
	{
		CameraObject.Fov -= 0.3f;
		if (CameraObject.Fov < 5.0f)
		{
			CameraObject.Fov = 5.0f;
		}
	}



	return;
}
void	Camera_Draw()
{ 
	//プロジェクション行列作成
	CameraObject.Projection = XMMatrixPerspectiveFovLH
	(
		XMConvertToRadians(CameraObject.Fov),
		CameraObject.Aspect,
		CameraObject.NearClip,
		CameraObject.FarClip
	);

	//ビュー行列作成
	XMVECTOR	vpos = XMVectorSet(
		CameraObject.Position.x,
		CameraObject.Position.y,
		CameraObject.Position.z,
		0.0f);
	XMVECTOR	vAt = XMVectorSet(
		CameraObject.AtPosition.x,
		CameraObject.AtPosition.y,
		CameraObject.AtPosition.z,
		0.0f
	);
	XMVECTOR	vUp = XMVectorSet(
		CameraObject.UpVector.x,
		CameraObject.UpVector.y,
		CameraObject.UpVector.z,
		0.0f
	);
	CameraObject.View = XMMatrixLookAtLH(
		vpos,
		vAt,
		vUp
	);

	return;

}

void	SetCameraFov(float fov)
{
	CameraObject.Fov = fov;
}
void	SetCameraAspect(float asp)
{ 
	CameraObject.Aspect = asp;
}
void	SetCameraClip(float n, float f)
{ 
	CameraObject.NearClip = n;
	CameraObject.FarClip = f;
}

void	SetCameraPosition(XMFLOAT3 pos)
{
	CameraObject.Position = pos;
}
void	SetCameraAtPosition(XMFLOAT3 at)
{
	CameraObject.AtPosition = at;
}
void	SetCameraUpVector(XMFLOAT3 up)
{ 
	CameraObject.UpVector = up;
}

XMMATRIX	GetViewMatrix()
{ 
	return	CameraObject.View;
}
XMMATRIX	GetProjectionMatrix()
{
	return	CameraObject.Projection;
}

XMFLOAT3 GetCameraAtPosition()
{
	return CameraObject.AtPosition;
}

XMFLOAT3 GetCameraPosition()
{
	return CameraObject.Position;
}



