#include "token_names.h"

const char *get_unoper_name(int type) 
{
	const char *uop_names[] = {
		"cos",
		"sin",
		"ln"
	};

	switch (type) {
	case UOP_COS:
		return uop_names[0];
	case UOP_SIN:
		return uop_names[1];
	case UOP_LOG:
		return uop_names[2];
	default:
		ERRNUM = DIFF_UNKNOWN_OPER;
		return NULL;
	}
}

const char *getRelopName(int type)
{
	if (type >= 6 || type < 0)
		return NULL;
	//TODO !!! 
	const char *relops_names[] = {
		"L",	// <
		"L=",	// <=
		"==",	// ==
		"!=",	// !=
		"G",	// >
		"G="	// >=
	};
	
	return relops_names[type];
}

const char *getTermName(int type)
{
	const char *term_names[] = {
		"statement",
		"call",
		"define",
		"parameter",
		"return",
		"function",
		"decision",
		"while",
		"if"
	};

	switch (type) {
	case STMT:
		return term_names[0];
	case CALL:
		return term_names[1];
	case DEFINE:
		return term_names[2];
	case PARAM:
		return term_names[3];
	case RETURN:
		return term_names[4];
	case FUNC:
		return term_names[5];
	case DECS:
		return term_names[6];
	case WHILE:
		return term_names[7];
	/*case CONS:
		return term_names[8];
	case SUPP:
		return term_names[9];
	*/
	default:
		ERRNUM = LANG_UNKNOWN_TYPE;
		return NULL;	
	}
}
