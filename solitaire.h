#pragma once
#include <vector>

#include "linked_list.h"
#include "engine/sprite.h"
#include "engine/input.h"

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
	
	LinkedList<Card> held_cards;
	LinkedList<Card> *held_cards_origin;
	
	Sprite card_sprites[CARDS_TOTAL];
	Sprite flipped_card;
	Sprite foundation_sprite;
	
	// std::vector<Rect> clickable_positions
	
	V2 cards_size = {126, 180};
	float tableau_empty_space;
	float tableau_y_starting_pos = 550;
	float base_y_padding = 37.0f;
	float tableau_x_positions[TABLEAU_SIZE];
	float padding;
	
	float foundations_y_padding = 20;
	float foundations_x_positions[CardType::COUNT];
};

void print_data(Card *card);

void init_board(Board *board, Window *window);
void shuffle_cards_to_the_board(Board *board);

// Implement this two functions.
void split_cards(LinkedList<Card> *colliding_card_list, LinkedList<Card> *target_list);
LinkedList<Card> *mouse_press_on_card_list(Board *board, MouseInfo *mouse_info);
void get_list_and_card_mouse_is_over(Board *board, MouseInfo *mouse, LinkedList<Card> **card_list, Card **card, LinkedListNode<Card> **node_to_split_from);

void draw_tableau(Board *board, Renderer *renderer);
void draw_foundations(Board *board, Renderer *renderer);
void draw_held_cards(Board *board, Renderer *renderer, V2 mouse_pos, V2 mouse_card_delta);