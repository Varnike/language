/**
 * Backend for Math language
 *
 * Compilation from pased tree to Linux X86-64 executable.
 *
 * 2022 Enikeev Erik
 */

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
#include "labels_linker.h"

#ifdef 	BACKEND_LOG
#define $				printf("< %s >\t%d\n",\
	       					__func__,__LINE__);
#define $$				printf("-< %d >-\n",\
						cdata->ip);
#else

#define $
#define $$
#endif

#define NUM(node_name) 		(uint32_t)(node_name)->data.value.num
#define STR(node_name) 			(node_name)->data.value.str
#define TYPE(node_name)			(node_name)->data.data_type
#define ID(node_name)			(node_name)->data.value.id
#define LEN(node_name)			(node_name)->data.len


#define LEFT				node->left
#define RIGHT				node->right
#define PARENT				node->parent
#define VISIT(node)			if (node) trav_compile(node, table, cdata);
#define VISIT_NEW_TABLE(node, newt)	if (node) 		\
						trav_compile(node, newt, cdata);
#define PRINT(...)			fprintf(file, __VA_ARGS__)

/**
 * Use int emulation in compiled program. If defined,
 * last n digits of each number in compiled programm
 * will be used as n digits after decimal point, where
 * PRESISION must be set to 10^n.
 */
//#define INT_AS_FLOAT

/**
 * Automatically make programm execitable after compilation.
 */
#define AUTO_CHMOD

const int PRESISION = 100;
const int COMP_BUFF_SIZE = 5048;
const int UNSET_DST = 0xFFFFFFFF;
const int UNSET_RSP = 0x1488;


enum puhs_pop_flag {
	POP_ARG  = 0,
	PUSH_ARG = 1
};

typedef struct COMPILE_DATA {
	unsigned char *buff = NULL;
	uint64_t ip = 0;		// current instruction pointer
	label_link labels = {};
	int size = COMP_BUFF_SIZE;
} comp_data;

int lang64_compile(TNODE *root, const char *name_out);
int std_func_check(TNODE *node);
int trav_compile
	(TNODE *node, name_table *table, comp_data *cdata);


#endif // BACKEND64_H
