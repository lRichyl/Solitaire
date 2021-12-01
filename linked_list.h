#pragma once
#include "memory_arena.h"
 // This is an implementation of a singly link linked list using an internal memory arena to allocate each link.
 // This can be up to 10x faster than C++'s forward_list when allocating. When freeing the speed is practically the same.
 // Author: Ricardo Trejo Castro
 // Date: 11/04/2021

template<typename T>
struct LinkedListNode{
	T data;
	LinkedListNode *next = NULL;
	int key = 0;
};

template<typename T>
struct LinkedList{
	LinkedListNode<T> *first = NULL;
	LinkedListNode<T> *last_node = NULL;
	int key_generator = 0;
	int size = 0;
	int max_size_in_bytes = 0;
	
	MemoryArena arena;
	
};



template<typename T>
void init_linked_list(LinkedList<T> *list, int max_size){
	int size_in_bytes = max_size * sizeof(LinkedListNode<T>);
	init_memory_arena(&list->arena, size_in_bytes);
	
	list->max_size_in_bytes = size_in_bytes;
}


template<typename T>
LinkedListNode<T>* add_node(LinkedList<T> *list, T data){
	LinkedListNode<T> *new_node = allocate_from_arena<LinkedListNode<T>>(&list->arena);
	new_node->data = data;
	new_node->next = NULL;
	new_node->key = list->key_generator;
	list->key_generator++;
	
	if(list->size == 0){
		list->first = new_node;
		list->last_node = list->first; 
	}else{
		list->last_node->next = new_node;
		list->last_node = new_node;
	}
	
	
	list->size++;
	return new_node;
}



template<typename T>
LinkedListNode<T>* add_node_to_beginning(LinkedList<T> *list, T data){
	LinkedListNode<T> *new_node = allocate_from_arena<LinkedListNode<T>>(&list->arena);
	new_node->data = data;
	new_node->key = list->key_generator;
	list->key_generator++;
	
	LinkedListNode<T> *temp = list->first;
	list->first = new_node;
	new_node->next = temp;
	
	list->size++;
	
	return new_node;
}


template<typename T>
LinkedListNode<T>* add_node_after(LinkedList<T> *list, T data, LinkedListNode<T> *node){
	LinkedListNode<T> *new_node = allocate_from_arena<LinkedListNode<T>>(&list->arena);
	// LinkedListNode<T> *new_node = find_free_node_in_pool(list);
	new_node->data = data;
	new_node->key = list->key_generator;
	list->key_generator++;
	LinkedListNode<T> *current_node = list->first;
	LinkedListNode<T> *next_node = NULL;
	
	next_node = node->next;
	node->next = new_node;
	new_node->next = next_node;
	
	list->size++;
	return new_node;
}

template<typename T>
void pop_from_list(LinkedList<T> *list){
	LinkedListNode<T> *delete_node = list->first;
	list->first = list->first->next;
	free_from_arena(&list->arena, delete_node);
	list->size--;
}

// For every type that we want to store in a linked list we need to define an overload of this function
// to be able to use the print_linked_list function.
inline void print_data(const int *data){
	printf("%d ", *data);
}

inline void print_data(const char **data){
	printf("\"%s\" ", *data);
}

template<typename T>
void print_linked_list(LinkedList<T> *list){
	LinkedListNode<T> *current_node = list->first;
	while(current_node){
		print_data(&current_node->data);
		// printf("%d", current_node->key);
		current_node = current_node->next;
	}
	printf("\n");
	printf("Size: %d\n", list->size);
	printf("\n");
}

// This is a very slow way to delete a node. 
template<typename T>
void delete_node_by_position(LinkedList<T> *list, int position){
	LinkedListNode<T> *current_node = list->first;
	LinkedListNode<T> *previous_node = NULL;
	int current_pos = 0;
	assert(position < list->size);
	while(current_node){
		if(current_pos == position){
			if(position == 0){
				LinkedListNode<T> *delete_node = list->first;
				list->first = list->first->next;
				free_from_arena(&list->arena, delete_node);
				list->size--;
				return;
			}else{
				previous_node->next = current_node->next;
				free_from_arena(&list->arena, current_node);
				list->size--;
				return;
			}
		}
		previous_node = current_node;
		current_node = current_node->next;
		current_pos++;
	}
}

template<typename T>
T* find_node_by_position(LinkedList<T> *list, int position){
	LinkedListNode<T> *current_node = list->first;
	LinkedListNode<T> *previous_node = NULL;
	int current_pos = 0;
	assert(position < list->size);
	while(current_node){
		if(current_pos == position){
			return &current_node->data;
			
		}
		previous_node = current_node;
		current_node = current_node->next;
		current_pos++;
	}
	
	return NULL;
}

// This gives us constant time deletion if we have a pointer to the previous node. Like for example if we are already iterating
// through the list we can update a pointer to the previous node after every iteration.
template<typename T>
void delete_node_after(LinkedList<T> *list, LinkedListNode<T> *&previous_node){
	// LinkedListNode<T> *current_node = node;
	
	if(previous_node->next == NULL && list->size == 1){
		list->size--;
		list->first = NULL;
		free_from_arena(&list->arena, list->first);
	}else{
		if(previous_node->next){
			LinkedListNode<T> *delete_node = previous_node->next;
			previous_node->next = previous_node->next->next;
			free_from_arena(&list->arena, delete_node);
			list->size--;
		}
		
	}
}

// This copies the elements of the source_list to the target_list.
template<typename T>
void append_list(LinkedList<T> *target_list, LinkedList<T> *source_list){
	LinkedListNode<T> *current_node = source_list->first;
	while(current_node){
		add_node(target_list, current_node->data);
		
		current_node = current_node->next;
	}
}

// Copies elements of the source list to the target list starting at a certain node while removing them from the source.
template<typename T>
void split_list(LinkedList<T> *target_list, LinkedList<T> *source_list, LinkedListNode<T> *source_previous_to_starting_node){
	// LinkedList<T> result;
	
	LinkedListNode<T> *current_node = source_previous_to_starting_node->next;
	LinkedListNode<T> *previous_node = source_previous_to_starting_node;
	while(current_node){
		add_node(target_list, current_node->data);
		delete_node_after(source_list, previous_node);
		
		// previous_node = current_node;
		current_node = current_node->next;
	}
}

template<typename T>
void clear_list(LinkedList<T> *list){
	LinkedListNode<T> *current_node = list->first;
	LinkedListNode<T> *next_node = NULL;
	while(current_node){
		list->size--;
		next_node = current_node->next;
		free_from_arena(&list->arena, current_node);
		
		current_node = next_node;
	}
	list->first = NULL;
	list->last_node = NULL;	
}
