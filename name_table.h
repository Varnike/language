#ifndef NAME_TABLE_H
#define NAME_TABLE_H

#include <ctype.h>
#include <stdio.h>
#include <assert.h>
#include <cstdlib>
#include "name_table.h"
#include "tree.h"

const int MAX_TABLE_SIZE = 10;

struct table_node {
	uint32_t name;
	int type;	
	int addr;
};

struct name_table {
	table_node *data;
	int size;
	int curr_addr;
};

int TableCtor(name_table *table);
int TableInsert(name_table *table, TNODE *node, int size = 1);
int TableFind(name_table *table, TNODE *key);
int TableDtor(name_table *table);

#endif // NAME_TABLE_H
