#include "solitaire.h"
#include "engine/collision.h"
#include "game.h"
#include <stdlib.h>

//Template requires this function defined.
void print_data(Card *card){
	printf("{%d , %d}\n", card->type, card->value);
}

static void set_tableau_cards_positions_and_clickable_areas(Board *board){
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
	
	for(int i = 0; i < TABLEAU_SIZE; i++){
		board->tableau_x_positions[i] = (board->padding/2.f) + (i * board->padding) + (board->cards_size.x * i);
	}
	
	int j = 3;
	for(int i = 0; i < CardType::COUNT; i++){
		// board->tableau_x_positions[i] = (board->padding/2.f) + (i * board->padding) + (board->cards_size.x * i);
		board->foundations_x_positions[i] = board->tableau_x_positions[j + i];
	}
	
	// This should be called every time a group of cards is moved to another stack to regenerate their positions and clickable areas.
	set_tableau_cards_positions_and_clickable_areas(board);
	
	init_linked_list(&board->held_cards, 15);
	
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
			// if(first){
				// card->flipped = false;
			// }else{
				// card->flipped = true;
			// }
			first = false;
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
		assert(card);
		add_node(&board->hand, *card);
		delete_node_by_position(&cards, random_card);
	}
	// print_linked_list(&board->hand);
	
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
	printf("%f, %f\n", mouse_pos.x, mouse_pos.y);
	while(current_node){
		Card *card = &current_node->data;
		int card_index = card->type * CARDS_PER_TYPE + card->value;
		
		render_sprite(renderer, &board->card_sprites[card_index], drawing_position);
		drawing_position.y -= board->base_y_padding;
		
		previous_node = current_node;
		current_node = current_node->next;
	}
}

void draw_foundations(Board *board, Renderer *renderer){
	for(int i = 0; i < CardType::COUNT; i++){
		render_sprite(renderer, &board->foundation_sprite, {board->foundations_x_positions[i], renderer->window->internalHeight - board->foundations_y_padding});
		
	}
}

void get_list_and_card_mouse_is_over(Board *board, MouseInfo *mouse, LinkedList<Card> **list_result, Card **card_result, LinkedListNode<Card> **node_to_split_from){
	// Card *result = NULL;
	for(int i = 0; i < TABLEAU_SIZE; i++){
		LinkedList<Card> *card_list = &board->tableau[i];
		
		LinkedListNode<Card> *previous_node = NULL;
		LinkedListNode<Card> *current_node = card_list->first;
		while(current_node){
			Card *card = &current_node->data;
			
			V2 mouse_pos = {(float)mouse->x, (float)mouse->y};
			if(DoRectContainsPoint(card->clickable_area, mouse_pos)){
				// printf("%d\n",i);
				*card_result = card;
				*list_result = card_list;
				*node_to_split_from = previous_node;
				return;
			}
			
			previous_node = current_node;
			current_node = current_node->next;
		}
	}
	*card_result = NULL;
	*list_result = NULL;
	*node_to_split_from = NULL;
}
