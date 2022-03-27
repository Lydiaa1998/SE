#ifndef _ALLOC_H_
#define _ALLOC_H_
#define _USE_MISC

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/mman.h>
#include <stdbool.h>

#define HEAP_START ((void*)0x04040000)

void* head ;
int taille;
struct node;
#pragma pack(push, 1)


struct node {
	struct node* next;
	size_t capacity;
	bool is_free;
};


#pragma pack(pop)

#define META_DATA_SIZE sizeof(struct node)

int initMemory(int nBytes);
void* myalloc(int nBytes);

void trysplit(struct node* blk, size_t query);

struct node* find_block(struct node* block, size_t query);

int myfree(void* p);

int FreeMemory();

void nodealloc_debug_struct_info( FILE * f,
		struct node const* const address);


void nodealloc_debug_heap( FILE * f, struct node const* ptr);

#endif
