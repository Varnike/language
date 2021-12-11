#include "lang.h"

static void print_token(TNODE *node);

int LangProcces(char *namein)
{// TODO ERRORS CHECK + for i remove + str -- btext
	textBuff btext = {};
	parsed_arr token_arr = {};
	
	init("check.txt", &btext);
	char *src_str = btext.buff;

	lexer_process(&btext, &token_arr);

	for (int i = 0; i != token_arr.size; i++) {
		print_token(token_arr.data[i]);
	}

	TNODE *root = _GetG(&token_arr);

	for (int i = 0; i != token_arr.size; i++) {
		print_token(token_arr.data[i]);
	}

	TreeDump(root);

	TreeDtor(root);
	
	free(token_arr.data);
	free(src_str);
}

int init(const char *file_in, textBuff *btext)
{
	CHECK_(!file_in, LANG_NULLPTR_ERR);

	btext->file_in = fopen(file_in, "r");
	CHECK_(btext->file_in == NULL, FOPEN_ERR);
	read_from_file(btext, file_in);
	
	printf("%s\n", btext->buff);
	
	fclose(btext->file_in);
}

int lexer_process(textBuff *btext, parsed_arr *token_arr)
{
	CHECK_(btext == nullptr, LANG_NULLPTR_ERR);
	CHECK_(btext->buff == NULL, LANG_NULLPTR_ERR);
	CHECK_(token_arr == NULL, LANG_NULLPTR_ERR);

	node_data node_val = {};
	
	token_arr->data = (TNODE **)calloc(MAX_TOKEN_CNT, sizeof(TNODE *));
	int comment_flag = 0;

	while (*(btext->buff) != '\0') {
$		if (isspace(*(btext->buff))) {
			if (*(btext->buff) == '\n')
				comment_flag = 0;
			(btext->buff)++;
			continue;
		} else if (comment_flag) {
			*(btext->buff)++;
			continue;
		} else if (*(btext->buff) == '#') {
			(btext->buff)++;
			comment_flag = 1; 
			continue;
		} else if (isdigit(*(btext->buff))) {
			node_val = tokenize_no(btext);
		} else if (isalpha(*(btext->buff))) {
			node_val = tokenize_id(btext);
		} else if (isOP(*(btext->buff))) {
			node_val = tokenize_op(btext);
		} else if (isTerminalChar(*(btext->buff))){
			DATA_ID(node_val)  = (btext->buff)++;
			node_val.len       = 1;
			node_val.data_type = TERM;
		} else if (isRelop(*(btext->buff))) {
			node_val = tokenize_relop(btext);			
		} else {
			SyntaxError();
		}

		ERRNUM_CHECK(ERRNUM);
		
		if (token_arr->size >= MAX_TOKEN_CNT) {
			ERRNUM = LANG_BUFFER_OVERFLOW;
			goto err_free_buffer;
		}

		TreeCtor((token_arr->data + token_arr->size++), node_val);
	}

err_free_buffer:
	if (ERRNUM)
		free(token_arr->data);
	
	return ERRNUM;
}

node_data tokenize_no(textBuff *btext)
{
	$
	node_data tmp_data = {};

	double val = strtod((btext->buff), &(btext->buff));	

	printf("number: %f\n str: %s\n", val, (btext->buff));

	tmp_data.value.num = val;
	tmp_data.data_type = CONST;

	return tmp_data;
}

node_data tokenize_id(textBuff *btext)
{	
$	node_data tmp_data = {};
	
	CHECK_SET_ERR(!(btext->buff), LANG_NULLPTR_ERR, tmp_data);

	DATA_ID(tmp_data) = (btext->buff);

	int it = 0;

	while (isdigit(*(btext->buff)) || isalpha(*(btext->buff))) {
		(btext->buff)++;
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
node_data tokenize_op(textBuff *btext)
{
$	node_data tmp_data = {};

	tmp_data.data_type = OPER;
	DATA_NUM(tmp_data) = *(btext->buff)++;
	
	return tmp_data;
}
//TODO macros
node_data tokenize_relop(textBuff *btext)
{	
	node_data tmp_data = {};
	tmp_data.data_type = RELOP;

	switch (*(btext->buff)++) {
	case '=':
		if (*(btext->buff) == '=') {
			(btext->buff)++;
			DATA_NUM(tmp_data) = RELOP_EQ;
			return tmp_data;
		} else {
			DATA_NUM(tmp_data) = OP_ASG;
			tmp_data.data_type = OPER;
			return tmp_data;
		}
		break;
	case '!':
		if (*(btext->buff) == '=') {
			(btext->buff)++;
			DATA_NUM(tmp_data) = RELOP_NE;
			return tmp_data;
		} else {
			SyntaxError();
		}
		break;
	case '<':
		if (*(btext->buff) == '=') {
			(btext->buff)++;
			DATA_NUM(tmp_data) = RELOP_LE;
			return tmp_data;
		} else {
			DATA_NUM(tmp_data) = RELOP_LT;
			return tmp_data;
		}
		break;
	case '>':
		if (*(btext->buff) == '=') {
			(btext->buff)++;
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
int isOP(char symb)
{
$	return (symb == OP_ADD || symb == OP_MUL || symb == OP_DIV || 
			symb == OP_SUB || symb == OP_PWR);
}

int isTerminalChar(char symb)
{
	return (symb == '(' || symb == ')' || symb == '{' || symb == '}' 
			|| symb == ';' || symb == '$');
}

int isRelop(char symb)
{
	return symb == '=' || symb == '>' || symb == '<' || symb == '!';
}

int isTerm(node_data ndata)
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
		printf(" data : [  %.*s  ], ", LEN(node), ID(node));
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

TNODE *_GetG(parsed_arr *token_arr)
{$
	TNODE *token = NULL;

	token = GetStmts();

	Require('$');
	return token;
}

TNODE *_GetStmts(parsed_arr *token_arr)
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

TNODE *_GetStmt(parsed_arr *token_arr)
{
	switch (TYPE(TOKEN)) {
	case ID:
		{	//TODO make pretty
			IT++;
			if (ID_MATCH('('))
				return GetF();
			IT--;

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
			Require(')');
					
			TNODE *stmt = GetStmt();
			
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

TNODE *_GetF(parsed_arr *token_arr)
{
	
	Require('(');
	//TNODE *arg = GetArg();
	Require(')');
	
	IT++;
	if (!ID_MATCH('{'))
		SyntaxError();
	IT--;

	TNODE *body = GetStmts();
	
}

TNODE *_GetRel(parsed_arr *token_arr)
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

TNODE *_GetE(parsed_arr *token_arr)
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

TNODE *_GetT(parsed_arr *token_arr)
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

TNODE *_GetP(parsed_arr *token_arr)
{$
	if (TYPE(TOKEN) == TERM && ID(TOKEN)[0] == '(') {
		IT++;

		TNODE *token = GetRel();

		Require(')');	

		return token;
	} else if (TYPE(TOKEN) == CONST){
		return GetN();
	} else if (TYPE(TOKEN) == ID) {
		return GetId();
	}
}

TNODE *_GetN(parsed_arr *token_arr)
{$
	if (TYPE(TOKEN) == CONST) {
		TNODE *tmp = TOKEN;
		IT++;
		return tmp;
	} else {
		SyntaxError();
	}
}

TNODE *_GetId(parsed_arr *token_arr)
{$
	if (TYPE(TOKEN) == ID) {
		TNODE *tmp = TOKEN;
		IT++;
		return tmp;
	} else {
		SyntaxError();
	}
}

int _Require(char cmp_symb, parsed_arr *token_arr, const char *func, const int line)
{
	if (LEN(TOKEN) == 1 && ID(TOKEN)[0] == cmp_symb) {
		TreeDeleteNode(&TOKEN);
		IT++;
		return 1;
	} else  {
		printf("Require [ %c ] but got [ %c ].\n Error: failed on line %d, function %s\n", 
				 cmp_symb, ID(TOKEN)[0], line, func);
		SyntaxError();
	}
}

int _SyntaxError(const char *func, const int line)
{
	fprintf(stderr, "!!!!!\n\nSyntax Error on line %d of func %s\n\n!!!!!\n", line, func);
	assert(!"Syntax ERROR!");
}
