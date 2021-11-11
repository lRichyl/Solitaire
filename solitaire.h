#pragma once
#include <vector>

#include "linked_list.h"

static constexpr int CARDS_PER_TYPE = 13;
static constexpr int CARDS_TOTAL = 52;
static constexpr int TABLEAU_SIZE = 7;
static constexpr int MAX_STACK_SIZE = 10;
static constexpr int MAX_HAND_SIZE = 24;

enum CardType{
	HEARTS,
	DIAMONDS,
	CLUBS,
	SPADES,
	COUNT
};

struct Card{
	CardType type;
	int value;
};

struct Board{
	//Index 0 is for Hearts
	//Index 1 is for Diamonds
	//Index 2 is for Clubs
	//Index 3 is for Spades
	LinkedList<Card> tableau[TABLEAU_SIZE];
	LinkedList<Card> foundations[CardType::COUNT];
	LinkedList<Card> hand;
	
};

void init_board(Board *board);
void shuffle_cards_to_the_board(Board *board);