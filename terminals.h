#ifndef TERMINALS_H
#define TERMINALS_H

enum DATA_TYPES {        
        CONST        = 0,
        OPER         = 1,
        VAR          = 2,
        UOPER        = 3,
	ID           = 4,
        UNKNOWN_TYPE = 5,
	TERM         = 6,
	IF           = 7,
	ELSE         = 8,
	WHILE        = 9,
	RELOP        = 10,
	TREE_TOKEN   = 11
	/*
	LT           = 10,
	LE           = 11,
	EQ           = 12,
	NE           = 13,
	GT           = 14,
	GE           = 15
	*/
};                     

enum operators {
	OP_ADD = '+',
	OP_MUL = '*',
	OP_DIV = '/',
	OP_SUB = '-',
	OP_PWR = '^',
	OP_ASG = '='
};

enum relops {
	RELOP_LT = 0,
	RELOP_LE = 1,
	RELOP_EQ = 2,
	RELOP_NE = 3,
	RELOP_GT = 4,
	RELOP_GE = 5
};

enum tree_tokens {
	STATEMENT = 0,
	CALL      = 1
}; 
/*
struct _sample {
	const int type;
	const char name[];
};

struct samples {
	const _sample = {IF, "if"};
}
*/
#endif // TERMINALS_H
