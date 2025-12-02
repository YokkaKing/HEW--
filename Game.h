
//Game.h
#pragma once

#include "Controller.h"
#include "direct3d.h"


void Game_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
void Game_Finalize();
void Game_Update();
void Game_Draw();



// Controllerのインスタンスを取得するための関数
Controller* GetPlayerController();
Controller* GetPlayer2Controller();//プレイヤー２用