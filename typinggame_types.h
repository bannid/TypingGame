/* date = May 22nd 2023 7:13 pm */

#ifndef TYPINGGAME_TYPES_H
#define TYPINGGAME_TYPES_H
#include "typing_math.h"
#include "typedefines.h"

typedef void (*clear_screen_fptr)(f32 R, f32 G, f32 B, f32 A);
typedef void (*draw_quad)(v2f Position, v2f Scale, f32 Angle);
typedef void (*play_sound)(const char *Name);
struct player 
{
	union
	{
		f32 v[4];
		struct
		{
			v2f Pos;
			v2f Scale;
		};
		struct
		{
			v4f PosScale;
		};
	};
    f32 Angle;
};

struct target
{
    f32 X;
    f32 Y;
    v2f Velocity;
    char Character;
	BOOL IsAlive;
};

struct bullet 
{
    v2f Position;
    v2f Velocity;
};

struct world
{
    f32 Width;
    f32 Height;
};

struct screen
{
    u32 Width;
    u32 Height;
};

enum game_input_key
{
    GameInputKey_A = 65,
    GameInputKey_B,
    GameInputKey_C,
    GameInputKey_D,
    GameInputKey_E,
    GameInputKey_F,
    GameInputKey_G,
    GameInputKey_H,
    GameInputKey_I,
    GameInputKey_J,
    GameInputKey_K,
    GameInputKey_L,
    GameInputKey_M,
    GameInputKey_N,
    GameInputKey_O,
    GameInputKey_P,
    GameInputKey_Q,
    GameInputKey_R,
    GameInputKey_S,
    GameInputKey_T,
    GameInputKey_U,
    GameInputKey_V,
    GameInputKey_W,
    GameInputKey_X,
    GameInputKey_Y,
    GameInputKey_Z,
	GameInputKey_Shift = 0x01,
	GameInputKey_Backspace = 0x02
};

struct keyboard_key {
	game_input_key Key;
	BOOL IsDown;
	BOOL IsPressed;
};
struct game_keyboard_input
{
	union
	{
		keyboard_key Keys[3];
		struct 
		{
			keyboard_key ShiftKey;
			keyboard_key Backspace;
			keyboard_key CharacterKey;
		};
	};
};

struct game_struct 
{
    world World;
    screen Screen;
    player Player;
    target Targets[20];
    bullet Bullets[100];
	game_keyboard_input GameInput;
	clear_screen_fptr ClearScreen;
	draw_quad DrawQuad;
	play_sound PlaySound;
};


#endif //TYPINGGAME_TYPES_H
