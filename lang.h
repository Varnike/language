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

#define DATA_ID(node_name)			(node_name).value.id
#define DATA_NUM(node_name)			(node_name).value.num

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
struct parsed_arr {
	TNODE **data = nullptr;
	int size = 0;
};

class lang{
public:
	lang(char *namein);
	~lang();
	
	int init(const char *file_in);
	double GetG();
	double GetE();
	double GetT();
	double GetP();
	double GetN();
	
	int parse();
private:
	int Require(char cmp_symb);
	int SyntaxError();
	
	node_data parse_op();
	node_data parse_no();
	node_data parse_id();

	int isOP(char symb);
	int isTerm(node_data ndata);

	textBuff btext = {};
	char *str   = nullptr;
	parsed_arr token_arr = {};
};
#endif // LANG_H
