/*
* ファイル名	terrain.cpp
* タイトル	地形
* 作成者		久保木幹太
* 作成日		11月25日
* 更新日		11月25日
*/

//================================================================
//	マクロ定義
//================================================================
#define TERRAIN_SIZE (0.25f)
#define TERRAIN_DISTANCE (0.25f) // 地形の当たり判定のオブジェクト同士の距離

//================================================================
//	インクルード
//================================================================
#include"terrain.h"
#include"keyboard.h"
#include"collision.h"
#include"colliderFactory.h"
#include"debug_ostream.h"
#include"shader.h"
#include"Camera.h"
#include"Player.h"
#include<string>

//================================================================
//	グローバル変数
//================================================================
// 地形オブジェクト
TERRAIN g_Terrain;

static ID3D11Device* g_pDevice;
static ID3D11DeviceContext* g_pContext;

std::vector<TERRAIN_OBJECT> HILL;	// 丘のデータ
std::vector<TERRAIN_OBJECT> WALL;	// 壁のデータ
std::vector<TERRAIN_OBJECT> TREE;	// 木のデータ

//================================================================
//	一文字0.25立法メートルとする
//	e -> 空気
//	a -> 当たり判定
//================================================================

// 地形::丘 の座標データ
const std::vector<std::vector<std::vector<std::string>>> Hill =
{
	{ // Y = 0 // Z->+
		{"aaaaaaaaaaaa"},
		{"aeeeeeeeeeea"},
		{"aeeeeeeeeeea"},
		{"aeeeeeeeeeea"},
		{"aeeeeeeeeeea"},
		{"aeeeeeeeeeea"},
		{"aeeeeeeeeeea"},
		{"aeeeeeeeeeea"},
		{"aeeeeeeeeeea"},
		{"aeeeeeeeeeea"},
		{"aeeeeeeeeeea"},
		{"aaaaaaaaaaaa"},
	},
	{ // Y = 0.25
		{"eeeeeeeeeeee"},
		{"eaaaaaaaaaae"},
		{"eaeeeeeeeeae"},
		{"eaeeeeeeeeae"},
		{"eaeeeeeeeeae"},
		{"eaeeeeeeeeae"},
		{"eaeeeeeeeeae"},
		{"eaeeeeeeeeae"},
		{"eaeeeeeeeeae"},
		{"eaeeeeeeeeae"},
		{"eaaaaaaaaaae"},
		{"eeeeeeeeeeee"},
	},
	{ // Y = 0.5
		{"eeeeeeeeeeee"},
		{"eeeeeeeeeeee"},
		{"eeaaaaaaaaee"},
		{"eeaeeeeeeaee"},
		{"eeaeeeeeeaee"},
		{"eeaeeeeeeaee"},
		{"eeaeeeeeeaee"},
		{"eeaeeeeeeaee"},
		{"eeaeeeeeeaee"},
		{"eeaaaaaaaaee"},
		{"eeeeeeeeeeee"},
		{"eeeeeeeeeeee"},
	},
	{ // Y = 0.75
		{"eeeeeeeeeeee"},
		{"eeeeeeeeeeee"},
		{"eeeeeeeeeeee"},
		{"eeeaaaaaaeee"},
		{"eeeaeeeeaeee"},
		{"eeeaeeeeaeee"},
		{"eeeaeeeeaeee"},
		{"eeeaeeeeaeee"},
		{"eeeaaaaaaeee"},
		{"eeeeeeeeeeee"},
		{"eeeeeeeeeeee"},
		{"eeeeeeeeeeee"},
	},
	{ // Y = 0.75
		{"eeeeeeeeeeee"},
		{"eeeeeeeeeeee"},
		{"eeeeeeeeeeee"},
		{"eeeeeeeeeeee"},
		{"eeeeeaaaeeee"},
		{"eeeeeaeaeeee"},
		{"eeeeeaeaeeee"},
		{"eeeeeaaaeeee"},
		{"eeeeeeeeeeee"},
		{"eeeeeeeeeeee"},
		{"eeeeeeeeeeee"},
		{"eeeeeeeeeeee"},
	},
	{ // Y = 1
		{"eeeeeeeeeeee"},
		{"eeeeeeeeeeee"},
		{"eeeeeeeeeeee"},
		{"eeeeeeeeeeee"},
		{"eeeeeeeeeeee"},
		{"eeeeeeaeeeee"},
		{"eeeeeeaeeeee"},
		{"eeeeeeeeeeee"},
		{"eeeeeeeeeeee"},
		{"eeeeeeeeeeee"},
		{"eeeeeeeeeeee"},
		{"eeeeeeeeeeee"},
	},
};

void TerrainInitialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	g_pDevice = pDevice;
	g_pContext = pContext;

	g_Terrain.m_model = ModelLoad("asset\\model\\test.fbx");

	g_Terrain.m_position = XMFLOAT3(0.0f, 0.5f, 3.0f);
	g_Terrain.m_rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_Terrain.m_velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);

	g_Terrain.m_scale = XMFLOAT3(1.0f, 1.0f, 1.0f);

	g_Terrain.m_motherPosition = g_Terrain.m_position;	// マザーポジションをプレイヤーのポジションと同期
	
	auto terrainChip = g_Terrain.ConvertTerrain(Hill);						// stringをcharに変更
	HILL = g_Terrain.InitializeObject(terrainChip, TERRAIN_TYPE::HILL);		// HILLに丘のデータを格納する
	g_Terrain.CreateHit(HILL, g_Terrain.m_motherPosition);					// 当たり判定の作成
}
void TerrainFinalize()
{
	ModelRelease(g_Terrain.m_model);
}
void TerrainUpdate()
{
	g_Terrain.UpdateObject(HILL);
}
void TerrainDraw()
{
	//ワールド行列作成
	XMMATRIX	scale = XMMatrixScaling(
		1.0f,
		1.0f,
		1.0f);
	XMMATRIX	rotation = XMMatrixRotationRollPitchYaw(
		g_Terrain.m_rotation.x,
		g_Terrain.m_rotation.y,
		g_Terrain.m_rotation.z);
	XMMATRIX	translation = XMMatrixTranslation(
		g_Terrain.m_position.x,
		g_Terrain.m_position.y,
		g_Terrain.m_position.z);
	XMMATRIX	world = scale * rotation * translation;

	//変換行列作成
	XMMATRIX	view = GetViewMatrix();
	XMMATRIX	projection = GetProjectionMatrix();
	XMMATRIX	wvp = world * view * projection;

	//シェーダーへ行列をセット
	Shader_SetWorldMatrix(world);
	Shader_SetMatrix(wvp);

	//モデルの描画リクエスト
	ModelDraw(g_Terrain.m_model);

	for (int i = 0; i < g_Terrain.hills.size(); i++)
	{
		//ワールド行列作成
		XMMATRIX	scale = XMMatrixScaling(
			0.25f,
			0.25f,
			0.25f);
		XMMATRIX	rotation = XMMatrixRotationRollPitchYaw(
			0.0f,
			0.0f,
			0.0f);
		XMMATRIX	translation = XMMatrixTranslation(
			g_Terrain.hills[i]->m_position.x,
			g_Terrain.hills[i]->m_position.y,
			g_Terrain.hills[i]->m_position.z);
		XMMATRIX	world = scale * rotation * translation;

		//変換行列作成
		XMMATRIX	view = GetViewMatrix();
		XMMATRIX	projection = GetProjectionMatrix();
		XMMATRIX	wvp = world * view * projection;

		//シェーダーへ行列をセット
		Shader_SetWorldMatrix(world);
		Shader_SetMatrix(wvp);

		//モデルの描画リクエスト
		ModelDraw(g_Terrain.m_model);
	}
}
void TERRAIN::SetObject(XMFLOAT3 pos, XMFLOAT3 scl, std::string tag, int lay)
{
	GameObject* obj = ColliderFactory::CreateBoxObject(
		pos,
		scl,
		tag,
		lay
	);

	if (obj != nullptr)
	{
		terrainObjects.push_back(obj);

		if (obj->m_tag == "HILL") hills.push_back(obj);
		if (obj->m_tag == "WALL") walls.push_back(obj);
		if (obj->m_tag == "TREE") trees.push_back(obj);
	}
}

// string型で書いたオブジェクトの当たり判定をchar型にして効率よくする
std::vector<std::vector<std::vector<char>>> TERRAIN::ConvertTerrain(std::vector<std::vector<std::vector<std::string>>> terrain)
{
	std::vector<std::vector<std::vector<char>>> obj;

	obj.reserve(terrain.size()); // 事前にY軸分の容量を確保

	for (const auto& layer : terrain) // Y軸方向
	{
		obj.emplace_back();					// 新しく枠を作る(次元の追加)
		obj.back().reserve(layer.size());	// Z軸分の容量を確保

		for (const auto& row : layer) // rowはstd::vector<std::string>
		{
			std::vector<char> rowChars;
			for (const auto& str : row) // strはstd::string
			{
				rowChars.insert(rowChars.end(), str.begin(), str.end());
			}
			obj.back().push_back(std::move(rowChars));
		}

	}

	return obj;
}
// char型になったオブジェクトの当たり判定がいくつあるのか数える
size_t TERRAIN::CountObjects(const std::vector<std::vector<std::vector<char>>>& obj)
{
	size_t count = 0;

	for (const auto& layer : obj)
	{
		for (const auto& row : layer)
		{
			for (char c : row)
			{
				if (c == 'a')
				{
					count++;
				}
			}
		}
	}

	return count;
}
// 受け取った総量のオブジェクトの当たり判定をデータとして格納する
std::vector<TERRAIN_OBJECT> TERRAIN::InitializeObject(const std::vector<std::vector<std::vector<char>>>& terrainChip, TERRAIN_TYPE type)
{
	std::vector<TERRAIN_OBJECT> terrain;

	size_t objects = 0;	// 何個オブジェクトがあるか

	objects = CountObjects(terrainChip); // オブジェクトの数を格納

	terrain.clear();
	terrain.reserve(objects);	// オブジェクトの数の分だけ事前に容量を確保

	for (size_t i = 0; i < objects; i++)
	{
		terrain.push_back(TERRAIN_OBJECT{}); // 空のTERRAIN_OBJECTを追加
	}

	int loop = 0;	// 何回ループしたか
	char c;			// 文字を取り出す

	XMFLOAT3 centerNo; // 真ん中の番号
	centerNo.y = (terrainChip.size() / 2);							// Y層の半分
	centerNo.x = (terrainChip[centerNo.y].size() / 2);				// X層の半分
	centerNo.z = (terrainChip[centerNo.y][centerNo.x].size() / 2);	// Z層の半分
	XMFLOAT3 distance;	// 現在の座標が、どれだけ中心と差があるか

	for (int y = 0; y < terrainChip.size(); y++)
	{
		for (int x = 0; x < terrainChip[y].size(); x++)
		{
			for (int z = 0; z < terrainChip[y][x].size(); z++)
			{
				c = terrainChip[y][x][z]; // 文字を取り出す

				distance.x = (x - centerNo.x) * TERRAIN_DISTANCE;	// Xが中心からどれだけ離れているか
				distance.y = (y - centerNo.y) * TERRAIN_DISTANCE;	// Yが中心からどれだけ離れているか
				distance.z = (z - centerNo.z) * TERRAIN_DISTANCE;	// Zが中心からどれだけ離れているか

				switch (c)
				{
				case 'a':
					terrain[loop].m_distance = distance;	// 中心からどれだけ離れているか渡す
					terrain[loop].m_type = type;			// 種類を格納
					loop++;
					break;

				case 'e':
					break;

				default:
					break;
				}
			}
		}
	}

	return terrain;
}
// 当たり判定の座標を更新する
void TERRAIN::UpdateObject(std::vector<TERRAIN_OBJECT> terrain)
{
	for (int i = 0; i < terrain.size(); i++)
	{
		switch (terrain[i].m_type)
		{
		case TERRAIN_TYPE::HILL:
			for (auto& hill : hills)
			{
				if (Keyboard_IsKeyDown(KK_R))
				{
					hill->Move(0.0f, 0.001f, 0.0f);
				}
				else if (Keyboard_IsKeyDown(KK_T))
				{
					hill->Move(0.0f, -0.001f, 0.0f);
				}
			}
			break;

		case TERRAIN_TYPE::WALL:
			break;

		case TERRAIN_TYPE::TREE:
			break;

		case TERRAIN_TYPE::MAX:
			break;

		default:
			break;
		}
	}
}
// 当たり判定を作る
void TERRAIN::CreateHit(std::vector<TERRAIN_OBJECT> terrain, XMFLOAT3 motherPosition)
{
	XMFLOAT3 pos;

	// terrainの数だけ繰り返す
	for (int i = 0; i < terrain.size(); i++)
	{
		// distanceを使って座標を求める
		pos.x = motherPosition.x + terrain[i].m_distance.x;
		pos.y = motherPosition.y + terrain[i].m_distance.y;
		pos.z = motherPosition.z + terrain[i].m_distance.z;

		switch (terrain[i].m_type)
		{
		case TERRAIN_TYPE::HILL:
			SetObject(pos, { TERRAIN_SIZE, TERRAIN_SIZE, TERRAIN_SIZE }, "HILL", 0);
			hills[i]->m_position = pos;	// 座標を格納
			break;

		case TERRAIN_TYPE::WALL:
			break;

		case TERRAIN_TYPE::TREE:
			break;

		case TERRAIN_TYPE::MAX:
			break;

		default:
			break;
		}
	}
}