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
#define DATA_NUM(node_name)			(node_name).value.str

#define TOKEN					token_arr.data[IT]
#define IT					token_arr.it
#define SIZE					token_arr.size

#define SYMB_MATCH(type, symb)			(TYPE(TOKEN) == type && STR(TOKEN) == symb)
#define ID_MATCH(cmp_symb)			(LEN(TOKEN) == 1 && ID(TOKEN)[0] == cmp_symb)

#define SyntaxError()				_SyntaxError(__func__, __LINE__)
#define Require(ch)				_Require(ch, __func__, __LINE__);

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
 *	TODO ?  where =?
 *	TODO	
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
	TNODE *GetStmts();
	TNODE *GetStmt();
	TNODE *GetE();
	TNODE *GetT();
	TNODE *GetP();
	TNODE *GetN();
	TNODE *GetId();
	TNODE *GetRel();

	int lexer_process();
private:
	int _Require(char cmp_symb, const char *func, const int line);
	int _SyntaxError(const char *func, const int line);
	
	node_data tokenize_op();
	node_data tokenize_no();
	node_data tokenize_id();
	node_data tokenize_relop();

	int isTerminalChar(char symb);
	int isOP(char symb);
	int isTerm(node_data ndata);
	int isRelop(char symb);

	textBuff btext = {};
	char *str   = nullptr;
	parsed_arr token_arr = {};
};
#endif // LANG_H
