/* date = May 23rd 2023 9:35 pm */

#ifndef TYPINGGAME_H
#define TYPINGGAME_H

#include "typinggame_types.h"


#define DLL_API extern "C" __declspec(dllexport)
#define GAME_INIT_FUNCTION(name) void name(game_struct* Game)
#define GAME_UPDATE_FUNCTION(name) void name(game_struct *Game, f32 dt, f32 t)

typedef GAME_INIT_FUNCTION(gameInitCallback);
typedef GAME_UPDATE_FUNCTION(gameUpdateCallback);

#endif //TYPINGGAME_H
