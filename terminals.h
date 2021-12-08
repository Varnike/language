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
	RELOP        = 10
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
	OP_EQ  = '='
};

enum relops {
	LT = 0,
	LE = 1,
	EQ = 2,
	NE = 3,
	GT = 4,
	GE = 5
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
