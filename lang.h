#ifndef LANG_H
#define LANG_H

#include <ctype.h>
#include <stdio.h>
#include <assert.h>
#include <cstdlib>
#include "tree.h"
#include "config.h"
#include "include/onegin.h"
#include "name_table.h"

#define  $ 				printf("< %s >\t%d\n", __func__,__LINE__);
//#define  $ 			printf("\t\t\t---ON LINE %d IN FUNCTION %s---\n", __LINE__, __func__);

#define NUM(node_name) 			(node_name)->data.value.num
#define STR(node_name) 			(node_name)->data.value.str
#define TYPE(node_name)			(node_name)->data.data_type
#define ID(node_name)			(node_name)->data.value.id
#define LEN(node_name)			(node_name)->data.len

#define DATA_ID(node_name)		(node_name).value.id
#define DATA_NUM(node_name)		(node_name).value.str

#define TOKEN				(token_arr->data[(IT >= SIZE) ?\
						SyntaxError() : IT])
#define IT				token_arr->it
//(token_arr->it >= token_arr->size) ? 	SyntaxError() : (token_arr->it)
#define SIZE				token_arr->size

#define SYMB_MATCH(type, symb)		(TYPE(TOKEN) == type && STR(TOKEN) == symb)
#define ID_MATCH(cmp_symb)		(LEN(TOKEN) == 1 && ID(TOKEN)[0] == cmp_symb)
#define TYPE_MATCH(type)		(TYPE(TOKEN) == type)

#define SyntaxError()			_SyntaxError(__func__, __LINE__)
#define Require(ch)			_Require(ch, token_arr, __func__,__LINE__);
#define RequireT(type)			_RequireT(type, token_arr,__func__,__LINE__);

#define GetG()				_GetG(token_arr)
#define GetStmts()			_GetStmts(token_arr)
#define GetStmt()			_GetStmt(token_arr)
#define GetFunc()			_GetFunc(token_arr)
#define GetArgs()			_GetArgs(token_arr)
#define GetCallArgs()			_GetCallArgs(token_arr)
#define GetCallF()			_GetCallF(token_arr)
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
	TYPE(name) = type;					

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
TNODE *_GetFunc(parsed_arr *tokens);
TNODE *_GetCallF(parsed_arr *tokens);
TNODE *_GetArgs(parsed_arr *tokens);
TNODE *_GetCallArgs(parsed_arr *tokens);
TNODE *_GetE(parsed_arr *tokens);
TNODE *_GetT(parsed_arr *tokens);
TNODE *_GetU(parsed_arr *tokens);
TNODE *_GetP(parsed_arr *tokens);
TNODE *_GetN(parsed_arr *tokens);
TNODE *_GetId(parsed_arr *tokens);
TNODE *_GetRel(parsed_arr *tokens);

int lexer_process(textBuff *btext, parsed_arr *token_arr);
int _Require(char cmp_symb, parsed_arr *tokens, 
		const char *func, const int line);
int _RequireT(int type, parsed_arr *tokens, 
		const char *func, const int line);

int _SyntaxError(const char *func, const int line);
int LexerError(char symb, int line);

int LangTranslate(TNODE *root, const char *name_out);
#if 0
	textBuff btext = {};
	char *str   = nullptr;
	parsed_arr token_arr = {};
#endif

#endif // LANG_H
