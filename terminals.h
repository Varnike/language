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
	TREE_TOKEN   = 11,
	STMT         = 12,
	CALL         = 13,
	DEFINE       = 14,
	PARAM        = 15,
	RETURN       = 16,
	FUNC         = 17,
	DECS         = 18,
	BREAK        = 19,
	THEOREM      = 20,
	GIVEN        = 21,
	PROOF        = 22,
	QED          = 23,
	CONS         = 24,
	ASSUME       = 25,
	EXPR         = 26,
	PERF         = 27,
	THEREF       = 28,
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

enum unary_operators {
        UOP_SIN = 'S',
        UOP_COS = 'C',
	UOP_LOG = 'L',
	UOP_IN  = 'I',
	UOP_OUT = 'O'
};                    

enum std_func {
	STD_SCAN  = 0,
	STD_PRINT = 1
};

#endif // TERMINALS_H
