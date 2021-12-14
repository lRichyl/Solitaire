#pragma once
#include <array>

#include "linked_list.h"
#include "engine/sprite.h"
#include "engine/input.h"

static constexpr int CARDS_PER_TYPE = 13;
static constexpr int CARDS_TOTAL = 52;
static constexpr int TABLEAU_SIZE = 7;
static constexpr int MAX_STACK_SIZE = 20;
static constexpr int MAX_HAND_SIZE = 24;

enum CardType{
	HEARTS,
	DIAMONDS,
	CLUBS,
	SPADES,
	COUNT
};

struct Card{
	CardType type = CardType::COUNT;
	LinkedList<Card> *origin;
	bool flipped = false;
	int value;
	V2 position = {};
	Rect clickable_area;
};

struct Stock{
	Rect bounding_box;
	Sprite *sprite;
};


enum Action{
	ACTION_NONE,
	ACTION_MOVE,
	ACTION_FLIP
};


struct BoardState{
	std::array<Card, MAX_STACK_SIZE> tableau[TABLEAU_SIZE];
	std::array<Card, CARDS_PER_TYPE> foundations[CardType::COUNT];
	std::array<Card, MAX_HAND_SIZE>  hand;
	
	Card previous_card_to_placed;
	Card card_to_place_as_current;
	bool first_card = false;
	bool from_hand = false;
};


struct Board{
	LinkedList<Card> tableau[TABLEAU_SIZE];
	LinkedList<Card> foundations[CardType::COUNT];
	LinkedList<Card> hand;
	
	LinkedList<Card> held_cards;
	
	Sprite card_sprites[CARDS_TOTAL];
	Sprite flipped_card;
	Sprite foundation_sprite;
	
	Stock stock;
	
	Rect empty_stacks_bboxes[TABLEAU_SIZE];
	
	// This linked list will be used as a stack. I will later implement a stack using this linked list implementation as the base data structure.
	LinkedList<BoardState> done_actions;
	
	V2 cards_size = {126, 180};
	float tableau_empty_space;
	float tableau_y_starting_pos = 550;
	float base_y_padding = 37.0f;
	float tableau_x_positions[TABLEAU_SIZE];
	float padding;
	float starting_x_padding;
	
	float foundations_y_padding = 20;
	float foundations_x_positions[CardType::COUNT];
	float foundations_y_position;
	LinkedListNode<Card> *previous_foundations_nodes[CardType::COUNT];
	LinkedList<Card> *origin_foundation;
	
	int stock_card_index = 0;
	LinkedListNode<Card> *current_stock_card = NULL;
    LinkedListNode<Card> *previous_hand_card = NULL;
	Card previous_card_to_held;
    Rect hand_card_bounding_box;
    bool is_hand_card_held = false;
	bool is_tableau_card_held = false;
	bool is_foundation_card_held = false;
	bool stock_cycle_completed = true;
};





void print_data(Card *card);

void init_board(Board *board, Window *window);
void shuffle_cards_to_the_board(Board *board);
void calculate_tableau_cards_positions_and_clickable_areas(Board *board);
void update_cards_origin(LinkedList<Card> *cards, LinkedList<Card> *new_origin);
bool can_card_be_added_to_card_list(Card *card, LinkedList<Card> *list);
bool maybe_add_card_to_tableau(Board *board, V2 mouse_pos, LinkedList<Card> *&result_list);

void draw_card(Card *card, V2 position, Board *board, Renderer *renderer);
void draw_tableau(Board *board, Renderer *renderer);
void draw_foundations(Board *board, Renderer *renderer);
void draw_stock(Board *board, Renderer *renderer);
void draw_held_cards(Board *board, Renderer *renderer, V2 mouse_pos, V2 mouse_card_delta);

void undo_action(Board *board);
void copy_cards_list_to_vector(LinkedList<Card> *list, std::vector<Card> *cards);
void set_undo_info(Board *board, bool from_hand = false);
