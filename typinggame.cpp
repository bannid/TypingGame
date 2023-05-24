#include <windows.h>
#include "typinggame.h"

DLL_API void 
UpdateGame(game_struct *Game, f32 dt, f32 t)
{
    Game->ClearScreen(1.0f, 1.0f, 0.0f, 1.0f);
}

DLL_API void 
InitiateGame(game_struct* Game)
{
    
}