#include "solitaire.h"
#include "engine/collision.h"
#include "game.h"
#include <stdlib.h>

//Template requires this function defined.
void print_data(Card *card){
    printf("{%d , %d}\n", card->type, card->value);
}

void calculate_tableau_cards_positions_and_clickable_areas(Board *board){
    for(int i = 0; i < TABLEAU_SIZE; i++){
        // float base_y_padding = 37.0f;
        float sum_y_padding = board->base_y_padding;
        LinkedList<Card> *card_stack = &board->tableau[i];
        LinkedListNode<Card> *previous_node;
        LinkedListNode<Card> *current_node = card_stack->first;
        while(current_node){
            Card *card = &current_node->data;
            card->position = {board->tableau_x_positions[i], board->tableau_y_starting_pos - sum_y_padding};
            sum_y_padding += board->base_y_padding;
            
            V2 position = card->position;
            V2 size = board->cards_size;
            if(current_node == card_stack->last_node){
                card->clickable_area = {position.x, position.y, size.x, size.y};
            }else{
                card->clickable_area = {position.x, position.y, size.x, board->base_y_padding};
            }
            
            
                
            
            previous_node = current_node;
            current_node = current_node->next;
        }
        
    }
}


void init_board(Board *board, Window *window){
    //Init the plateau, foundations and hand.
    
    //When the game is restarted, this lists have to be cleared.
    for(int i = 0; i < TABLEAU_SIZE; i++){
        init_linked_list(&board->tableau[i], MAX_STACK_SIZE);
    }
    
    for(int i = 0; i < CardType::COUNT; i++){
        init_linked_list(&board->foundations[i], CARDS_PER_TYPE);
    }
    
    init_linked_list(&board->hand, MAX_HAND_SIZE);
    
    // TODO: This should be done every time we want to start a game;
    shuffle_cards_to_the_board(board);
    
    
    
    // We initialize the sprites for all the 52 cards from a single texture.
    int initial_x_padding = 11;
    int initial_y_padding = 2;
    int x_padding_between_cards = 22;
    int y_padding_between_cards = 4;
    V2 original_card_size = {42,60};
    for(int j = 0; j < CardType::COUNT; j++){
        for(int i = 0; i < CARDS_PER_TYPE; i++){
            int index = j * CARDS_PER_TYPE + i; 
            Sprite *spr = &board->card_sprites[index];
            spr->size = board->cards_size;
            spr->texture = get_texture(&Game::asset_manager, "Cards");
            
            float clip_x_pos = initial_x_padding + (i * original_card_size.x) + (i * x_padding_between_cards);
            float clip_y_pos = initial_y_padding + (j * original_card_size.y) + (j * y_padding_between_cards);
            spr->clipping_box = {clip_x_pos, clip_y_pos, original_card_size.x, original_card_size.y};
        }
    }
    
    // Initialize the sprite for the flip side of the cards.
    board->flipped_card.size = board->cards_size;
    board->flipped_card.texture = get_texture(&Game::asset_manager, "Cards");
    board->flipped_card.clipping_box = {843, 66, original_card_size.x, original_card_size.y};
    
    board->foundation_sprite.size = board->cards_size;
    board->foundation_sprite.texture = get_texture(&Game::asset_manager, "Cards");
    board->foundation_sprite.clipping_box = {843, 2, original_card_size.x, original_card_size.y};
    
    // This are used to rendered the tableau cards spaced correctly. 
    board->tableau_empty_space = window->internalWidth - (board->cards_size.x * TABLEAU_SIZE);
    board->padding = board->tableau_empty_space / TABLEAU_SIZE;
    board->starting_x_padding = board->padding/2.f;
    
    for(int i = 0; i < TABLEAU_SIZE; i++){
        board->tableau_x_positions[i] = board->starting_x_padding + (i * board->padding) + (board->cards_size.x * i);
    }
    
    int j = 3;
    for(int i = 0; i < CardType::COUNT; i++){
        // board->tableau_x_positions[i] = (board->padding/2.f) + (i * board->padding) + (board->cards_size.x * i);
        board->foundations_x_positions[i] = board->tableau_x_positions[j + i];
    }
	board->foundations_y_position = window->internalHeight - board->foundations_y_padding;
    
    // This should be called every time a group of cards is moved to another stack to regenerate their positions and clickable areas.
    calculate_tableau_cards_positions_and_clickable_areas(board);
    
	for(int i = 0; i < TABLEAU_SIZE; i++){
		float tableau_x = board->tableau_x_positions[i];
		float y = board->tableau_y_starting_pos - board->base_y_padding;
		board->empty_stacks_bboxes[i] = {tableau_x, y, board->cards_size.x, board->cards_size.y};
	}
        
    // The stock is initialized. This is just the the part you click on to get the next card.
    board->stock.bounding_box = {board->starting_x_padding, board->foundations_y_position, board->cards_size.x, board->cards_size.y};
    Rect *bbox = &board->stock.bounding_box;
    board->stock.sprite = &board->flipped_card;

    board->hand_card_bounding_box = {bbox->x + bbox->w + board->padding, bbox->y, board->cards_size.x, board->cards_size.y};
    // board->current_stock_card = board->hand.first;
    
    init_linked_list(&board->held_cards, 30);
	init_linked_list(&board->done_actions, 500);
    
}


void shuffle_cards_to_the_board(Board *board){
    // First we fill a temporary cards list to randomly pick from.
    LinkedList<Card> cards;
    init_linked_list(&cards, CARDS_TOTAL);
    for(int j = 0; j < CardType::COUNT; j++){
        for(int i = 0; i < CARDS_PER_TYPE; i++){
            Card card;
            card.type = (CardType)j;
            card.value = i;
            add_node(&cards, card);
        }
    }
    
    // Randomly distribute cards on the 7 stacks of the plateau.
    for(int j = 0; j < TABLEAU_SIZE; j++){
        bool first = true;
        for(int i = 0; i < TABLEAU_SIZE - j; i++){
            int random_card = rand() % cards.size;
            
            // This find_node_by_position and delete_node_by_position are slow but we don't care because it is only run once at the start of the game.
            Card *card = find_node_by_position(&cards, random_card);
            if(first){
                card->flipped = false;
            }else{
                card->flipped = true;
            }
            first = false;
			card->origin = &board->tableau[j + i];
            add_node(&board->tableau[j + i], *card);
            delete_node_by_position(&cards, random_card);
        }
    }
    
    // for(int i = 0; i < TABLEAU_SIZE; i++){
        // print_linked_list(&board->tableau[i]);
    // }
    // printf("Cards left: %d\n", cards.size);
    
    
    // Randomly insert the rest of the cards in the hand.
    for(int i = 0; i < MAX_HAND_SIZE; i++){
        int random_card = rand() % cards.size;
        // int foundation_index = random_card / CARDS_PER_TYPE;
            
        Card *card = find_node_by_position(&cards, random_card);
		card->origin = &board->hand;
        assert(card);
        add_node(&board->hand, *card);
        delete_node_by_position(&cards, random_card);
    }
    // print_linked_list(&board->hand);
    
}

bool can_card_be_added_to_card_list(Card *card, LinkedList<Card> *list){
	Card *last_tableau_card = &list->last_node->data;
	int target_value = card->value + 1;
	if(list == card->origin) return false;
	if(card->type == CardType::HEARTS || card->type == CardType::DIAMONDS){
		if((last_tableau_card->type == CardType::CLUBS || last_tableau_card->type == CardType::SPADES )&& last_tableau_card->value == target_value){
			return true;
		}
	}else if(card->type == CardType::CLUBS || card->type == CardType::SPADES){
		if((last_tableau_card->type == CardType::HEARTS || last_tableau_card->type == CardType::DIAMONDS )&& last_tableau_card->value == target_value){
			return true;
		}
	}
	return false;
}

bool maybe_add_card_to_tableau(Board *board, V2 mouse_pos, LinkedList<Card> *&result_list){
	for(int i = 0; i < TABLEAU_SIZE; i++){
		bool break_flag = false;
		LinkedList<Card> *card_list = &board->tableau[i];
		
		LinkedListNode<Card> *previous_node = NULL;
		LinkedListNode<Card> *current_node = card_list->first;
		
		int card_counter = 0;
		
		while(current_node){
			Card *card = &current_node->data;
			
			if(DoRectContainsPoint(card->clickable_area, mouse_pos) && !card->flipped){
				// TODO: Only add the card if the tableau card is -1 the value of the hand card and is the opposite color.
				Card held_card = board->held_cards.first->data;
				if(can_card_be_added_to_card_list(&held_card, card_list)){
					result_list = card_list;
					return true;
					
				}
				return false;

				// print_linked_list(card_list);
				
			}
			
			previous_node = current_node;
			current_node = current_node->next;
		}
		
		

	}
	return false;
}

void update_cards_origin(LinkedList<Card> *cards, LinkedList<Card> *new_origin){
		LinkedListNode<Card> *previous_node = NULL;
		LinkedListNode<Card> *current_node = cards->first;
		
		
		while(current_node){
			current_node->data.origin = new_origin;
			
			previous_node = current_node;
			current_node = current_node->next;
		}
}

void draw_card(Card *card, V2 position, Board *board, Renderer *renderer){
	int card_index = card->type * CARDS_PER_TYPE + card->value;
        
	render_sprite(renderer, &board->card_sprites[card_index], position);
}

void draw_tableau(Board *board, Renderer *renderer){
    
    for(int i = 0; i < TABLEAU_SIZE; i++){
        // float base_y_padding = 37.0f;
        // float sum_y_padding = base_y_padding;
        LinkedList<Card> *card_stack = &board->tableau[i];
        LinkedListNode<Card> *previous_node;
        LinkedListNode<Card> *current_node = card_stack->first;
        while(current_node){
            Card *card = &current_node->data;
            int card_index = card->type * CARDS_PER_TYPE + card->value;
            
            if(card->flipped){
                render_sprite(renderer, &board->flipped_card, card->position);
            }else{
                render_sprite(renderer, &board->card_sprites[card_index], card->position);
                
            }
            
            // Uncomment this to see the clickable areas of the cards.
            // if(current_node == card_stack->last_node){
                // render_colored_rect(renderer, &card->clickable_area,{255,0,0}, 0.4);
                // printf("%f %f\n", card->clickable_area.x, card->clickable_area.y);
            // }else{
                // render_colored_rect(renderer, &card->clickable_area,{0,0,255}, 0.4);
            // }

            
            previous_node = current_node;
            current_node = current_node->next;
        }
        
    }
}

void draw_held_cards(Board *board, Renderer *renderer, V2 mouse_pos, V2 mouse_card_delta){
    LinkedListNode<Card> *previous_node;
    LinkedListNode<Card> *current_node = board->held_cards.first;
    // V2 delta = {mouse_pos.x - current_node->data.position.x, mouse_pos.y - current_node->data.position.y};
    V2 drawing_position = {mouse_pos.x - mouse_card_delta.x, mouse_pos.y - mouse_card_delta.y};
    // printf("%f, %f\n", mouse_pos.x, mouse_pos.y);
    while(current_node){
        Card *card = &current_node->data;
        // int card_index = card->type * CARDS_PER_TYPE + card->value;
        
        // render_sprite(renderer, &board->card_sprites[card_index], drawing_position);
		draw_card(card, drawing_position, board, renderer);
        drawing_position.y -= board->base_y_padding;
        
        previous_node = current_node;
        current_node = current_node->next;
    }
}

void draw_foundations(Board *board, Renderer *renderer){
    for(int i = 0; i < CardType::COUNT; i++){
		LinkedList<Card> *foundation = &board->foundations[i];
		
		if(foundation->size > 0){
			Card *card = &foundation->last_node->data;
			// int card_index = card->type * CARDS_PER_TYPE + card->value;
			V2 position = {board->foundations_x_positions[i], board->foundations_y_position};
			// render_sprite(renderer, &board->card_sprites[card_index], drawing_position);
			draw_card(card, position, board, renderer);
			
		}else{
			render_sprite(renderer, &board->foundation_sprite, {board->foundations_x_positions[i], board->foundations_y_position});
			
		}
        
    }
}

void draw_stock(Board *board, Renderer *renderer){
    Rect *stock_bbox = &board->stock.bounding_box;
    static Rect shadow_bbox = {stock_bbox->x + 5, stock_bbox->y - 5, stock_bbox->w, stock_bbox->h};
    render_colored_rect(renderer, &shadow_bbox,{0,0,0}); // Render a shadow below the stock.
    render_sprite(renderer, board->stock.sprite, {board->stock.bounding_box.x, board->stock.bounding_box.y});
}

template<typename T>
static void copy_card_array_to_list(T *cards, LinkedList<Card> *list){
	// if(!state->from_hand){
		for(int i = 0; i < cards->size(); i++){
			if((*cards)[i].type == CardType::COUNT) return;
			add_node(list, (*cards)[i]);
		}
		
	// }else{
		// for(int i = 0; i < cards->size(); i++){
			// Card *card = &(*cards)[i];
			// Card *previous_card = &board->previous_card_to_held;
			// if(card->type == CardType::COUNT) return;
			// if(card->type == previous_card->type && card->value == previous_card->value){
				// add_node(list, *card);
				// add_node(list, state->card_to_place_as_current);
			// }
			// add_node(list, (*cards)[i]);
		// }
	// }
}

template<typename T>
static void copy_hand_card_array_to_list(T *cards, LinkedList<Card> *list, Board *board, BoardState *state){
	if(!state->first_card){
		for(int i = 0; i < cards->size(); i++){
			Card *card = &(*cards)[i];
			Card *previous_card = &state->previous_card_to_placed;
			if(card->type == CardType::COUNT) return;
			if(card->type == previous_card->type && card->value == previous_card->value){
				add_node(list, *card);
				add_node(list, state->card_to_place_as_current);
			}else{
				add_node(list, *card);
				
			}
		}
		
	}else{
		for(int i = 0; i < cards->size(); i++){
			Card *card = &(*cards)[i];
			if(card->type == CardType::COUNT) return;
			if(i == 0){
				add_node(list, state->card_to_place_as_current);
				add_node(list, *card);
			}else{
				add_node(list, *card);
			}
		}
	}
}

void undo_action(Board *board){
	if(board->done_actions.size == 0) return;
	BoardState *state = &board->done_actions.first->data;
	for(int i = 0; i < TABLEAU_SIZE; i++){
		LinkedList<Card> *list = &board->tableau[i];
		clear_list(list);
		copy_card_array_to_list(&state->tableau[i],list);
	}
	
	for(int i = 0; i < CardType::COUNT; i++){
		LinkedList<Card> *list = &board->foundations[i];
		clear_list(list);
		copy_card_array_to_list(&state->foundations[i],list);
	}
	
	
	
	if(state->from_hand){
		clear_list(&board->hand);
		copy_hand_card_array_to_list(&state->hand,&board->hand, board, state);
		LinkedListNode<Card> *previous_node = NULL;
		LinkedListNode<Card> *current_node = board->hand.first;
		if(!state->first_card){
			while(current_node){
				Card *hand_card     = &current_node->data;
				Card *card_to_place = &state->card_to_place_as_current;
				if(hand_card->type == card_to_place->type && hand_card->value == card_to_place->value){
					board->current_stock_card = current_node;
					board->previous_hand_card = previous_node;
					break;
				}
				
				previous_node = current_node;
				current_node = current_node->next;
			}
			
		}else{
			board->current_stock_card = current_node;
			board->previous_hand_card = previous_node;
		}
		
	}
	
	pop_from_list(&board->done_actions);
}

template<typename T>
void copy_cards_list_to_array(LinkedList<Card> *list, T *cards, LinkedList<Card> *held_cards, Board *board, bool from_hand  = false){
	LinkedListNode<Card> *previous_node = NULL;
    LinkedListNode<Card> *current_node = list->first;
    // printf("%f, %f\n", mouse_pos.x, mouse_pos.y);
	int index = 0;
    while(current_node){
        // cards->push_back(current_node->data);
		(*cards)[index] = current_node->data;
        
        previous_node = current_node;
        current_node = current_node->next;
		index++;
    }
	
	// This is done because the cards that are being held have to be saved into the last state. Because when we grab the cards 
	// they get deleted from the origin list, so we need to add them to the last board state.
	if(from_hand) return;	
	if(held_cards->size == 0) return;
	if(held_cards->first->data.origin == list && list != &board->hand){
		previous_node = NULL;
		current_node = held_cards->first;
		while(current_node){
			(*cards)[index] = current_node->data;
			
			
			previous_node = current_node;
			current_node = current_node->next;
			index++;
		}
		
	}
	
}


static void copy_hand_cards_list_to_array(LinkedList<Card> *list, BoardState *state, Board *board){
	LinkedListNode<Card> *previous_node = NULL;
	LinkedListNode<Card> *current_node = board->hand.first;
	Card *previous_card_to_held = &board->previous_card_to_held;
	int index = 0;
	while(current_node){
		if(previous_node){
			if(previous_card_to_held){
				if(previous_card_to_held->type == previous_node->data.type && previous_card_to_held->value == previous_node->data.value){
					state->hand[index] = board->held_cards.first->data;
                    // board->previous_hand_card = previous_node;
				}else{
					state->hand[index] = current_node->data;
				}

			}
				
		}else{
			state->hand[index] = current_node->data;
		}
		
		previous_node = current_node;
		current_node = current_node->next;
		index++;
	}
	
}


void set_undo_info(Board *board, bool from_hand){
	BoardState state;
	state.from_hand = from_hand;
	if(state.from_hand){
		state.card_to_place_as_current = board->held_cards.first->data;
		if(board->previous_hand_card)
			state.previous_card_to_placed = board->previous_hand_card->data;
		else state.first_card = true;
	}
	for(int i = 0; i < TABLEAU_SIZE; i++){
		copy_cards_list_to_array(&board->tableau[i], &state.tableau[i], &board->held_cards, board);
	}
	
	for(int i = 0; i < CardType::COUNT; i++){
		copy_cards_list_to_array(&board->foundations[i], &state.foundations[i], &board->held_cards, board);
	}
	
	// if(state.from_hand){
		copy_cards_list_to_array(&board->hand, &state.hand, &board->held_cards, board, state.from_hand);
		
	// }
	
	add_node_to_beginning(&board->done_actions, state);
}
