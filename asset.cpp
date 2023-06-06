#include <string.h>
#include "asset.h"
#define ASSET_EOF 0
bool
ParseAssetsFile(u8 *AssetFileText, asset *Assets)
{
	u8 *BasePointer = AssetFileText;
	u8 *Cursor = BasePointer;
	while(*Cursor == '\n' || *Cursor == ' ' || *Cursor == '\t' || *Cursor == '\r')
	{
		Cursor++;
		if (*Cursor == ASSET_EOF)
		{
			return true;
		}
	}
	// TODO(Banni): Fix this!!! If we have more assets than this,
	// then we will have undefined behaviour
	// Right now lets say we only have 100 assets
	asset_file_token Tokens[100 * 3];
	u32 NumberOfTokens = 0;
	u32 NumberOfAssets = 0;
	while(1)
	{
		asset_file_token Token = {};
		while(1)
		{
			Token.Token[Token.Length++] = *Cursor;
			Cursor++;
			if(*Cursor == '\n' || 
			   *Cursor == ' '  || 
			   *Cursor == '\t' || 
			   *Cursor == '\r' ||
			   *Cursor == ASSET_EOF)
			{
				while(*Cursor == '\n' || *Cursor == ' ' || *Cursor == '\t' || *Cursor == '\r')
				{
					Cursor++;
					if (*Cursor == ASSET_EOF)
					{
						break;
					}
				}
				break;
			}
		}
		Tokens[NumberOfTokens++] = Token;
		if(*Cursor == ASSET_EOF)
		{
			break;
		}
	}
	if (NumberOfTokens % 3 != 0)
	{
		return false;
	}
	for(int i = 0; i < NumberOfTokens; i+=3)
	{
		strcpy(Assets[NumberOfAssets].Name, Tokens[i].Token);
		strcpy(Assets[NumberOfAssets].Path, Tokens[i + 2].Token);
		if(strcmp(Tokens[i + 1].Token, "shader") == 0){
			Assets[NumberOfAssets].Type = AssetType_Shader;
		}
		else if(strcmp(Tokens[i + 1].Token, "texture") == 0)
		{
			Assets[NumberOfAssets].Type = AssetType_Texture;
		}
		else if(strcmp(Tokens[i + 1].Token, "sound") == 0)
		{
			Assets[NumberOfAssets].Type = AssetType_Sound;
		}
		else{
			//DEBUG_LOG("Unknown asset type");
		}
		NumberOfAssets++;
	}
	return true;
}
