#ifndef CONTROLLER_H
#define CONTROLLER_H


#define NOMINMAX

#include <windows.h>
#include <Xinput.h>
#include <cmath> // std::abs, std::sqrt, std::powのために必要
#include <algorithm> // std::min, std::max のために必要

// Xinput.lib をリンクする
#pragma comment(lib, "XInput.lib")

// 最大コントローラー数 (IsConnected()で全インデックスをチェックするために必要)
#define MAX_CONTROLLERS 4

// アナログスティックのデッドゾーンを設定
// (この値より小さい入力は無視される)
constexpr float DEADZONE_THRESHOLD = 0.15f;

class Controller
{
public:
    // コンストラクタ
    Controller(int index);

    // コントローラーの状態を更新（ポーリング）
    // 戻り値: 接続されていれば true
    bool UpdateState();

    // コントローラーが接続されているか (全インデックスをチェックする汎用ロジック)
    bool IsConnected() const;

    // ボタンが押されているか
    bool IsButtonPressed(WORD button) const;

    // 左スティックの正規化されたX軸の値を取得 (-1.0f から 1.0f)
    float GetLeftThumbX() const;

    // 左スティックの正規化されたY軸の値を取得 (-1.0f から 1.0f)
    float GetLeftThumbY() const;
    
    //右スティックの正規化されたX軸の値を取得 (-1.0f から 1.0f)
    float GetRightThumbX() const;

    //右スティックの正規化されたY軸の値を取得 (-1.0f から 1.0f)
    float GetRightThumbY() const;

    //左トリガーの値を取得 (0.0f ～ 1.0f)
    float GetLeftTrigger() const;

    //右トリガーの値を取得 (0.0f ～ 1.0f)
    float GetRightTrigger() const;






    // 振動を設定
    // leftMotor: 左側のモーター速度 (0.0f から 1.0f)
    // rightMotor: 右側のモーター速度 (0.0f から 1.0f)
    void SetVibration(float leftMotor, float rightMotor);

private:
    int controllerIndex;
    XINPUT_STATE currentState;

    // スティックの生の値から正規化された値を計算するヘルパー関数
    float NormalizeThumbValue(SHORT value) const;
};

#endif // CONTROLLER_H