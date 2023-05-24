#include <stdio.h>
#include <windows.h>
#include <glad.h>
#include <glfw3.h>
#include "debug.h"
#include "typinggame.h"
#include "typinggame_types.h"
#include "win32_fileapi.h"
#include "glad.c"
#include "win32_fileapi.cpp"
#include "gl_texture.cpp"
#include "gl_shader.cpp"

global r32 WindowWidth = 800;
global r32 WindowHeight = 800;
global const char* WindowName = "Typing Game";

GAME_INIT_FUNCTION(GameInitStub)
{
}
GAME_UPDATE_FUNCTION(GameUpdateStub)
{
}
struct game_code 
{
    HINSTANCE Lib;
    gameInitCallback *GameInitCallback;
    gameUpdateCallback *GameUpdateCallback;
};

void
LoadGameDLL(game_code *GameCode)
{
    if (GameCode->Lib != NULL)
    {
        FreeLibrary(GameCode->Lib);
    }
    while(CopyFileA("game.dll", "gameTemp.dll", FALSE) == 0)
    {
        
    }
    GameCode->Lib = LoadLibraryA("gameTemp.dll");
    if (GameCode->Lib != NULL)
    {
        GameCode->GameInitCallback = (gameInitCallback*) GetProcAddress(GameCode->Lib, "InitiateGame");
        GameCode->GameUpdateCallback = (gameUpdateCallback*) GetProcAddress(GameCode->Lib, "UpdateGame");
        if (GameCode->GameInitCallback == NULL ||
            GameCode->GameUpdateCallback == NULL)
        {
            GameCode->GameInitCallback = GameInitStub;
            GameCode->GameUpdateCallback = GameUpdateStub;
        }
    }
}

void
ClearScreen(f32 R, f32 G, f32 B, f32 A)
{
    glClearColor(R, G, B, A);
    glClear(GL_COLOR_BUFFER_BIT);
}

int 
CALLBACK WinMain(HINSTANCE instance,
                 HINSTANCE prevInstance,
                 LPSTR commandLine,
                 int showCode)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    GLFWwindow * Window = glfwCreateWindow(WindowWidth, WindowHeight, WindowName, NULL, NULL);
    if ( Window == NULL )
    {
        return -1;
    }
    glfwMakeContextCurrent(Window);
    
#if 0    
    if (callback)
    {
        glfwSetFramebufferSizeCallback(window, callback);
    }
#endif
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        glfwTerminate();
        return -1;
    }
    game_struct Game;
    Game.ClearScreen = ClearScreen;
    game_code GameCode;
    LoadGameDLL(&GameCode);
    GameCode.GameInitCallback(&Game);
    while(!glfwWindowShouldClose(Window))
    {
        TIMED_BLOCK("GameLoop");
        ClearScreen(1.0f, 0.0f, 1.0f, 1.0f);
        GameCode.GameUpdateCallback(&Game, 0, 0);
        glfwSwapBuffers(Window);
        glfwPollEvents();
    }
    return 0;
}