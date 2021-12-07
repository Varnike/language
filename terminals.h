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
	RELOP        = 9
};                     

enum operators {
	OP_ADD = '+',
	OP_MUL = '*',
	OP_DIV = '/',
	OP_SUB = '-',
	OP_PWR = '^',
	OP_EQ  = '='
};


#endif // TERMINALS_H
