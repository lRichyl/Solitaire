#include "game.h"
#include "engine/window.h"
#include "GLFW/glfw3.h"

#include <stdlib.h>


Game::Game(Renderer *r, Window *w){
	
	renderer = r;
	window = w;
	
	init_board(&game_board);
}

void Game::UpdateGame(float dt){
	
}

static Rect pos = {100,100,100,100};
void Game::DrawGame(float dt, float fps){
	render_quad(renderer, NULL, &background);

	
	renderer_draw(renderer);
	swap_buffers(window);
}

void Game::GameLoop(float dt, float fps){
	UpdateGame(dt);
	DrawGame(dt, fps);
	poll_events();
}