#include <windows.h>
#include "typinggame.h"

DLL_API void 
UpdateGame(game_struct *Game, f32 dt, f32 t)
{
    Game->ClearScreen(.0f, .5f, .0f, 1.0f);
	Game->Player.PosScale = {400, 200, 10, 100};
	if (Game->GameInput.CharacterKey.IsPressed == TRUE)
	{
		if(Game->GameInput.CharacterKey.Key == GameInputKey_A)
		{
			Game->PlaySound("shoot.wav");
			Game->Player.Angle+=.1f;
		}
		else if (Game->GameInput.CharacterKey.Key == GameInputKey_D)
		{
			Game->Player.Angle-=.1f;
			Game->PlaySound("shoot.wav");
		}
	}
	Game->DrawQuad(Game->Player.Pos, Game->Player.Scale, Game->Player.Angle);
}

DLL_API void 
InitiateGame(game_struct* Game)
{
    Game->World.Width = 100;
    Game->World.Height = 100;
    Game->Player.PosScale = {0, 0, 200, 200};
    Game->Player.Angle = 0.0f;
}