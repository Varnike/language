#ifndef LANG_H
#define LANG_H

#include <ctype.h>
#include <stdio.h>
#include <assert.h>
#include <cstdlib>
#include "tree.h"
#include "config.h"
#include "include/onegin.h"

#define  $ 			printf("< %s >\t%d\n", __func__,__LINE__);
//#define  $ 			printf("\t\t\t---ON LINE %d IN FUNCTION %s---\n", __LINE__, __func__);

#define NUM(node_name) 				(node_name)->data.value.num
#define STR(node_name) 				(node_name)->data.value.str
#define TYPE(node_name)				(node_name)->data.data_type
#define ID(node_name)				(node_name)->data.value.id
#define LEN(node_name)				(node_name)->data.len

#define DATA_ID(node_name)			(node_name).value.id
#define DATA_NUM(node_name)			(node_name).value.num

#define TOKEN					token_arr.data[IT]
#define IT					token_arr.it
#define SIZE					token_arr.size

#define SYMB_MATCH(type, symb)			(TYPE(TOKEN) == type && LEN(TOKEN) == 1 && ID(TOKEN)[0] == symb)

#define SyntaxError()				_SyntaxError(__func__, __LINE__)

const int MAX_TOKEN_CNT = 100;

const int MAX_ID_LEN    = 100;

/*
 * 0)! TODO list for parse buff or resize!!!
 * 0)! TODO operators to char*, expand isOP()
 *
 * 1) TODO terminals file
 * 2) TODO onegin read
 * 3) TODO errs check
 * 4) TODO ? list from parsing
 */

/* real problems
 *	TODO	multiple lines stmts
 *	TODO	terminal type(LT LE EQ and s.o.)
 *	TODO	some print func to other file
 *	TODO	err checks in Get* functions	
 */
struct parsed_arr {
	TNODE **data = nullptr;
	int it   = 0;
	int size = 0;
};

class lang{
public:
	lang(char *namein);
	~lang();
	
	int init(const char *file_in);
	TNODE *GetG();
	TNODE *GetStmt();
	TNODE *GetE();
	TNODE *GetT();
	TNODE *GetP();
	TNODE *GetN();
	TNODE *GetId();
	TNODE *GetRel();

	int parse();
private:
	int Require(char cmp_symb);
	int _SyntaxError(const char *func, const int line);
	
	node_data parse_op();
	node_data parse_no();
	node_data parse_id();
	node_data process_relop();

	int isTerminalChar(char symb);
	int isOP(char symb);
	int isTerm(node_data ndata);

	textBuff btext = {};
	char *str   = nullptr;
	parsed_arr token_arr = {};
};
#endif // LANG_H
