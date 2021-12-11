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
V2 mouse_card_pos_delta;
LinkedList<Card> *hovered_list = NULL;
LinkedListNode<Card> *node_to_split_from = NULL;
Card *hovered_card = NULL;


void Game::UpdateGame(float dt){
    mouse = GetMouseInfo(window);
    V2 mouse_pos = {(float)mouse.x, (float)mouse.y};
    
    
    switch(mouse.left.state){
        case MOUSE_PRESSED:{
			bool break_out = false;
            ////////////////////Check if mouse is clicked on a tableau's card////////////////////////////////////
            hovered_card = NULL;
            hovered_list = NULL;
            node_to_split_from = NULL;
            // game_board.held_cards_origin = NULL;
            
            for(int i = 0; i < TABLEAU_SIZE; i++){
                LinkedList<Card> *card_list = &game_board.tableau[i];
                
                LinkedListNode<Card> *previous_node = NULL;
                LinkedListNode<Card> *current_node = card_list->first;
                
                int card_counter = 0;
                while(current_node){
                    Card *card = &current_node->data;
                    hovered_list = card_list;
                    if(DoRectContainsPoint(card->clickable_area, mouse_pos)){
						if(!card->flipped){
							hovered_card = card;
							game_board.is_tableau_card_held  = true;
							node_to_split_from = previous_node;
							// clear_list(&game_board.held_cards);
							
							mouse_card_pos_delta = {mouse_pos.x - hovered_card->position.x, mouse_pos.y - hovered_card->position.y};
							if(card_counter != 0){
								// printf("%d\n",i);
								
								// printf("Selected card: %d, %d\n", (int)hovered_card->type, hovered_card->value);
					
								// printf("Before splitting\n");
								// print_linked_list(hovered_list);
								
								assert(node_to_split_from);
								// game_board.held_cards_origin = hovered_list;
								split_list(&game_board.held_cards, hovered_list, node_to_split_from);
								// print_linked_list(hovered_list);

									
								
							}else{
								
								//If we grab the bottom card, we dont split, we copy the origin list and clear it.
								append_list(&game_board.held_cards, hovered_list);
								// print_linked_list(&game_board.held_cards);
								clear_list(hovered_list);
								// print_linked_list(hovered_list);

								
							}
							
							// Recalculate the clickable areas.
							calculate_tableau_cards_positions_and_clickable_areas(&game_board);
							break_out = true;
							break;
							
						}else if(card_counter == hovered_list->size - 1){
							card->flipped = false;
							break_out = true;
							break;
						}

                    
					}
                    card_counter++;
                    previous_node = current_node;
                    current_node = current_node->next;
                }
				if(break_out) break;
            }
            
            /////////////////////Check if the stock is clicked on////////////////////
            if(DoRectContainsPoint(game_board.stock.bounding_box, mouse_pos)){
                
                if(game_board.hand.size > 0){
					if(game_board.stock_cycle_completed){
						game_board.previous_hand_card = NULL;
						game_board.current_stock_card = game_board.hand.first;
						game_board.stock_cycle_completed = false;
					}else{
						assert(game_board.current_stock_card);
						game_board.previous_hand_card = game_board.current_stock_card;
						game_board.current_stock_card = game_board.current_stock_card->next;

						if(!game_board.current_stock_card){
						game_board.stock_cycle_completed = true;
						
						}
					}
					
				}
                
            } 
            
            //////////Check if a hand card is clicked on/////////////////
            if(DoRectContainsPoint(game_board.hand_card_bounding_box, mouse_pos) && !game_board.stock_cycle_completed > 0 && !game_board.is_hand_card_held){
                add_node(&game_board.held_cards, game_board.current_stock_card->data);
				game_board.current_stock_card = game_board.current_stock_card->next;
				
				mouse_card_pos_delta = {mouse_pos.x - game_board.hand_card_bounding_box.x, mouse_pos.y - game_board.hand_card_bounding_box.y};
                    
                if(game_board.previous_hand_card){
                    delete_node_after(&game_board.hand, game_board.previous_hand_card);
                }else{
                    pop_from_list(&game_board.hand);
                }
                
                game_board.is_hand_card_held = true;
				
				if(!game_board.current_stock_card){
					game_board.stock_cycle_completed = true;
				}
				
            }
            
            
            
            break;
        }
        
        

        
        case MOUSE_RELEASED:{
			// print_linked_list(&game_board.tableau[0]);
			LinkedList<Card> *card_list = NULL;
			Card *held_card;
			bool return_to_origin = true;
			if(game_board.held_cards.size > 0){
				held_card = &game_board.held_cards.first->data;
				
			}
			
			if(held_card){
				bool break_out_case = false;
				// When a card or stack of cards gets placed on an empty place break out.
				for(int i = 0; i < TABLEAU_SIZE; i++){
					LinkedList<Card> *tableau_stack = &game_board.tableau[i];

					if(DoRectContainsPoint(game_board.empty_stacks_bboxes[i], mouse_pos) && tableau_stack->size == 0 && hovered_list != tableau_stack){
						append_list(tableau_stack, &game_board.held_cards);
						
						break_out_case = true;
						

						break;
					}
				}
				
				// If mouse is released on top of a foundation we check if it can be added.
				for(int i = 0; i < CardType::COUNT; i++){
					LinkedList<Card> *foundation = &game_board.foundations[i];
					Rect foundation_bbox = {game_board.foundations_x_positions[i], game_board.foundations_y_position, game_board.cards_size.x, game_board.cards_size.y};

					if(DoRectContainsPoint(foundation_bbox, mouse_pos) && game_board.held_cards.size == 1){
						Card *current_foundation_card = &foundation->last_node->data;
						Card *held_card = &game_board.held_cards.first->data;
						
						if(foundation->size > 0){
							if((int)held_card->type == i && held_card->value - 1 == current_foundation_card->value){
								add_node(foundation, *held_card);
								break_out_case = true;
								break;
							}
							
						}else{
							if((int)held_card->type == i && held_card->value == 0){
								add_node(foundation, *held_card);
								break_out_case = true;
							}
						}
						
					}
				}
				
				if(break_out_case){
					game_board.is_tableau_card_held = false;
					game_board.is_hand_card_held = false;
					
					calculate_tableau_cards_positions_and_clickable_areas(&game_board);
					clear_list(&game_board.held_cards);
					break;
				}
			}
			
			
			
            if(game_board.is_tableau_card_held){
				
				
				if(maybe_add_card_to_tableau(&game_board, mouse_pos, card_list)){
					append_list(card_list, &game_board.held_cards);
					return_to_origin = false;
					// calculate_tableau_cards_positions_and_clickable_areas(&game_board);

				}
				
				if(return_to_origin){
					append_list(hovered_list, &game_board.held_cards);
					
				}
                clear_list(&game_board.held_cards);
                
                // Recalculate the clickable areas when we release a grabbed card or group of cards from the tableau..
                calculate_tableau_cards_positions_and_clickable_areas(&game_board);
				game_board.is_tableau_card_held = false;
				
            }else if(game_board.is_hand_card_held){
				// If the the card taken from the hand is above a tableau stack we add it to it.
				// bool return_to_hand = true;
				
				if(maybe_add_card_to_tableau(&game_board, mouse_pos, card_list)){
					add_node(card_list, *held_card);
					return_to_origin = false;
					calculate_tableau_cards_positions_and_clickable_areas(&game_board);

				}
				
				// If the card is not above a tableau stack we return it to the hand. 
				if(return_to_origin){
					LinkedListNode<Card> *card;
					if(game_board.previous_hand_card){
						card = add_node_after(&game_board.hand, game_board.held_cards.first->data, game_board.previous_hand_card);
						
					}else{
						card = add_node_to_beginning(&game_board.hand, game_board.held_cards.first->data);
					}
					
					game_board.current_stock_card = card;
					game_board.stock_cycle_completed = false;
					
				}
				
				clear_list(&game_board.held_cards);
				game_board.is_hand_card_held = false;
				
            }
            
            break;
        }
    }
    
    
}

void Game::DrawGame(float dt, float fps){
    // Render the board//
    render_sprite_as_background(renderer, &background);
    draw_tableau(&game_board, renderer);
    draw_foundations(&game_board, renderer);
    draw_stock(&game_board, renderer);
    
    // Draw the stock and hand.
    static V2 pos = {game_board.hand_card_bounding_box.x, game_board.hand_card_bounding_box.y};

    
    if(!game_board.stock_cycle_completed){
        Card *card = &game_board.current_stock_card->data;
        int card_index = card->type * CARDS_PER_TYPE + card->value;
        render_sprite(renderer, &game_board.card_sprites[card_index], pos);
    }else{
        render_sprite(renderer, &game_board.foundation_sprite, pos);
    }
    
    
    //Render the held cards.
    switch(mouse.left.state){
		case MOUSE_PRESSED:
        case MOUSE_HELD:{
            V2 mouse_pos = {(float)mouse.x, (float)mouse.y};
            draw_held_cards(&game_board, renderer, mouse_pos, mouse_card_pos_delta);
            
            
            break;
        }
    }
	
	// Uncomment this to see the bounding boxes that are used when the tableau stacks are empty to place cards in the empty space.
	// for(int i = 0; i < TABLEAU_SIZE; i++){
		// render_quad(renderer, &game_board.empty_stacks_bboxes[i], NULL);
		
	// }
	
	// for(int i = 0; i < CardType::COUNT; i++){
		// Rect foundation_bbox = {game_board.foundations_x_positions[i], renderer->window->internalHeight - game_board.foundations_y_padding, game_board.cards_size.x, game_board.cards_size.y};
		// render_quad(renderer, &foundation_bbox, NULL);
		
	// }
    
    
    ////////////////////
    
    renderer_draw(renderer);
    swap_buffers(window);
}

void Game::GameLoop(float dt, float fps){
    UpdateGame(dt);
    DrawGame(dt, fps);
    poll_events();
}