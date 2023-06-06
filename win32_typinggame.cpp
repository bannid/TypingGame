#include <stdio.h>
#include <windows.h>
#include <glad.h>
#include "glad.c"
#include <glfw3.h>
#include <irklang/irrKlang.h>
#include "debug.h"
#include "utils.h"
#include "typinggame_debug.h"
#include "typinggame.h"
#include "typinggame_types.h"
#include "win32_fileapi.h"
#include "asset.h"
#include "asset.cpp"
#include "gl_shader.h"
#include "random.cpp"

#define MAX_SHADERS 50
global const char* WindowName = "Typing Game";
global game_struct *GlobalGame;
global BOOL GlobalRunning = TRUE;
global u32 GlobalQuadVao;
global gl_shader GlobalShaders[MAX_SHADERS];
global u32 GlobalNumberOfShaders = 0;
global irrklang::ISoundEngine *GlobalSoundEngine = NULL;
global int NumberOfOpenFileHanldes = 0;
#define PLAY_SOUND(FunctionName) void FunctionName(const char *Name)
PLAY_SOUND(PlaySoundStub){}
GAME_INIT_FUNCTION(GameInitStub){}
GAME_UPDATE_FUNCTION(GameUpdateStub){}

struct game_code 
{
    HINSTANCE Lib;
    gameInitCallback *GameInitCallback;
    gameUpdateCallback *GameUpdateCallback;
    FILETIME FileTime;
};

inline u32
OpenGLCreateQuadTextureBuffer()
{
    GLfloat Vertices[] = 
	{
		0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		
		0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 0.0f, 1.0f, 0.0f
    };
    GLuint Vbo;
    GLuint Vao;
    glGenBuffers(1, &Vbo);
    glGenVertexArrays(1, &Vao);
    glBindVertexArray(Vao);
    glBindBuffer(GL_ARRAY_BUFFER, Vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * CNT_ARR(Vertices), Vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    return Vao;
}

inline bool
OpenGLCreateShader(const char ** VertexShaderSource, const char ** FragmentShaderSource, i32 *ProgramOut)
{
    char Log[1024];
    int Success;
    GLint VS;
    GLint FS;
    //Vertex shader
    VS = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(VS, 1, VertexShaderSource, NULL);
    glCompileShader(VS);
    glGetShaderiv(VS, GL_COMPILE_STATUS, &Success);
    //Check if the vertex shader compiled
    if (!Success)
    {
        glGetShaderInfoLog(VS, 1024, NULL, Log);
        DEBUG_LOG(Log);
        return false;
    }
    //Fragment shader
    FS = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(FS, 1, FragmentShaderSource, NULL);
    glCompileShader(FS);
    glGetShaderiv(FS, GL_COMPILE_STATUS, &Success);
    //Check if the fragment shader compiled successfully
    if (!Success)
    {
        glGetShaderInfoLog(FS, 1024, NULL, Log);
        DEBUG_LOG(Log);
        return false;
    }
    GLint Program;
    Program = glCreateProgram();
    glAttachShader(Program, VS);
    glAttachShader(Program, FS);
    glLinkProgram(Program);
    glGetProgramiv(Program, GL_LINK_STATUS, &Success);
    //Delete the shaders
    glDeleteShader(VS);
    glDeleteShader(FS);
    if (!Success)
    {
        glGetProgramInfoLog(Program, 1024, NULL, Log);
        DEBUG_LOG(Log);
        return false;
    }
    *ProgramOut = Program;
    return true;
}

inline void
OpenGLPopulateUniformInfo(gl_shader *Shader)
{
	Shader->NumberOfUniforms = 0;
	// Get all the info about uniforms
    GLint I;
    GLint Count;
    
    GLint Size; // size of the variable
    GLsizei Length;
    glGetProgramiv(Shader->ProgramID, GL_ACTIVE_UNIFORMS, &Count);
    if (T_GL_SHADERS_MAX_UNIFORMS < Count)
    {
		DEBUG_LOG("ERROR: There are more uniforms than max allowed");
    }
	glUseProgram(Shader->ProgramID);
    for (I = 0; I < (Count <= T_GL_SHADERS_MAX_UNIFORMS ? Count : T_GL_SHADERS_MAX_UNIFORMS); I++)
    {
        glGetActiveUniform(Shader->ProgramID,
                           (GLuint)I,
                           T_GL_SHADERS_UNIFORM_NAME_MAX_LENGTH,
                           &Length,
                           &Size,
                           &Shader->Uniforms[Shader->NumberOfUniforms].Type,
                           Shader->Uniforms[Shader->NumberOfUniforms].Name);
        Shader->Uniforms[Shader->NumberOfUniforms].Location = glGetUniformLocation(Shader->ProgramID, Shader->Uniforms[Shader->NumberOfUniforms].Name);
        Shader->NumberOfUniforms++;
    }
}

inline void
OpenGLShaderSetV2f(gl_shader *Shader, const char *Name, v2f *Value)
{
	for(int i = 0; i < Shader->NumberOfUniforms; i++)
	{
		if(strcmp(Shader->Uniforms[i].Name, Name) == 0)
		{
			if(Shader->Uniforms[i].Type == GL_FLOAT_VEC2)
			{
				glUniform2fv(Shader->Uniforms[i].Location, 1, (f32*)Value);
			}
			else
			{
				DEBUG_LOG("ERROR OpenGLShaderSetV2f: Uniform doesnt match the type");
			}
		}
	}
}

inline void
OpenGLShaderSetFloat(gl_shader *Shader, const char *Name, f32 Value)
{
	for(int i = 0; i < Shader->NumberOfUniforms; i++)
	{
		if(strcmp(Shader->Uniforms[i].Name, Name) == 0)
		{
			if(Shader->Uniforms[i].Type == GL_FLOAT)
			{
				glUniform1f(Shader->Uniforms[i].Location, Value);
			}
			else
			{
				DEBUG_LOG("ERROR OpenGLShaderSetV2f: Uniform doesnt match the type");
			}
		}
	}
}

inline void
OpenGLSetGlobalsInShaders()
{
	for(int i = 0; i < GlobalNumberOfShaders; i++)
	{
		gl_shader *Shader = GlobalShaders + i;
		glUseProgram(Shader->ProgramID);
		v2f ScreenDimensions = { (f32)GlobalGame->Screen.Width, (f32)GlobalGame->Screen.Height };
		OpenGLShaderSetV2f(Shader, "ScreenDimensions", &ScreenDimensions);
	}
}

inline void
OpenGLDrawQuad(v2f Position, v2f Scale, f32 Angle)
{
	gl_shader *Shader = NULL;
	for(int i = 0; i < GlobalNumberOfShaders; i++)
	{
		if(strcmp(GlobalShaders[i].Name, "quad_shader") == 0)
		{
			Shader = GlobalShaders + i;
		}
	}
	if(Shader == NULL)
	{
		DEBUG_LOG("ERROR GLDrawQuad: quad_shader not found");
		return;
	}
	glUseProgram(Shader->ProgramID);
	OpenGLShaderSetV2f(Shader, "Position", &Position);
	OpenGLShaderSetV2f(Shader, "Scale", &Scale);
	OpenGLShaderSetFloat(Shader, "Angle", Angle);
	glBindVertexArray(GlobalQuadVao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

inline void
OpenGLDrawCircle(v2f Position, v2f Scale)
{
	gl_shader *Shader = NULL;
	for(int i = 0; i < GlobalNumberOfShaders; i++)
	{
		if(strcmp(GlobalShaders[i].Name, "circle_shader") == 0)
		{
			Shader = GlobalShaders + i;
		}
	}
	if(Shader == NULL)
	{
		DEBUG_LOG("ERROR GLDrawQuad: quad_shader not found");
		return;
	}
	glUseProgram(Shader->ProgramID);
	OpenGLShaderSetV2f(Shader, "Position", &Position);
	OpenGLShaderSetV2f(Shader, "Scale", &Scale);
	glBindVertexArray(GlobalQuadVao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

inline void
OpenGLClearScreen(f32 R, f32 G, f32 B, f32 A)
{
    glClearColor(R, G, B, A);
    glClear(GL_COLOR_BUFFER_BIT);
}

inline bool
Win32ReadEntireFile(const char *FileName, win32_file *FileStruct)
{
    FileStruct->Handle = CreateFileA(
                                     FileName,
                                     GENERIC_READ,
                                     FILE_SHARE_READ,
                                     NULL,
                                     OPEN_EXISTING,
                                     FILE_ATTRIBUTE_NORMAL,
                                     NULL
                                     );
	if (FileStruct->Handle == INVALID_HANDLE_VALUE) 
    {
		return false;
	}
	
    FileStruct->Size = GetFileSize(
                                   FileStruct->Handle,
                                   NULL
                                   );
	if (FileStruct->Size == INVALID_FILE_SIZE) 
    {
		CloseHandle(FileStruct->Handle);
		return false;
	}
    u8 *Buffer = (u8*)VirtualAlloc(NULL,FileStruct->Size,MEM_COMMIT,PAGE_READWRITE);
	DWORD BytesRead = 0;
	DWORD TotalNumberOfBytesRead  = 0;
	while(TotalNumberOfBytesRead < FileStruct->Size)
    {
		BOOL Result = ReadFile(
                               FileStruct->Handle,
                               (LPVOID)Buffer,
                               FileStruct->Size,
                               &BytesRead,
                               NULL
                               );
		if(Result == FALSE)
        {
			CloseHandle(FileStruct->Handle);
			VirtualFree(Buffer,FileStruct->Size,MEM_RELEASE);
			return false;
		}
		
        TotalNumberOfBytesRead = BytesRead;
	}
	ASSERT(TotalNumberOfBytesRead == FileStruct->Size);
	FileStruct->Data = Buffer;
	NumberOfOpenFileHanldes++;
	return true;
}

inline void
Win32CloseFile(win32_file *File)
{
	CloseHandle(File->Handle);
	VirtualFree(File->Data, File->Size, MEM_RELEASE);
	NumberOfOpenFileHanldes--;
}

inline FILETIME
Win32GetLastWriteTime(char *Filename)
{
    FILETIME LastWriteTime = {};
    
    WIN32_FILE_ATTRIBUTE_DATA Data;
    if(GetFileAttributesExA(Filename, GetFileExInfoStandard, &Data))
    {
        LastWriteTime = Data.ftLastWriteTime;
    }
    
    return(LastWriteTime);
}

inline void
Win32LoadGameDLL(game_code *GameCode)
{
    if (GameCode->Lib != NULL)
    {
        FreeLibrary(GameCode->Lib);
    }
    GameCode->FileTime = Win32GetLastWriteTime("game.dll");
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

inline void
Win32ReloadCode(game_code *GameCode)
{
    FILETIME NewTime = Win32GetLastWriteTime("game.dll");
    i32 Result = (CompareFileTime(&NewTime, &GameCode->FileTime) != 0);
    if (Result)
    {
        Win32LoadGameDLL(GameCode);
    }
}

inline bool
Win32FileExists(const char *FileName)
{
	HANDLE FileHandle = CreateFileA(FileName, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);
	if (FileHandle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(FileHandle);
		return true;
	}
	return false;
}

void GLFWFramebufferResizeCallback(GLFWwindow *Window, i32 Width, i32 Height)
{
    GlobalGame->Screen.Width = Width;
    GlobalGame->Screen.Height = Height;
    glViewport(0, 0, Width, Height);
}


inline void 
GLFWKeyInput(GLFWwindow* Window)
{
	i32 MinKey = 65;
	i32 MaxKey = 90;
	if(GlobalGame->GameInput.CharacterKey.IsDown == TRUE)
	{
		i32 CharacterKeyState = glfwGetKey(Window, GlobalGame->GameInput.CharacterKey.Key);
		if (CharacterKeyState != GLFW_PRESS)
		{
			GlobalGame->GameInput.CharacterKey.IsPressed = TRUE;
			GlobalGame->GameInput.CharacterKey.IsDown = FALSE;
			return;
		}
	}
	
	GlobalGame->GameInput.CharacterKey.IsDown = FALSE;
	GlobalGame->GameInput.CharacterKey.IsPressed = FALSE;
	for(i32 i = MinKey; i < MaxKey; i++)
	{
		i32 State = glfwGetKey(Window, i);
		if (State == GLFW_PRESS)
		{
			GlobalGame->GameInput.CharacterKey.IsDown = TRUE;
			GlobalGame->GameInput.CharacterKey.IsPressed = FALSE;
			GlobalGame->GameInput.CharacterKey.Key =(game_input_key) i;
		}
	}
}

inline void
Win32SaveGameState()
{
    HANDLE FileHandle = CreateFileA("Game.state", GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
	DWORD BytesWritten;
    if (FileHandle != INVALID_HANDLE_VALUE)
    {
        game_struct ToSave = *GlobalGame;
        WriteFile(FileHandle, &ToSave, sizeof(ToSave), &BytesWritten, 0);
    }
    CloseHandle(FileHandle);
}

inline bool
Win32GLCreateShaderFromFile(const char *VsFilePath, const char *FsFilePath, gl_shader *Shader)
{
	win32_file VsFile;
	win32_file FsFile;
	bool Result = false;
	for(;;){
		if (!Win32ReadEntireFile(VsFilePath, &VsFile) || !Win32ReadEntireFile(FsFilePath, &FsFile))
		{
			break;
		}
		if (!OpenGLCreateShader((const char**)&VsFile.Data,(const char**) &FsFile.Data, &Shader->ProgramID))
		{
			break;
		}
		Result = true;
		break;
	}
	Win32CloseFile(&VsFile);
	Win32CloseFile(&FsFile);
	return(Result);
}

inline PLAY_SOUND(PlaySound)
{
	GlobalSoundEngine->play2D((const char*)Name);
}

inline void
LoadAssets()
{
	GlobalNumberOfShaders = 0;
	win32_file AssetFile;
	if(!Win32ReadEntireFile("..\\game.asset", &AssetFile))
	{
		exit(-1);
	}
	asset Assets[50] = {};
	ParseAssetsFile(AssetFile.Data, Assets);
	Win32CloseFile(&AssetFile);
	for(asset *It = Assets; It != Assets + 50; It++)
	{
		if (GlobalSoundEngine != NULL && It->Type == AssetType_Sound)
		{
			win32_file SoundFile;
			if(Win32ReadEntireFile(It->Path, &SoundFile))
			{
				irrklang::ISoundSource *Source = GlobalSoundEngine->addSoundSourceFromMemory((void*)SoundFile.Data,
																							 SoundFile.Size,
																							 It->Name);
				Win32CloseFile(&SoundFile);
			}
		}
		if(It->Type == AssetType_Shader)
		{
			win32_file VsSource;
			win32_file FsSource;
			const char *ExtensionVs = "_vs.c";
			const char *ExtensionFs = "_fs.c";
			char FilePathVs[512];
			char FilePathFs[512];
			strcpy(FilePathVs, It->Path);
			strcat(FilePathVs, ExtensionVs);
			strcpy(FilePathFs, It->Path);
			strcat(FilePathFs, ExtensionFs);
			bool VsFileOkay = Win32ReadEntireFile(FilePathVs, &VsSource);
			bool FsFileOkay = Win32ReadEntireFile(FilePathFs, &FsSource);
			if(VsFileOkay && FsFileOkay)
			{
				strcpy(GlobalShaders[GlobalNumberOfShaders].Name, It->Name);
				OpenGLCreateShader((const char**)&VsSource.Data,
								   (const char**)&FsSource.Data,
								   &GlobalShaders[GlobalNumberOfShaders].ProgramID);
				GlobalNumberOfShaders++;
			}
			if(VsFileOkay)
			{
				Win32CloseFile(&VsSource);
			}
			if(FsFileOkay)
			{
				Win32CloseFile(&FsSource);
			}
			
		}
	}
	for(int i = 0; i < GlobalNumberOfShaders; i++)
	{
		OpenGLPopulateUniformInfo(&GlobalShaders[i]);
	}
}

int 
CALLBACK WinMain(HINSTANCE instance,
                 HINSTANCE prevInstance,
                 LPSTR commandLine,
                 int showCode)
{
	i32 WindowWidth = 800;
	i32 WindowHeight = 800;
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
    glfwSetFramebufferSizeCallback(Window, GLFWFramebufferResizeCallback);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        glfwTerminate();
        return -1;
    }
	RandomInitialize();
	GlobalSoundEngine = irrklang::createIrrKlangDevice();
	LoadAssets();
	GlobalQuadVao = OpenGLCreateQuadTextureBuffer();
	GlobalGame = new game_struct;
	game_code GameCode = {};
    Win32LoadGameDLL(&GameCode);
    GameCode.GameInitCallback(GlobalGame);
	if (Win32FileExists("Game.state"))
	{
		win32_file GameStateFile;
		if (Win32ReadEntireFile("Game.state", &GameStateFile))
		{
			game_struct SavedState = *((game_struct*)GameStateFile.Data);
			*GlobalGame = SavedState;
			Win32CloseFile(&GameStateFile);
		}
	}
	GlobalGame->Screen.Width = WindowWidth;
	GlobalGame->Screen.Height = WindowHeight;
    GlobalGame->ClearScreen = OpenGLClearScreen;
	GlobalGame->DrawQuad = OpenGLDrawQuad;
	GlobalGame->DrawCircle = OpenGLDrawCircle;
	OpenGLSetGlobalsInShaders();
	if(GlobalSoundEngine != NULL)
	{
		GlobalGame->PlaySound = PlaySound;
	}
	else
	{
		GlobalGame->PlaySound = PlaySoundStub;
	}
	f32 KeyTimer = 0;
	f32 OldTime = glfwGetTime();
	f32 Time = 0;
    while(!glfwWindowShouldClose(Window) && GlobalRunning)
	{
		f32 CurrentTime = glfwGetTime();
		f32 ElapsedTime = CurrentTime - OldTime;
		Time += ElapsedTime;
		OldTime = CurrentTime;
		GLFWKeyInput(Window);
        GameCode.GameUpdateCallback(GlobalGame, ElapsedTime, Time);
        glfwSwapBuffers(Window);
		
#ifdef TYPING_INTERNAL
		if(glfwGetKey(Window, GLFW_KEY_R) == GLFW_PRESS && KeyTimer <= 0)
		{
			KeyTimer = .1;
			LoadAssets();
			OpenGLSetGlobalsInShaders();
			GameCode.GameInitCallback(GlobalGame);
		}
#endif
        glfwPollEvents();
		Win32ReloadCode(&GameCode);
		if(KeyTimer > 0)
		{
			KeyTimer -= ElapsedTime;
		}
    }
	if(GlobalSoundEngine != NULL)
	{
		GlobalSoundEngine->drop();
	}
	Win32SaveGameState();
	glfwTerminate();
	// TODO(Banni): Check if we dint close any file just for debugging purposes.
	// Its good to not lock the files while we are playing the game.
	ASSERT(NumberOfOpenFileHanldes == 0);
    return 0;
}
