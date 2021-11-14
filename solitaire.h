#pragma once
#include <vector>

#include "linked_list.h"
#include "engine/sprite.h"

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
	bool flipped = false;
	int value;
	V2 position = {};
	Rect clickable_area;
};

struct Board{
	LinkedList<Card> tableau[TABLEAU_SIZE];
	LinkedList<Card> foundations[CardType::COUNT];
	LinkedList<Card> hand;
	
	Sprite card_sprites[CARDS_TOTAL];
	Sprite flipped_card;
	Sprite foundation_sprite;
	
	// std::vector<Rect> clickable_positions
	
	V2 cards_size = {126, 180};
	float tableau_empty_space;
	float tableau_y_starting_pos = 550;
	float tableau_x_positions[TABLEAU_SIZE];
	float padding;
	
	float foundations_y_padding = 20;
	float foundations_x_positions[CardType::COUNT];
};

void init_board(Board *board, Window *window);
void shuffle_cards_to_the_board(Board *board);
void draw_tableau(Board *board, Renderer *renderer);
void draw_foundations(Board *board, Renderer *renderer);