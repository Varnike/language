#ifndef BACKEND64_H
#define BACKEND64_H

#include <ctype.h>
#include <stdio.h>
#include <assert.h>
#include <cstdlib>
#include "../tree.h"
#include "../config.h"
#include "../name_table.h"
#include "elfhdr.h"
#include <elf.h>

//#define $
#define  $				printf("< %s >\t%d\n",\
	       					__func__,__LINE__);
#define $$				printf("-< %d >-\n",\
						cdata->ip);
#define NUM(node_name) 		(uint32_t)(node_name)->data.value.num
#define STR(node_name) 			(node_name)->data.value.str
#define TYPE(node_name)			(node_name)->data.data_type
#define ID(node_name)			(node_name)->data.value.id
#define LEN(node_name)			(node_name)->data.len


#define LEFT				node->left
#define RIGHT				node->right
#define PARENT				node->parent
#define VISIT(node)			if (node) trav_compile(node, table, file, cdata);
#define VISIT_NEW_TABLE(node, newt)	if (node) 		\
						trav_compile(node, newt, file, cdata);
#define PRINT(...)			fprintf(file, __VA_ARGS__)

const int COMP_BUFF_SIZE = 2048;

typedef struct COMPILE_DATA{
	unsigned char *buff = NULL;
	uint64_t ip = 0;		// current instruction pointer
	int size = COMP_BUFF_SIZE;
} comp_data;

int lang64_compile(TNODE *root, const char *name_out);
int trav_compile
	(TNODE *node, name_table *table, FILE *file, comp_data *cdata);


#endif // BACKEND64_H
