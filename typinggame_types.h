/* date = May 22nd 2023 7:13 pm */

#ifndef TYPINGGAME_TYPES_H
#define TYPINGGAME_TYPES_H

#define global static
typedef int r32;
typedef unsigned char u8;
typedef unsigned int u32;
typedef float f32;
typedef double f64;

/*
Function pointers
*/
typedef void (*clearScreen)(f32 R, f32 G, f32 B, f32 A);
struct v2f{
    f32 X;
    f32 Y;
};

struct game_struct {
    v2f Player;
    f32 PlayerAngle;
    v2f Targets[20];
    v2f Bullets[100];
    clearScreen ClearScreen;
};


#endif //TYPINGGAME_TYPES_H
