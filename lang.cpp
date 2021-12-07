#include "lang.h"

static void print_token(TNODE *node);

lang::lang(char *namein)
{// TODO
	init("check.txt");

	str = btext.buff;
	TNODE *token = NULL;

	while ((token = getNextToken()) != NULL)
		print_token(token);
}

lang::~lang()
{// TODO 
	free(btext.buff);
}

int lang::init(const char *file_in)
{
	CHECK_(!file_in, LANG_NULLPTR_ERR);

	btext.file_in = fopen(file_in, "r");
	CHECK_(btext.file_in == NULL, FOPEN_ERR);
	read_from_file(&btext, file_in);
	
	
	printf("%s\n", btext.buff);
	
	fclose(btext.file_in);
}

TNODE *lang::getNextToken()
{
	CHECK_SET_ERR(str == nullptr, LANG_NULLPTR_ERR, NULL);
	
	node_data node_val = {};
	
	while (*str != '\0') {
$		if (isspace(*str)) {
			str++;
			continue;
		} else if (isdigit(*str)) {
			node_val = parse_no();
		} else if (isalpha(*str)) {
			node_val = parse_id();
		} else if (isOP(*str)) {
			node_val = parse_op();
		} else if (isTerminalChar(*str)){
			DATA_ID(node_val)  = str++;
			node_val.len       = 1;
			node_val.data_type = TERM;
		}
		
		ERRNUM_CHECK(NULL);
		
		TNODE *node = NULL;
		TreeCtor(&node, node_val);

		return node;
	}

	return NULL; // TODO ???
}

node_data lang::parse_no()
{$
	node_data tmp_data = {};

	double val = strtod(str, &str);	

	printf("number: %f\n str: %s\n", val, str);	

	tmp_data.value.num = val;
	tmp_data.data_type = CONST;

	return tmp_data;
}

node_data lang::parse_id()
{	
$	node_data tmp_data = {};
	
	CHECK_SET_ERR(!str, LANG_NULLPTR_ERR, tmp_data);	

	DATA_ID(tmp_data) = str;
	int it = 0;

	while (isdigit(*str) || isalpha(*str)) {
		str++;
		it++;
	}

	if (isTerm(tmp_data))
		tmp_data.data_type = TERM;
	else 
		tmp_data.data_type = ID;

	tmp_data.len  = it;

	return tmp_data;
}

node_data lang::parse_op()
{
$	node_data tmp_data = {};
	tmp_data.data_type = OPER;
	DATA_ID(tmp_data)  = str;

	while (isOP(*str))
		str++;

	tmp_data.len = str - DATA_ID(tmp_data);

	return tmp_data;
}

int lang::isOP(char symb)
{
	return (symb == OP_ADD || symb == OP_MUL || symb == OP_DIV || 
			symb == OP_SUB || symb == OP_PWR || symb == OP_EQ);
}

int lang::isTerm(node_data ndata)
{
	if (strncmp(DATA_ID(ndata), "if", ndata.len) == 0)
		return 1;
	
	return 0;
}

int lang::isTerminalChar(char symb)
{
	return (symb == '(' || symb == ')' || symb == '{' || symb == '}' || symb == ';' || symb == '$');
}

static void print_token(TNODE *node) {
	if (!node)
		return;
	printf("node : [  %p  ], datatype : [  %d  ], ", node, node->data.data_type);
	
	switch (node->data.data_type) {
	case CONST:
		printf("data : [  %lg  ], ", node->data.value.num);
		break;
	case OPER:
		printf("data : [  %.*s  ], ", LEN(node), ID(node));
		break;
	case TERM:
	case ID:
		printf("data : [  %.*s  ], ", LEN(node), ID(node));
		break;
	default:
		break;
	}

	printf("\n");
#if 0
	fprintf(fout, "\tleft : [  %p  ],\tright : [  %p  ],\tparent : [  %p  ]\n", 
			node->left, node->right, node->parent);                             
#endif
}

TNODE *lang::GetG()
{$
	double val = GetE();
	Require(*str == '$');

	return val;
}

TNODE *lang::GetE()
{$
	double val = GetT();

	while(*str == '+' || *str == '-') {
		char op = *((str)++);
		double val2 = GetT();
		
		if (op == '+')
			val += val2;
		else 
			val -= val2;
	}

	return val;
}

TNODE *lang::GetT()
{$
	double val = GetP();
	
	while(*str == '*' || *str == '/') {
		char op = *((str)++);
		double val2 = GetP();
		
		if (op == '*')
			val *= val2;
		else 
			val /= val2;
	}

	return val;
}

TNODE *lang::GetP()
{
	if (*str == '(') {
		str++;

		TNODE *token = lang::GetE();
		Require(*str == ')');
		(str)++;

		return val;
	} else {
		return GetN();
	}
}

TNODE *lang::GetN()
{
	int cnt = 0;
	TNODE *token = getNextToken();

	if (token && TYPE(token) == CONST)
		return token;

	return NULL;
}

int lang::Require(char cmp_symb)
{
	if (*str != cmp_symb) {
		SyntaxError();
		return 0;
	}

	return 1;
}

int lang::SyntaxError()
{
	assert(!"Syntax ERROR!");
}
