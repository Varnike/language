#include "name_table.h"

#define TYPE(node_name)                 (node_name)->data.data_type
#define ID(node_name)                   (node_name)->data.value.id 
#define LEN(node_name)                  (node_name)->data.len      

static uint32_t djb_hash(const char* data, size_t length)
{
	unsigned int hash = 5381; // MAGIC NUMBER
	
	for (int i = 0; i < length; ++i) {
		hash = (hash << 5) + hash + data[i];
	}

	return hash;
}

int TableCtor(name_table *table)
{
	CHECK_(!table, NTABLE_BAD_NODE);

	table->data = (table_node*)calloc(sizeof(table_node), MAX_TABLE_SIZE);
	CHECK_(table->data == NULL, CALLOC_ERR);

	return 0;
}

int TableInsert(name_table *table, TNODE *token, int size)
{
	CHECK_(!table, 				NTABLE_BAD_NODE);
	CHECK_(!table->data, 			NTABLE_BAD_NODE);
	CHECK_(table->size >= MAX_TABLE_SIZE, 	NTABLE_OVERFLOW);
	CHECK_(token == NULL,			TREE_NULL_NODE);

	printf("\n\nADDING NEW VAR: %.*s\n\n", LEN(token), ID(token));
// SLOW SLOW SLOW SLOW!
	if (TableFind(table, token) > 0)
		return ERRNUM = NTABLE_REDEFINE_ERR;

	int addr = table->curr_addr;

	uint32_t srch = djb_hash(ID(token), LEN(token));
	table_node new_name = {srch, TYPE(token), addr};
	
	table->curr_addr -= size;
	table->arg_cnt++;
	table->data[table->size++] = new_name;

	return addr;
}

int TableFind(name_table *table, TNODE *key)
{
	CHECK_(!table, 				NTABLE_BAD_NODE);
	CHECK_(!table->data, 			NTABLE_BAD_NODE);
	CHECK_(table->size >= MAX_TABLE_SIZE, 	NTABLE_OVERFLOW);
	CHECK_(!key,				TREE_NULL_NODE);

	uint32_t keyh = djb_hash(ID(key), LEN(key));

	for (int it = 0; it != table->size; it++) {
		if (TYPE(key) == table->data[it].type
			       	&& table->data[it].name == keyh) {
			printf("[found] ADDRESS = %d\n",
					table->data[it].addr );
			return table->data[it].addr;
		}
	}	
	//TODO COLLISIONS!!!
	return 0;	
}

int TableDtor(name_table *table)
{
	CHECK_(!table, 				NTABLE_BAD_NODE);
	CHECK_(!table->data, 			NTABLE_BAD_NODE);

	free(table->data);
	table->data = NULL;

	return 0;
}

int TableAddArg(name_table *table, TNODE *token, int size)
{
	CHECK_(!table, 				NTABLE_BAD_NODE);
	CHECK_(!table->data, 			NTABLE_BAD_NODE);
	CHECK_(table->size >= MAX_TABLE_SIZE, 	NTABLE_OVERFLOW);
	CHECK_(token == NULL,			TREE_NULL_NODE);

// SLOW SLOW SLOW SLOW!
	
	int addr = table->curr_arg_addr;

	uint32_t srch = djb_hash(ID(token), LEN(token));
	table_node new_name = {srch, TYPE(token), addr};
	
	table->curr_arg_addr += size;
	table->arg_cnt++;
	table->data[table->size++] = new_name;

	printf("\n\nADDING NEW ARG: %.*s, addr = %d\n\n", 
			LEN(token), ID(token), addr);

	return addr;
}

#undef LEN
#undef ID
#undef TYPE
