/*
* ファイル名	terrain.h
* タイトル	地形
* 作成者		久保木幹太
* 作成日		11月25日
* 更新日		11月25日
*/

#ifndef TERRAIN_H
#define TERRAIN_H

#include <d3d11.h>
#include <DirectXMath.h>
#include "direct3d.h"
#include "sprite.h"
using namespace DirectX;
#include"gameObject.h"
#include<string>

// 地形の種類
enum class TERRAIN_TYPE
{
	HILL = 0,
	WALL,
	TREE,

	MAX
};

void TerrainInitialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
void TerrainFinalize();
void TerrainUpdate();
void TerrainDraw();

class TERRAIN_OBJECT
{
public:
	XMFLOAT3 m_position;
	XMFLOAT3 m_distance;	// 中心からどれだけ離れた場所にいるか
	TERRAIN_TYPE m_type;	// どの種類の地形か
};

class TERRAIN : public GameObject
{
public:
	// マザーポジション,こいつが動くと他が連動して動く
	XMFLOAT3 m_motherPosition;
	// 作成したオブジェクトを保存する器
	std::vector<GameObject*> terrainObjects;
	std::vector<GameObject*> hills;
	std::vector<GameObject*> walls;
	std::vector<GameObject*> trees;
public:
	void SetObject(XMFLOAT3 pos, XMFLOAT3 scl, std::string tag, int lay);

public:
	// string型で書いたオブジェクトの当たり判定をchar型にして効率よくする
	std::vector<std::vector<std::vector<char>>> ConvertTerrain(std::vector<std::vector<std::vector<std::string>>> terrain);
	// char型になったオブジェクトの当たり判定がいくつあるのか数える
	size_t CountObjects(const std::vector<std::vector<std::vector<char>>>& obj);
	// 受け取った総量のオブジェクトの当たり判定をデータとして格納する
	std::vector<TERRAIN_OBJECT> InitializeObject(const std::vector<std::vector<std::vector<char>>>& terrainChip, TERRAIN_TYPE type);
	void UpdateObject(std::vector<TERRAIN_OBJECT> terrain);
	// 自動で当たり判定を作り出す
	void CreateHit(std::vector<TERRAIN_OBJECT> terrain, XMFLOAT3 motherPosition);
};

#endif // TERRAIN_H