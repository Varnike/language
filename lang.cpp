#include "lang.h"

static void print_token(TNODE *node);

lang::lang(char *namein)
{// TODO
	init("check.txt");

	str = btext.buff;
	lexer_process();

	for (int i = 0; i != token_arr.size; i++) {
		print_token(token_arr.data[i]);
	}

	TNODE *root = GetG();

	for (int i = 0; i != token_arr.size; i++) {
		print_token(token_arr.data[i]);
	}

	TreeDump(root);
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

int lang::lexer_process()
{
	CHECK_(str == nullptr, LANG_NULLPTR_ERR);
	
	node_data node_val = {};
	
	token_arr.data = (TNODE **)calloc(MAX_TOKEN_CNT, sizeof(TNODE *));
	int comment_flag = 0;

	while (*str != '\0') {
$		if (isspace(*str)) {
			if (*str == '\n')
				comment_flag = 0;
			str++;
			continue;
		} else if (comment_flag) {
			*str++;
			continue;
		} else if (*str == '#') {
			str++;
			comment_flag = 1; 
			continue;
		} else if (isdigit(*str)) {
			node_val = tokenize_no();
		} else if (isalpha(*str)) {
			node_val = tokenize_id();
		} else if (isOP(*str)) {
			node_val = tokenize_op();
		} else if (isTerminalChar(*str)){
			DATA_ID(node_val)  = str++;
			node_val.len       = 1;
			node_val.data_type = TERM;
		} else if (isRelop(*str)) {
			node_val = tokenize_relop();			
		} else {
			SyntaxError();
		}

		ERRNUM_CHECK(ERRNUM);
		
		if (token_arr.size >= MAX_TOKEN_CNT) {
			ERRNUM = LANG_BUFFER_OVERFLOW;
			goto err_free_buffer;
		}

		TreeCtor((token_arr.data + token_arr.size++), node_val);
	}

err_free_buffer:
	if (ERRNUM)
		free(token_arr.data);
	
	return ERRNUM;
}

node_data lang::tokenize_no()
{
	$
	node_data tmp_data = {};

	double val = strtod(str, &str);	

	printf("number: %f\n str: %s\n", val, str);	

	tmp_data.value.num = val;
	tmp_data.data_type = CONST;

	return tmp_data;
}

node_data lang::tokenize_id()
{	
$	node_data tmp_data = {};
	
	CHECK_SET_ERR(!str, LANG_NULLPTR_ERR, tmp_data);

	DATA_ID(tmp_data) = str;

	int it = 0;

	while (isdigit(*str) || isalpha(*str)) {
		str++;
		it++;	
	}
	
	tmp_data.len  = it;
	
	int type = isTerm(tmp_data);	
	if (type)
		tmp_data.data_type = type;
	else 
		tmp_data.data_type = ID;
	
	return tmp_data;
}
//TODO WTF WTF WTF
node_data lang::tokenize_op()
{
$	node_data tmp_data = {};

	tmp_data.data_type = OPER;
	DATA_NUM(tmp_data) = *str++;
	
	return tmp_data;
}
//TODO macros
node_data lang::tokenize_relop()
{	
	node_data tmp_data = {};
	tmp_data.data_type = RELOP;

	switch (*str++) {
	case '=':
		if (*str == '=') {
			str++;
			DATA_NUM(tmp_data) = RELOP_EQ;
			return tmp_data;
		} else {
			DATA_NUM(tmp_data) = OP_ASG;
			tmp_data.data_type = OPER;
			return tmp_data;
		}
		break;
	case '!':
		if (*str == '=') {
			str++;
			DATA_NUM(tmp_data) = RELOP_NE;
			return tmp_data;
		} else {
			SyntaxError();
		}
		break;
	case '<':
		if (*str == '=') {
			str++;
			DATA_NUM(tmp_data) = RELOP_LE;
			return tmp_data;
		} else {
			DATA_NUM(tmp_data) = RELOP_LT;
			return tmp_data;
		}
		break;
	case '>':
		if (*str == '=') {
			str++;
			DATA_NUM(tmp_data) = RELOP_GE;
			return tmp_data;
		} else {
			DATA_NUM(tmp_data) = RELOP_GT;
			return tmp_data;
		}
		break;
	default:
		SyntaxError(); // TODO Lexer Error!
		break;
	}
}
int lang::isOP(char symb)
{
$	return (symb == OP_ADD || symb == OP_MUL || symb == OP_DIV || 
			symb == OP_SUB || symb == OP_PWR);
}

int lang::isTerminalChar(char symb)
{
	return (symb == '(' || symb == ')' || symb == '{' || symb == '}' || symb == ';' || symb == '$');
}

int lang::isRelop(char symb)
{
	return symb == '=' || symb == '>' || symb == '<' || symb == '!';
}

int lang::isTerm(node_data ndata)
{
	if (strncmp(DATA_ID(ndata), "if", ndata.len) == 0)
		return IF;
	else if (strncmp(DATA_ID(ndata), "else", ndata.len) == 0)
		return ELSE;
	else if (strncmp(DATA_ID(ndata), "while", ndata.len) == 0)
		return WHILE;
	
	return 0;
}

static void print_token(TNODE *node) 
{
	if (!node)
		return;
	printf("node : [  %p  ], datatype : [  %d  ], ", node, node->data.data_type);
	
	switch (node->data.data_type) {
	case CONST:
		printf("data : [  %lg  ], ", node->data.value.num);
		break;
	case OPER:
		printf("data : [  %c  ], ", STR(node));
		break;
	case TERM:
	case IF:
	case ELSE:
	case WHILE:
	case ID:
		printf("data : [  %.*s  ], ", LEN(node), ID(node));
		break;
	case RELOP:
		printf("data : [  %s  ]  ", getRelopName(STR(node)));
		break;
	default:
		break;
	}

	printf("\tleft : [  %p  ],\tright : [  %p  ],\tparent : [  %p  ]\n", 
			node->left, node->right, node->parent);                             
}

TNODE *lang::GetG()
{$
	TNODE *token = NULL;
	TreeCtor(&token);
	/*
	while(1) {
		if (ID_MATCH('$'))
			return token;
		GetStmt();
		Require(';');
		IT++;
	}
	*/
	token = GetStmts();

	Require('$');
	return token;
}

TNODE *lang::GetStmts()
{
	TNODE *node = 0;
	if (ID_MATCH('$') || ID_MATCH('}')) {
		return node;
	} else {
		TreeCtor(&node);
		TYPE(node) = CONST;
		NUM(node)  = 1488;
		node->right = GetStmt();
		node->left  = GetStmts();
		return node;
	}
}

TNODE *lang::GetStmt()
{
	switch (TYPE(TOKEN)) {
	case ID:
		{
			TNODE *id = GetId();
			
			if (!SYMB_MATCH(OPER, OP_ASG))
				SyntaxError();

			TNODE *assign = TOKEN;
			IT++;
			TNODE *expr = GetE();

			assign->left  = id;
			assign->right = expr;
			id->parent    = assign;
			expr->parent  = assign;
			
			Require(';');

			return assign;
		}
		break;
	case IF:
		{
			$
			TNODE *cond = TOKEN;
			IT++;
			Require('(');
			
			TNODE *expr = GetRel();
$
			Require(')');
			
			
			TNODE *stmt = GetStmt();
$			
			cond->left  = expr;
			cond->right = stmt;
	
			return cond;
		}	
		break;
	case TERM:
		{
			Require('{');
	
			TNODE *stmt = GetStmts();
			Require('}');


			return stmt;
		}
		break;
	case WHILE:
		{
			TNODE *whileloop = TOKEN;
			IT++;
			Require('(');
		
			TNODE *expr = GetRel();
			Require(')');
			

			TNODE *stmt = GetStmt();

			whileloop->left  = expr;
			whileloop->right = stmt;	

			return whileloop;
		}
	default:
		TNODE *node = GetE();
		Require(';');
		
		return node;
		break;
	}
}

TNODE *lang::GetRel()
{
	TNODE *token = GetE();
	
	if (TYPE(TOKEN) == RELOP) {
		TNODE *relop = TOKEN;
		IT++;
		TNODE *token2 = GetE();

		CHECK_SET_ERR(!relop, LANG_NULLPTR_ERR, NULL);
		CHECK_SET_ERR(!token, LANG_NULLPTR_ERR, NULL);

		relop->right = token2;
		relop->left  = token;

		token->parent  = relop;
		token2->parent = relop;

		token = relop;
	}

	return token;
}

TNODE *lang::GetE()
{$
	TNODE *token = GetT();
	
	while(SYMB_MATCH(OPER, OP_ADD) || SYMB_MATCH(OPER, OP_SUB)) {
		TNODE *op = TOKEN;
		IT++;

		TNODE *token2 = GetT();
		
		op->left  = token;
		op->right = token2;
		
		printf("ffffff\n");	
		token     = op;
	}

	return token;
}

TNODE *lang::GetT()
{$
	TNODE *token = GetP();
	
	CHECK_SET_ERR(!token, LANG_NULLPTR_ERR, NULL);	

	while(SYMB_MATCH(OPER, OP_MUL) || SYMB_MATCH(OPER, OP_DIV)) {
		TNODE *op = TOKEN;
		IT++;

		TNODE *token2 = GetP();
		
		CHECK_SET_ERR(!token2, LANG_NULLPTR_ERR, NULL);	
		CHECK_SET_ERR(!op, LANG_NULLPTR_ERR, NULL);
		
		op->left  = token;
		op->right = token2;
		token->parent  = op;
		token2->parent = op;
		token     = op;
	}

	return token;
}

TNODE *lang::GetP()
{$
	if (TYPE(TOKEN) == TERM && ID(TOKEN)[0] == '(') {
		IT++;

		TNODE *token = lang::GetRel();

		Require(')');	

		return token;
	} else if (TYPE(TOKEN) == CONST){
		return GetN();
	} else if (TYPE(TOKEN) == ID) {
		return GetId();
	}
}

TNODE *lang::GetN()
{$
	if (TYPE(TOKEN) == CONST) {
		TNODE *tmp = TOKEN;
		IT++;
		return tmp;
	} else {
		SyntaxError();
	}
}

TNODE *lang::GetId()
{$
	if (TYPE(TOKEN) == ID) {
		TNODE *tmp = TOKEN;
		IT++;
		return tmp;
	} else {
		SyntaxError();
	}
}

int lang::_Require(char cmp_symb, const char *func, const int line)
{
	if (LEN(TOKEN) == 1 && ID(TOKEN)[0] == cmp_symb) {
		IT++;
		return 1;
	} else  {
		printf("Require [ %c ] but got [ %c ].\n Error: failed on line %d, function %s\n", 
				 cmp_symb, ID(TOKEN)[0], line, func);
		SyntaxError();
	}
}

int lang::_SyntaxError(const char *func, const int line)
{
	fprintf(stderr, "!!!!!\n\nSyntax Error on line %d of func %s\n\n!!!!!\n", line, func);
	assert(!"Syntax ERROR!");
}
