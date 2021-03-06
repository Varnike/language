#ifndef NAME_TABLE_H
#define NAME_TABLE_H

#include <ctype.h>
#include <stdio.h>
#include <assert.h>
#include <cstdlib>
#include "name_table.h"
#include "tree.h"
#include "config.h"

const int MAX_TABLE_SIZE = 10;

struct table_node {
	uint32_t name;
	int type;	
	int addr;
};

struct name_table {
	table_node *data;
	int size;
	int var_cnt = 0;
	int arg_cnt = 0;
	int curr_addr = -8;
	int curr_arg_addr = 16;
	int rsp_pos = 0;
};

int TableCtor(name_table *table);
int TableInsert(name_table *table, TNODE *node, int size = 8);
int TableAddArg(name_table *table, TNODE *token, int size = 8);
int TableFind(name_table *table, TNODE *key);
int TableDtor(name_table *table);

#endif // NAME_TABLE_H
