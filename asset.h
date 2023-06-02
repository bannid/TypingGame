/* date = June 2nd 2023 7:49 am */

#ifndef ASSET_H
#define ASSET_H

enum enum_asset_type
{
	AssetType_Shader,
	AssetType_Sound,
	AssetType_Image
};
struct asset_file_token
{
	u32 Length;
	char Token[512];
};
struct asset
{
	enum_asset_type Type;
	char Name[20];
	char Path[512];
};

#endif //ASSET_H
