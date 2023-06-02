#include <stdio.h>
typedef char u8;
typedef unsigned int u32;
#include "asset.h"
#include "asset.cpp"
int main()
{
	const char *AssetFileText = "something shader /somewhere/in/the/file\nsomething shader /somewhere/in/the/file";
	LoadAssets((u8*)AssetFileText);
	return 1;
}