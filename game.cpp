#include "game.h"
#include "engine/window.h"
#include "engine/input.h"
#include "engine/collision.h"
// #include "engine/sprite.h"

#include "GLFW/glfw3.h"

#include <stdlib.h>

AssetManager Game::asset_manager;

Game::Game(Renderer *r, Window *w){
	
	renderer = r;
	window = w;
	
	add_texture(&asset_manager, "Background", make_texture("assets/textures/background.jpg"));
	add_texture(&asset_manager, "Cards", make_texture("assets/textures/Solitaire Cards.png"));
	
	background.texture = get_texture(&asset_manager, "Background");
	
	init_board(&game_board, w);
}

void Game::UpdateGame(float dt){
	MouseInfo mouse = GetMouseInfo(window);
	PrintMouseInfo(&mouse);
	// V2 point = {mouse.x, mouse.y};
	// Rect card_area = game_board.tableau[1].first->data.clickable_area;
	// printf("Mouse: %f,%f     Area: %f,%f\n", mouse.x, mouse.y, card_area.x, card_area.y);
	// if(DoRectContainsPoint(card_area, point)){
		// printf("In card area\n");
	// }
}

static Rect r = {100,100,64,64};
void Game::DrawGame(float dt, float fps){
	// Render the board//
	render_sprite_as_background(renderer, &background);
	draw_tableau(&game_board, renderer);
	draw_foundations(&game_board, renderer);
	////////////////////
	
	renderer_draw(renderer);
	swap_buffers(window);
}

void Game::GameLoop(float dt, float fps){
	UpdateGame(dt);
	DrawGame(dt, fps);
	poll_events();
}