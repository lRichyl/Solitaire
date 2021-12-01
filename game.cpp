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

MouseInfo mouse;
V2 delta;

void Game::UpdateGame(float dt){
	mouse = GetMouseInfo(window);
	V2 mouse_pos = {(float)mouse.x, (float)mouse.y};
	LinkedList<Card> *hovered_list = NULL;
	LinkedListNode<Card> *node_to_split_from = NULL;
	Card *hovered_card = NULL;
	
	// PrintMouseInfo(&mouse);
	
	switch(mouse.left.state){
		case MOUSE_PRESSED:{
			// get_list_and_card_mouse_is_over(&game_board, &mouse, &hovered_list, &hovered_card, &node_to_split_from);
			hovered_card = NULL;
			hovered_list = NULL;
			node_to_split_from = NULL;
			game_board.held_cards_origin = NULL;
			for(int i = 0; i < TABLEAU_SIZE; i++){
				LinkedList<Card> *card_list = &game_board.tableau[i];
				
				LinkedListNode<Card> *previous_node = NULL;
				LinkedListNode<Card> *current_node = card_list->first;
				
				int card_counter = 0;
				while(current_node){
					Card *card = &current_node->data;
					
					// V2 mouse_pos = {(float)mouse.x, (float)mouse.y};
					if(DoRectContainsPoint(card->clickable_area, mouse_pos)){
						hovered_card = card;
						hovered_list = card_list;
						node_to_split_from = previous_node;
						clear_list(&game_board.held_cards);
						
						delta = {mouse_pos.x - hovered_card->position.x, mouse_pos.y - hovered_card->position.y};
						if(card_counter != 0){
							// printf("%d\n",i);
							
							
							// printf("Selected card: %d, %d\n", (int)hovered_card->type, hovered_card->value);
				
							// printf("Before splitting\n");
							// print_linked_list(hovered_list);
							
							assert(node_to_split_from);
							game_board.held_cards_origin = hovered_list;
							split_list(&game_board.held_cards, hovered_list, node_to_split_from);
								
							break;
						}else{
							
							//If we grab the bottom card, we dont split, we copy the origin list and clear it.
							append_list(&game_board.held_cards, hovered_list);
							print_linked_list(&game_board.held_cards);
							clear_list(hovered_list);
							break;
						}
							

					}
					card_counter++;
					previous_node = current_node;
					current_node = current_node->next;
				}
			}
			
			
			break;
		}
		
		
		
		case MOUSE_RELEASED:{
			
			
			break;
		}
	}
	
	
}

static Rect r = {100,100,64,64};
void Game::DrawGame(float dt, float fps){
	// Render the board//
	render_sprite_as_background(renderer, &background);
	draw_tableau(&game_board, renderer);
	draw_foundations(&game_board, renderer);
	
	switch(mouse.left.state){
		case MOUSE_HELD:{
			V2 mouse_pos = {(float)mouse.x, (float)mouse.y};
			draw_held_cards(&game_board, renderer, mouse_pos, delta);
			// print_linked_list(&game_board.held_cards);
			
			// PrintMouseInfo(&mouse);
			
			break;
		}
	}
	
	////////////////////
	
	renderer_draw(renderer);
	swap_buffers(window);
}

void Game::GameLoop(float dt, float fps){
	UpdateGame(dt);
	DrawGame(dt, fps);
	poll_events();
}