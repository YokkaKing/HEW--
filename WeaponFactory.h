// WeaponFactory.h

#pragma once

#include "IWeapon.h"

// 武器生成ファクトリー
class WeaponFactory
{
public:
    // 武器タイプを指定して、新しいIWeaponオブジェクトを生成する静的関数
    // 呼び出し元がdeleteする必要があるため、unique_ptrなどスマートポインタの使用を推奨しますが、
    // 今回はポインタでシンプルに記述します。
    static IWeapon* CreateWeapon(WeaponType type, ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
};
