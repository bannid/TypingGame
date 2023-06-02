#include <string.h>

#define ASSET_EOF 0
void
LoadAssets(u8 *AssetFileText)
{
	u8 *BasePointer = AssetFileText;
	u8 *Cursor = BasePointer;
	while(*Cursor == '\n' || *Cursor == ' ' || *Cursor == '\t' || *Cursor == '\r')
	{
		Cursor++;
		if (*Cursor == ASSET_EOF)
		{
			return;
		}
	}
	// Right now lets say we only have 100 assets
	asset_file_token Tokens[100 * 3];
	asset Assets[100];
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
	for(int i = 0; i < NumberOfTokens; i+=3)
	{
		strcpy(Assets[NumberOfAssets].Name, Tokens[i].Token);
		strcpy(Assets[NumberOfAssets].Path, Tokens[i + 2].Token);
		if(strcmp(Tokens[i + 1].Token, "shader") == 0){
			Assets[NumberOfAssets].Type = AssetType_Shader;
		}
		else if(strcmp(Tokens[i + 1].Token, "image") == 0)
		{
			Assets[NumberOfAssets].Type = AssetType_Image;
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
}
