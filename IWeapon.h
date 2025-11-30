// IWeapon.h

#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "direct3d.h"
#include <string>
#include "debug_ostream.h"

using namespace DirectX;

// 武器の種類を識別するための列挙型
enum class WeaponType
{
    SWORD,
    BOW,
    SPEAR,
    SHURIKEN,
    HAMMER,
    NONE
};

// 全ての武器クラスが継承する基底クラス（インターフェース）
class IWeapon
{
public:
    // 仮想デストラクタは、継承したクラスを正しく解放するために必須
    virtual ~IWeapon() = default;

    // 初期化処理（デバイス、コンテキスト、モデルロードなど）
    virtual void Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) = 0;

    // 終了処理（モデル解放、当たり判定破棄など）
    virtual void Finalize() = 0;

    // 攻撃開始
    // プレイヤーの位置と回転を受け取り、当たり判定を生成する
    virtual void StartAttack(const XMFLOAT3& playerPosition, const XMFLOAT3& playerRotation) = 0;

    // 攻撃終了（当たり判定を破棄する）
    virtual void EndAttack() = 0;

    // 描画
    virtual void Draw(const XMFLOAT3& playerPosition, const XMFLOAT3& playerRotation) = 0;

    // 武器自身の状態（主にタイマー）を更新する
    virtual void Update(float deltaTime) = 0;

    // 攻撃を終了すべきか、武器自身に判断させる
    virtual bool ShouldEndAttack() const = 0;
    // 攻撃中かどうかを返す
    virtual bool IsAttacking() const = 0;
};
