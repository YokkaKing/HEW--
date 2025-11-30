// WeaponFactory.cpp

#include "WeaponFactory.h"
#include "IWeapon.h"
//// ★★★ 既に作成されている武器のヘッダーをインクルード ★★★
#include "Sword.h"    // 仮: class Sword : public IWeapon {...};
//#include "Bow.h"      // 仮: class Bow : public IWeapon {...};
//#include "Spear.h"    // 仮: class Spear : public IWeapon {...};
//#include "Shuriken.h" // 仮: class Shuriken : public IWeapon {...};
//#include "Hammer.h"   // 仮: class Hammer : public IWeapon {...};

IWeapon* WeaponFactory::CreateWeapon(WeaponType type, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    IWeapon* newWeapon = nullptr;

    switch (type)
    {
    case WeaponType::SWORD:
        newWeapon = new Sword();
        break;
    //case WeaponType::BOW:
    //    newWeapon = new Bow();
    //    break;
    //case WeaponType::SPEAR:
    //    newWeapon = new Spear();
    //    break;
    //case WeaponType::SHURIKEN:
    //    newWeapon = new Shuriken();
    //    break;
    //case WeaponType::HAMMER:
    //    newWeapon = new Hammer();
    //    break;
    default:
        // タイプが指定されていない場合は nullptr
        break;
    }

    if (newWeapon)
    {
        // 生成に成功したら、初期化も行う
        newWeapon->Initialize(pDevice, pContext);
    }

    return newWeapon;
}