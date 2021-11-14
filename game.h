#pragma once

#include "engine/time.h"
#include "engine/renderer.h"
#include "engine/asset_manager.h"
#include "engine/sprite.h"

#include "solitaire.h"

struct Game{
	void UpdateGame(float dt);
	void DrawGame(float dt, float fps);
	void GameLoop(float dt, float fps);
	Game(Renderer *r, Window *w);
	
	Renderer *renderer;
	Window *window;
	bool showFPS = true;
	
	static AssetManager asset_manager;
	Board game_board;
	
	Sprite background;
	Sprite a;
	// Sprite cards;
};
