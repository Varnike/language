#ifndef LANG_H
#define LANG_H

#include <ctype.h>
#include <stdio.h>
#include <assert.h>
#include <cstdlib>
#include "tree.h"
#include "config.h"
#include "include/onegin.h"

#define  $ 				printf("< %s >\t%d\n", __func__,__LINE__);
//#define  $ 			printf("\t\t\t---ON LINE %d IN FUNCTION %s---\n", __LINE__, __func__);

#define NUM(node_name) 			(node_name)->data.value.num
#define STR(node_name) 			(node_name)->data.value.str
#define TYPE(node_name)			(node_name)->data.data_type
#define ID(node_name)			(node_name)->data.value.id
#define LEN(node_name)			(node_name)->data.len

#define DATA_ID(node_name)		(node_name).value.id
#define DATA_NUM(node_name)		(node_name).value.str

#define TOKEN				(token_arr->data[IT])
#define IT				(token_arr->it >= token_arr->size) ? \
						SyntaxError() : (token_arr->it)
#define SIZE				token_arr.size

#define SYMB_MATCH(type, symb)		(TYPE(TOKEN) == type && STR(TOKEN) == symb)

#define SYMB_MATCH(type, symb)		(TYPE(TOKEN) == type && STR(TOKEN) == symb)
#define ID_MATCH(cmp_symb)		(LEN(TOKEN) == 1 && ID(TOKEN)[0] == cmp_symb)

#define SyntaxError()			_SyntaxError(__func__, __LINE__)
#define Require(ch)			_Require(ch, token_arr, __func__, __LINE__);

#define GetG()				_GetG(token_arr)
#define GetStmts()			_GetStmts(token_arr)
#define GetStmt()			_GetStmt(token_arr)
#define GetF()				_GetF(token_arr)
#define GetArgs()			_GetArgs(token_arr)
#define GetCF()				_GetCF(token_arr)
#define GetE()				_GetE(token_arr)
#define GetT()				_GetT(token_arr)
#define GetP()				_GetP(token_arr)
#define GetN()				_GetN(token_arr)
#define GetId()				_GetId(token_arr)
#define GetRel()			_GetRel(token_arr)

#define CREATE_TYPE_TOKEN(name, type)				\
	TNODE *name = NULL;					\
	TreeCtor(&name);					\
	ERRNUM_CHECK(NULL);					\
	TYPE(name) = type;					\

const int MAX_TOKEN_CNT = 100;
const int MAX_ID_LEN    = 100;

/* real problems
 *	TODO	multiple lines stmts
 *	TODO	terminal type(LT LE EQ and s.o.)
 *	TODO	some print func to other file
 *	TODO	err checks in Get* functions	
 *	TODO ?  where =?
 *	TODO	UNARY OPERATORS!	
 */

struct parsed_arr {
	TNODE **data = nullptr;
	int it   = 0;
	int size = 0;
};

int LangProcces(char *namein);	
int init(const char *file_in, textBuff *btext);

TNODE *_GetG(parsed_arr *tokens);
TNODE *_GetStmts(parsed_arr *tokens);
TNODE *_GetStmt(parsed_arr *tokens);
TNODE *_GetF(parsed_arr *tokens);
TNODE *_GetCF(parsed_arr *tokens);
TNODE *_GetArgs(parsed_arr *tokens);
TNODE *_GetE(parsed_arr *tokens);
TNODE *_GetT(parsed_arr *tokens);
TNODE *_GetP(parsed_arr *tokens);
TNODE *_GetN(parsed_arr *tokens);
TNODE *_GetId(parsed_arr *tokens);
TNODE *_GetRel(parsed_arr *tokens);

int lexer_process(textBuff *btext, parsed_arr *token_arr);
int _Require(char cmp_symb, parsed_arr *tokens, 
		const char *func, const int line);
int _SyntaxError(const char *func, const int line);

node_data tokenize_op(textBuff *btext);
node_data tokenize_no(textBuff *btext);
node_data tokenize_id(textBuff *btext);
node_data tokenize_relop(textBuff *btext);

int isTerminalChar(char symb);
int isOP(char symb);
int isTerm(node_data ndata);
int isRelop(char symb);

#if 0
	textBuff btext = {};
	char *str   = nullptr;
	parsed_arr token_arr = {};
#endif

#endif // LANG_H
