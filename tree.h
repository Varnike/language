#ifndef TREE_H
#define TREE_H

#include "include/error.h"
#include "config.h"
#include "token_names.h"

#include <cstring>
#include <stdio.h>
#include <cassert>
#include <cstdlib>
#include <typeinfo>
#include <stdint.h>
#include <inttypes.h>
#include "terminals.h"

//#define  $ 			printf("\t\t\t---ON LINE %d IN FUNCTION %s---\n", __LINE__, __func__);
//#define $ ;
#define SET_ERR(errtype)        return ERRNUM = errtype;

#define CHECK_(what, code)      if (what) SET_ERR (code)

#define CHECK_SET_ERR(what, code, retval) 					\
{										\
	if (what) {								\
		ERRNUM = code;							\
		return retval;							\
	}									\
}

#define CHECK_BREAK(what)	if (what) break;

#define ERRNUM_CHECK(ret_val)   						\
	{									\
		if(ERRNUM) {							\
			perror(errmsg(ERRNUM));					\
			TREE_EXIT_ERR(ret_val);					\
		}								\
	}

#define TREE_CHECK(node, ret_val)						\
	{									\
		if (TreeCheck(node)) {						\
			perror(errmsg(ERRNUM));					\
			TREE_EXIT_ERR(ret_val);					\
		}								\
	}			

#if DEBUG == 1                 
#define TREE_EXIT_ERR(ret) assert(!"ok");

#else
#define TREE_EXIT_ERR(ret) return ret;
#endif

/*
 * 2)TODO validation
 * 3)TODO int tree dump 
 *
enum DATA_TYPES {
	CONST        = 0,
	OPER         = 1,
	VAR          = 2,
	UOPER        = 3,
	UNKNOWN_TYPE = 4
};
*/
union DATA {
	double num;
	char str; // TODO char * ???
	char *id;
};

struct node_data {
	DATA value;
	int len = -1;
	int data_type = 5;
};

typedef node_data tval_t;

struct TNODE {
	tval_t data;
	TNODE *left;
	TNODE *right;
	TNODE *parent;
#ifdef VALIDATE_TREE_INTEGRY
	int visit_flag;
#endif
};

enum SUBTREE_SIDE {
	LEFT  = 0,
	RIGHT = 1
};

int TreeInsert(TNODE *parent, int side, tval_t data);
int TreeCtor(TNODE **root, tval_t val = {}, TNODE *parent = NULL);
int TreeDtor(TNODE *node);
int TreePrintNode(TNODE *node);
int TreeCopy(TNODE *src, TNODE **dst, TNODE *parent);
int TreeDeleteNode(TNODE **node);

void TreeDump(TNODE *node);

node_data set_node_data(int data_type = 0, DATA val = {});
DATA data_un_d(double num = 0);
DATA data_un_c(char str = 0);
void set_node(TNODE *node, tval_t val, TNODE *parent = NULL, TNODE *left = NULL, TNODE *right = NULL);


void TreeDotDump(TNODE *node);
void VisitPrint(TNODE *node, FILE *fout = stdout);
int TreeCheck(TNODE *node);

#endif // TREE_H
