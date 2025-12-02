#include "Controller.h"
#include "EvolutionType.h"

// XInputのボタン定義 (XINPUT_GAMEPAD_XXX) を使用します。

/**
 * @brief Construct a new Controller object
 * * @param index コントローラーのユーザーインデックス (0～3)
 */
Controller::Controller(int index) : controllerIndex(index)
{
    // 構造体をゼロクリア
    ZeroMemory(&currentState, sizeof(XINPUT_STATE));
}

/**
 * @brief コントローラーの状態を更新
 * * @return true コントローラーが接続されている
 * @return false コントローラーが接続されていない
 */
bool Controller::UpdateState()
{
    // 前回の状態をクリアし、現在の状態を取得
    ZeroMemory(&currentState, sizeof(XINPUT_STATE));
    DWORD result = XInputGetState(controllerIndex, &currentState);

    if (result == ERROR_SUCCESS)
    {
        return true; // 接続済み
    }
    else
    {
        return false; // 未接続またはエラー
    }
}

/**
 * @brief コントローラーが接続されているか
 * * @return true 接続済み
 * @return false 未接続
 */
bool Controller::IsConnected() const
{
    // 現在のインスタンスが持っているインデックス 0～3 を使用
    for (DWORD i = 0; i < MAX_CONTROLLERS; i++)
    {
        XINPUT_STATE state;
        ZeroMemory(&state, sizeof(XINPUT_STATE));
        if (XInputGetState(i, &state) == ERROR_SUCCESS)
        {
            // 少なくともどれか一つのXInputデバイスが接続されている
            return true;
        }
    }
    return false; // すべてのインデックスで未接続
}

/**
 * @brief 指定したボタンが押されているか
 * * @param button XINPUT_GAMEPAD_XXX のボタンマスク
 * @return true 押されている
 * @return false 押されていない
 */
bool Controller::IsButtonPressed(WORD button) const
{
    return (currentState.Gamepad.wButtons & button) != 0;
}

/**
 * @brief スティックの生の値からデッドゾーンを考慮した正規化された値を計算
 * * @param value スティックの生の値 (SHORT: -32768 ～ 32767)
 * @return float 正規化された値 (-1.0f ～ 1.0f)。デッドゾーン内なら 0.0f
 */
float Controller::NormalizeThumbValue(SHORT value) const
{
    // スティックの最大値 (32767.0f)
    const float maxThumb = 32767.0f;
    float normalizedValue = (float)value / maxThumb;

    // デッドゾーン (円形デッドゾーンを適用する場合、軸ごとに単純化)
    // ここでは単純な軸ごとのデッドゾーンを適用
    if (std::abs(normalizedValue) < DEADZONE_THRESHOLD)
    {
        return 0.0f;
    }

    // デッドゾーンを超えた後の範囲で再スケーリング (オプション)
    // この実装では単純にデッドゾーンの外側の値を返す
    return normalizedValue;
}

/**
 * @brief 左スティックのX軸の値を取得 (-1.0f ～ 1.0f)
 */
float Controller::GetLeftThumbX() const
{
    return NormalizeThumbValue(currentState.Gamepad.sThumbLX);
}

/**
 * @brief 左スティックのY軸の値を取得 (-1.0f ～ 1.0f)
 */
float Controller::GetLeftThumbY() const
{
    // XInputのY軸は上に押すと正の値 (32767)
    return NormalizeThumbValue(currentState.Gamepad.sThumbLY);
}

float Controller::GetRightThumbX() const
{
    // XInput の右スティックのX軸は sThumbRX
    return NormalizeThumbValue(currentState.Gamepad.sThumbRX);
}

/**
 * @brief 右スティックのY軸の値を取得 (-1.0f ～ 1.0f)
 */
float Controller::GetRightThumbY() const
{
    // XInput の右スティックのY軸は sThumbRY
    return NormalizeThumbValue(currentState.Gamepad.sThumbRY);
}

/**
 * @brief 左トリガーの値を取得 (0.0f ～ 1.0f)
 */
float Controller::GetLeftTrigger() const
{
    // トリガーの最大値 (255.0f)
    const float maxTrigger = 255.0f;
    float normalizedValue = (float)currentState.Gamepad.bLeftTrigger / maxTrigger;

    // トリガーにはスティックほどの厳密なデッドゾーンは通常不要ですが、ここでは単に正規化して返す
    return normalizedValue;
}

/**
 * @brief 右トリガーの値を取得 (0.0f ～ 1.0f)
 */
float Controller::GetRightTrigger() const
{
    // トリガーの最大値 (255.0f)
    const float maxTrigger = 255.0f;
    float normalizedValue = (float)currentState.Gamepad.bRightTrigger / maxTrigger;

    return normalizedValue;
}




/**
 * @brief 振動を設定
 * * @param leftMotor 左側のモーター速度 (0.0f ～ 1.0f)
 * @param rightMotor 右側のモーター速度 (0.0f ～ 1.0f)
 */
void Controller::SetVibration(float leftMotor, float rightMotor)
{
    // XInputのモーター速度は WORD (0 ～ 65535) で設定
    XINPUT_VIBRATION vibration;
    ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));

    // float (0.0f ～ 1.0f) を WORD (0 ～ 65535) に変換
    vibration.wLeftMotorSpeed = (WORD)(std::min(std::max(leftMotor, 0.0f), 1.0f) * 65535.0f);
    vibration.wRightMotorSpeed = (WORD)(std::min(std::max(rightMotor, 0.0f), 1.0f) * 65535.0f);

    XInputSetState(controllerIndex, &vibration);
}