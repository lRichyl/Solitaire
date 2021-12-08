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
                    
                    if(DoRectContainsPoint(card->clickable_area, mouse_pos) && !card->flipped){
                        hovered_card = card;
                        hovered_list = card_list;
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
                        break;

                    }
                    card_counter++;
                    previous_node = current_node;
                    current_node = current_node->next;
                }
            }
            
            /////////////////////Check if the stock is clicked on////////////////////
            if(DoRectContainsPoint(game_board.stock.bounding_box, mouse_pos)){
                
                
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
            
            //////////Check if a hand card is clicked on/////////////////
            if(DoRectContainsPoint(game_board.hand_card_bounding_box, mouse_pos) && !game_board.stock_cycle_completed > 0 && !game_board.is_hand_card_held){
                add_node(&game_board.held_cards, game_board.current_stock_card->data);
				game_board.current_stock_card = game_board.current_stock_card->next;
                    
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
            if(hovered_list){
                append_list(hovered_list, &game_board.held_cards);
                clear_list(&game_board.held_cards);
                
                // Recalculate the clickable areas.
                calculate_tableau_cards_positions_and_clickable_areas(&game_board);
            }else if(game_board.is_hand_card_held){
                LinkedListNode<Card> *card;
                if(game_board.previous_hand_card){
                    card = add_node_after(&game_board.hand, game_board.held_cards.first->data, game_board.previous_hand_card);
                    
                }else{
                    card = add_node_to_beginning(&game_board.hand, game_board.held_cards.first->data);
                }
				
				game_board.current_stock_card = card;
				
			
                clear_list(&game_board.held_cards);
                game_board.is_hand_card_held = false;
				game_board.stock_cycle_completed = false;
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
        case MOUSE_HELD:{
            V2 mouse_pos = {(float)mouse.x, (float)mouse.y};
            draw_held_cards(&game_board, renderer, mouse_pos, mouse_card_pos_delta);
            
            
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