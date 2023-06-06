/**
Temp program to compile and link shaders until we can have something built
into the game itself to monitor the shaders.
*/
#include <windows.h>
#include <string.h>
#include <stdio.h>
#include <glad.h>
#include "glad.c"
#include <glfw3.h>
#include "win32_fileapi.h"
#include "typinggame_debug.h"
#include "asset.cpp"
typedef int i32;
typedef unsigned int u32;

inline void 
PrintError(const char *Log)
{
	fprintf(stderr, Log);
}

inline void
Win32CloseFile(win32_file *File)
{
	CloseHandle(File->Handle);
	VirtualFree(File->Data, File->Size, MEM_RELEASE);
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
	return true;
}

int main()
{
	i32 WindowWidth = 800;
	i32 WindowHeight = 800;
	const char *WindowName = "ShaderCompiler";
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
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        glfwTerminate();
        return -1;
    }
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
		if(It->Type == AssetType_Shader)
		{
			u32 ShaderID;
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
				
				char Log[1024];
				int Success;
				GLint VS;
				GLint FS;
				//Vertex shader
				VS = glCreateShader(GL_VERTEX_SHADER);
				glShaderSource(VS, 1,(const char**) &VsSource.Data, NULL);
				glCompileShader(VS);
				glGetShaderiv(VS, GL_COMPILE_STATUS, &Success);
				//Check if the vertex shader compiled
				if (!Success)
				{
					glGetShaderInfoLog(VS, 1024, NULL, Log);
					PrintError(Log);
					continue;
				}
				//Fragment shader
				FS = glCreateShader(GL_FRAGMENT_SHADER);
				glShaderSource(FS, 1,(const char**) &FsSource.Data, NULL);
				glCompileShader(FS);
				glGetShaderiv(FS, GL_COMPILE_STATUS, &Success);
				//Check if the fragment shader compiled successfully
				if (!Success)
				{
					glGetShaderInfoLog(FS, 1024, NULL, Log);
					PrintError(Log);
					continue;
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
					PrintError(Log);
					
				}
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
	glfwTerminate();
	return 1;
}