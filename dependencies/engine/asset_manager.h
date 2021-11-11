#pragma once 

struct AssetManager{
	// Use a map to add assets.
	// Reference assets by a string.
	// Save only textures for the moment.
};

void get_texture(&AssetManager *asset_manager, const char *texture_name);