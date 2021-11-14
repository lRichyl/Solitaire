#include "game.h"
#include "engine/window.h"
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
	a.texture = get_texture(&asset_manager, "Cards");
	
	init_board(&game_board, w);
	// game_board.card_sprites[51].position = {500,500};
	// game_board.flipped_card.position = {200,200};
	// game_board.foundation_sprite.position = {200,200};
}

void Game::UpdateGame(float dt){
	
}
// float empty_space = (1280.0f - (168.0f * 7.f));
// float padding = (empty_space) / 7.f;

static Rect pos = {100,100,42,60};
void Game::DrawGame(float dt, float fps){
	render_sprite_as_background(renderer, &background);
	draw_tableau(&game_board, renderer);
	draw_foundations(&game_board, renderer);
	
	renderer_draw(renderer);
	swap_buffers(window);
}

void Game::GameLoop(float dt, float fps){
	UpdateGame(dt);
	DrawGame(dt, fps);
	poll_events();
}