#pragma once

#include "Player.h" 
#include <DirectXMath.h>

using namespace DirectX;



// 進化システム初期化 
void EvolutionInitialize();

void EvolutionFinalize();

// プレイヤーを進化させるメイン関数 
void EvolvePlayer();

// 進化タイプに応じたパラメーター補正を適用
void ApplyEvolutionEffect();

void EvolvePlayer2();
void ApplyEvolutionEffect2();
