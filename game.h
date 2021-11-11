#pragma once

#include "engine/time.h"
#include "engine/renderer.h"
#include "solitaire.h"

struct Game{
	void UpdateGame(float dt);
	void DrawGame(float dt, float fps);
	void GameLoop(float dt, float fps);
	Game(Renderer *r, Window *w);
	
	Renderer *renderer;
	Window *window;
	bool showFPS = true;
	
	Board game_board;
	Texture background = make_texture("assets/textures/background.jpg");
	Texture cards      = make_texture("assets/textures/Solitaire Cards.png");

};
