#include <stdio.h>
typedef char u8;
typedef unsigned int u32;
#define DEBUG_LOG(log)
#include "asset.h"
#include "asset.cpp"
int main()
{
	const char *AssetFileText = "something shader /somewhere/in/the/file\nsomething shader /somewhere/in/the/file";
	asset Assets[50];
	ParseAssetsFile((u8*)AssetFileText, Assets);
	return 1;
}