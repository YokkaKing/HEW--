// Evolution.cpp

#include "Evolution.h"
#include "Player.h"
#include "Player2.h"
#include "keyboard.h"
#include "Controller.h"
#include <cstdio>
#include <cstdlib>

extern PLAYER g_Player;
extern PLAYER2 g_Player2;

const char* INITIAL_MODEL_PATH = "asset\\model\\test.fbx";
static Controller* g_pEvoController = nullptr;
static Controller* g_pEvoController2 = nullptr;

void EvolutionInitialize()
{
    // 初期化処理
    if (g_pEvoController == nullptr)
    {
        g_pEvoController = new Controller(0);
    }
    if (g_pEvoController2 == nullptr)
    {
        g_pEvoController2 = new Controller(1); // Player2はインデックス1を使用
    }
}

void EvolutionFinalize()
{
    // 終了処理

    //コントローラーの解放を追加
    if (g_pEvoController != nullptr)
    {
        delete g_pEvoController;
        g_pEvoController = nullptr;
    }
    if (g_pEvoController2 != nullptr)
    {
        delete g_pEvoController2;
        g_pEvoController2 = nullptr;
    }
}

void EvolvePlayer()
{
    const char* newModelPath = nullptr;
 
    // コントローラーの状態を更新
    bool isControllerConnected = false;
    if (g_pEvoController != nullptr)
    {
        isControllerConnected = g_pEvoController->UpdateState();
    }

    //共通の進化条件: 未進化 (EVOLUTION_TYPE_NONE) の場合のみ

    if (g_Player.m_EvolutionType == EVOLUTION_TYPE::EVOLUTION_TYPE_NONE)
    {
        bool evolved = false;

        // 【変更1】Xボタンの代わりにLBボタンでタイプAに変身
        if (isControllerConnected && g_pEvoController->IsButtonPressed(XINPUT_GAMEPAD_LEFT_SHOULDER)) // LB
        {
            g_Player.m_EvolutionType = EVOLUTION_TYPE::EVOLUTION_TYPE_A;
            newModelPath = "asset\\model\\ball.fbx"; // A用モデルパス
            evolved = true;
        }
        // 【変更2】Yボタンの代わりにRBボタンでタイプBに変身
        else if (isControllerConnected && g_pEvoController->IsButtonPressed(XINPUT_GAMEPAD_RIGHT_SHOULDER)) // RB
        {
            g_Player.m_EvolutionType = EVOLUTION_TYPE::EVOLUTION_TYPE_B;
            newModelPath = "asset\\model\\tree.fbx"; // B用モデルパス
            evolved = true;
        }

        // 変身が完了したら共通のスケール変更を適用
        if (evolved)
        {
            if (g_Player.m_model != nullptr)
            {
                ModelRelease(g_Player.m_model);
            }

            //新しいモデルをロード
            g_Player.m_model = ModelLoad(newModelPath);

            // 進化後の共通処理：スケール変更
           // g_Player.scale = XMFLOAT3(1.5f, 1.5f, 1.5f);
        }
    }
    else if (g_Player.m_EvolutionType != EVOLUTION_TYPE::EVOLUTION_TYPE_NONE)
    {
        // 【変更3】変身解除条件の分岐
        bool shouldRevert = false;

        if (g_Player.m_EvolutionType == EVOLUTION_TYPE::EVOLUTION_TYPE_A)
        {
            // タイプA (LBで変身) の場合、LTで解除
            // GetLeftTrigger() は 0.0f～1.0f の値なので、デッドゾーン (例: 0.5f) 以上でトリガーが引かれたと判定
            if (isControllerConnected && g_pEvoController->GetLeftTrigger() > 0.5f)
            {
                shouldRevert = true;
            }
        }
        else if (g_Player.m_EvolutionType == EVOLUTION_TYPE::EVOLUTION_TYPE_B)
        {
            // タイプB (RBで変身) の場合、RTで解除
            if (isControllerConnected && g_pEvoController->GetRightTrigger() > 0.5f)
            {
                shouldRevert = true;
            }
        }

        if (shouldRevert)
        {
            // 古いモデル（進化後のモデル）を解放
            if (g_Player.m_model != nullptr)
            {
                ModelRelease(g_Player.m_model);
            }
            // 進化タイプをリセット
            g_Player.m_EvolutionType = EVOLUTION_TYPE::EVOLUTION_TYPE_NONE;

            // 初期モデルをロード
            g_Player.m_model = ModelLoad(INITIAL_MODEL_PATH);

            // スケールを初期値に戻す (1.0倍)
            g_Player.m_scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
        }

    }
}

void EvolvePlayer2()
{
    const char* newModelPath = nullptr;

    // コントローラーの状態を更新
    bool isControllerConnected = false;
    if (g_pEvoController2 != nullptr)
    {
        // g_pEvoController2 を使用
        isControllerConnected = g_pEvoController2->UpdateState();
    }

    //共通の進化条件: 未進化 (EVOLUTION_TYPE_NONE) の場合のみ

    if (g_Player2.m_EvolutionType == EVOLUTION_TYPE::EVOLUTION_TYPE_NONE)
    {
        bool evolved = false;

        // Xボタンの代わりにLBボタンでタイプAに変身
        if (isControllerConnected && g_pEvoController2->IsButtonPressed(XINPUT_GAMEPAD_LEFT_SHOULDER)) // LB
        {
            g_Player2.m_EvolutionType = EVOLUTION_TYPE::EVOLUTION_TYPE_A;
            newModelPath = "asset\\model\\ball.fbx"; // A用モデルパス
            evolved = true;
        }
        // Yボタンの代わりにRBボタンでタイプBに変身
        else if (isControllerConnected && g_pEvoController2->IsButtonPressed(XINPUT_GAMEPAD_RIGHT_SHOULDER)) // RB
        {
            g_Player2.m_EvolutionType = EVOLUTION_TYPE::EVOLUTION_TYPE_B;
            newModelPath = "asset\\model\\tree.fbx"; // B用モデルパス
            evolved = true;
        }

        // 変身が完了したら共通のスケール変更を適用
        if (evolved)
        {
            if (g_Player2.m_model != nullptr)
            {
                ModelRelease(g_Player2.m_model);
            }

            //新しいモデルをロード
            g_Player2.m_model = ModelLoad(newModelPath);

            // 進化後の共通処理：スケール変更
            // g_Player2.scale = XMFLOAT3(1.5f, 1.5f, 1.5f);
        }
    }
    else if (g_Player2.m_EvolutionType != EVOLUTION_TYPE::EVOLUTION_TYPE_NONE)
    {
        // 変身解除条件の分岐
        bool shouldRevert = false;

        if (g_Player2.m_EvolutionType == EVOLUTION_TYPE::EVOLUTION_TYPE_A)
        {
            // タイプA (LBで変身) の場合、LTで解除
            if (isControllerConnected && g_pEvoController2->GetLeftTrigger() > 0.5f)
            {
                shouldRevert = true;
            }
        }
        else if (g_Player2.m_EvolutionType == EVOLUTION_TYPE::EVOLUTION_TYPE_B)
        {
            // タイプB (RBで変身) の場合、RTで解除
            if (isControllerConnected && g_pEvoController2->GetRightTrigger() > 0.5f)
            {
                shouldRevert = true;
            }
        }

        if (shouldRevert)
        {
            // 古いモデル（進化後のモデル）を解放
            if (g_Player2.m_model != nullptr)
            {
                ModelRelease(g_Player2.m_model);
            }
            // 進化タイプをリセット
            g_Player2.m_EvolutionType = EVOLUTION_TYPE::EVOLUTION_TYPE_NONE;

            // 初期モデルをロード
            g_Player2.m_model = ModelLoad(INITIAL_MODEL_PATH);

            // スケールを初期値に戻す (1.0倍)
            g_Player2.m_scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
        }

    }
}

void ApplyEvolutionEffect()
{
    if (g_Player.m_EvolutionType == EVOLUTION_TYPE::EVOLUTION_TYPE_NONE)
    {
        // 初期状態の基本パラメータ
        g_Player.m_acceleration.x = 0.0f;
        g_Player.m_acceleration.z = 0.0f;
        g_Player.m_FrictionRate = 0.98f;
        g_Player.m_scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
    }
    else if (g_Player.m_EvolutionType == EVOLUTION_TYPE::EVOLUTION_TYPE_A)
    {
        // 進化先 A 
        g_Player.m_acceleration.x = 0.005f;
        g_Player.m_acceleration.z = 0.005f;
        g_Player.m_FrictionRate = 0.99f; // 減速しにくくする (滑りやすい)
    }
    else if (g_Player.m_EvolutionType == EVOLUTION_TYPE::EVOLUTION_TYPE_B)
    {
        // 進化先 B
        g_Player.m_acceleration.x = 0.0f;
        g_Player.m_acceleration.z = 0.0f;
        g_Player.m_FrictionRate = 0.95f; // 減速しやすくする (止まりやすい)
    }
}

void ApplyEvolutionEffect2()
{
    if (g_Player2.m_EvolutionType == EVOLUTION_TYPE::EVOLUTION_TYPE_NONE)
    {
        // 初期状態の基本パラメータ
        g_Player2.m_acceleration.x = 0.0f;
        g_Player2.m_acceleration.z = 0.0f;
        g_Player2.m_FrictionRate = 0.98f;
        g_Player2.m_scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
    }
    else if (g_Player2.m_EvolutionType == EVOLUTION_TYPE::EVOLUTION_TYPE_A)
    {
        // 進化先 A 
        g_Player2.m_acceleration.x = 0.005f;
        g_Player2.m_acceleration.z = 0.005f;
        g_Player2.m_FrictionRate = 0.99f; // 減速しにくくする (滑りやすい)
    }
    else if (g_Player2.m_EvolutionType == EVOLUTION_TYPE::EVOLUTION_TYPE_B)
    {
        // 進化先 B
        g_Player2.m_acceleration.x = 0.0f;
        g_Player2.m_acceleration.z = 0.0f;
        g_Player2.m_FrictionRate = 0.95f; // 減速しやすくする (止まりやすい)
    }
}