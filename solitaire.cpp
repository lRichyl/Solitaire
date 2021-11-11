#include "solitaire.h"
#include <stdlib.h>

//Template requires this function defined.
void print_data(Card *card){
	printf("{%d , %d}\n", card->type, card->value);
}

void init_board(Board *board){
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
		for(int i = 0; i < TABLEAU_SIZE - j; i++){
			int random_card = rand() % cards.size;
			// int foundation_index = random_card / CARDS_PER_TYPE;
			
			// This find_node_by_position and delete_node_by_position are slow but we don't care because it is only run once at the start of the game.
			Card *card = find_node_by_position(&cards, random_card);
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