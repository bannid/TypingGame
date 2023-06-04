#include <windows.h>
#include "typinggame.h"
#include "random.cpp"

DLL_API void
UpdateGame(game_struct *Game, f32 DeltaTime, f32 Time)
{
    Game->ClearScreen(.0f, .0f, .0f, 1.0f);
	if (Game->GameInput.CharacterKey.IsPressed == TRUE)
	{
		if(Game->GameInput.CharacterKey.Key == GameInputKey_A)
		{
			Game->PlaySound("pew");
			Game->Player.Angle+=.1f;
		}
		else if (Game->GameInput.CharacterKey.Key == GameInputKey_D)
		{
			Game->Player.Angle-=.1f;
			Game->PlaySound("pew2");
		}
	}
	Game->DrawQuad(Game->Player.Pos, Game->Player.Scale, Game->Player.Angle);
}

DLL_API void 
InitiateGame(game_struct* Game)
{
    Game->World.Width = 100;
    Game->World.Height = 100;
    Game->Player.PosScale = {400, 200, 10, 100};
    Game->Player.Angle = 0.0f;
}