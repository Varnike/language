#include "lang.h"

FILE *dump_file = stdout;

static void print_token(TNODE *node, FILE *file = dump_file);
static void connect(TNODE *parent, TNODE *lchild, TNODE *rchild);

static int isTerminalChar(char symb);
static int isOP(char symb);
static int isKeyword(node_data ndata);
static int isRelop(char symb);

static node_data tokenize_op(textBuff *btext);
static node_data tokenize_no(textBuff *btext);
static node_data tokenize_id(textBuff *btext);
static node_data tokenize_relop(textBuff *btext);

static TNODE *process_id(parsed_arr *token_arr);
static TNODE *process_if(parsed_arr *token_arr);
static TNODE *process_return(parsed_arr *token_arr);
static TNODE *process_while(parsed_arr *token_arr);

int LangProcces(char *namein)
{
	textBuff btext = {};
	parsed_arr token_arr = {};
	char *src_str = NULL;
	
	do {
//		dump_file = fopen("lang_dump.txt", "w");
//		CHECK_(dump_file == NULL, FOPEN_ERR);

		init("check.txt", &btext);
		CHECK_BREAK(ERRNUM);

		src_str = btext.buff;
		lexer_process(&btext, &token_arr);
		CHECK_BREAK(ERRNUM);

		for (int it = 0; it != token_arr.size; it++) {
			print_token(token_arr.data[it]);
		}

		TNODE *root = _GetG(&token_arr);
		CHECK_BREAK(ERRNUM);

		for (int it = 0; it != token_arr.size; it++) {
			print_token(token_arr.data[it]);
		}

		TreeDump(root);
		
		LangTranslate(root, "asm.txt");

		TreeDtor(root);	
	} while(0);
	
	
	if (token_arr.data)
		free(token_arr.data);
	if (src_str)
		free(src_str);
	if (dump_file != NULL || dump_file != stdout)
		fclose(dump_file);

	return ERRNUM;
}

int init(const char *file_in, textBuff *btext)
{
	CHECK_(!file_in, LANG_NULLPTR_ERR);

	btext->file_in = fopen(file_in, "r");
	CHECK_(btext->file_in == NULL, FOPEN_ERR);
	read_from_file(btext, file_in);
	
	printf("%s\n", btext->buff);
	
	fclose(btext->file_in);

	return 0;
}

int lexer_process(textBuff *btext, parsed_arr *token_arr)
{
	CHECK_(btext == nullptr, LANG_NULLPTR_ERR);
	CHECK_(btext->buff == NULL, LANG_NULLPTR_ERR);
	CHECK_(token_arr == NULL, LANG_NULLPTR_ERR);

	node_data node_val = {};
	
	token_arr->data = (TNODE **)calloc(MAX_TOKEN_CNT, sizeof(TNODE *));
	CHECK_(token_arr->data == NULL, CALLOC_ERR);

	int comment_flag = 0;
	btext->linecnt   = 0;
	//int line_cnt     = 1;

	while (*(btext->buff) != '\0') {
$		if (isspace(*(btext->buff))) {
			if (*(btext->buff) == '\n') {
				btext->linecnt++;
				comment_flag = 0;
			}
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
			LexerError(*(btext->buff), btext->linecnt); // LEXER ERR
		}

		ERRNUM_CHECK(ERRNUM);
		
		if (token_arr->size >= MAX_TOKEN_CNT) {
			ERRNUM = LANG_BUFFER_OVERFLOW;
			break;
		}

		TreeCtor((token_arr->data + token_arr->size++), node_val);
	}
	
	return ERRNUM;
}

static node_data tokenize_no(textBuff *btext)
{
	$
	node_data tmp_data = {};

	double val = strtod((btext->buff), &(btext->buff));	

	tmp_data.value.num = val;
	tmp_data.data_type = CONST;

	return tmp_data;
}

static node_data tokenize_id(textBuff *btext)
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
	
	int type = isKeyword(tmp_data);	
	if (type)
		tmp_data.data_type = type;
	else 
		tmp_data.data_type = ID;
	
	return tmp_data;
}

static node_data tokenize_op(textBuff *btext)
{
$	node_data tmp_data = {};

	tmp_data.data_type = OPER;
	DATA_NUM(tmp_data) = *(btext->buff)++;
	
	return tmp_data;
}

#define RELOP_CMP(expr, ifrel, els_code)			\
	case expr:						\
	if (*(btext->buff) == '=') {				\
		(btext->buff)++;				\
		DATA_NUM(tmp_data) = ifrel;			\
		return tmp_data;				\
	} else if (!isRelop(*(btext->buff))) {			\
		els_code					\
		return tmp_data;				\
	} else {						\
		LexerError(*(btext->buff), btext->linecnt);	\
	}							\
	break;

static node_data tokenize_relop(textBuff *btext)
{	
	node_data tmp_data = {};
	tmp_data.data_type = RELOP;

	switch (*(btext->buff)++) {
	RELOP_CMP('=', RELOP_EQ, 
		{
			DATA_NUM(tmp_data) = OP_ASG;
			tmp_data.data_type = OPER;
		});
	RELOP_CMP('!',RELOP_NE,
		{
			LexerError(*(btext->buff), btext->linecnt);
		});
	RELOP_CMP('<',RELOP_LE,
		{
			DATA_NUM(tmp_data) = RELOP_LT;
		});
	RELOP_CMP('>', RELOP_GE,
		{
			DATA_NUM(tmp_data) = RELOP_GT;
		});
	default:
		LexerError(*(btext->buff), btext->linecnt);
		break;
	}
}

#undef RELOP_CMP

static int isOP(char symb)
{
$	return (symb == OP_ADD || symb == OP_MUL || symb == OP_DIV || 
			symb == OP_SUB || symb == OP_PWR);
}

static int isTerminalChar(char symb)
{
	return (symb == '(' || symb == ')' || symb == '{' || symb == '}' 
			|| symb == ';' || symb == '$' || symb == '[' || symb == ']');
}

static int isRelop(char symb)
{
	return symb == '=' || symb == '>' || symb == '<' || symb == '!';
}

#define TERM_CMP(name, type)					\
	if (strncmp(DATA_ID(ndata), name,			\
			((size = strlen(name)) > ndata.len) ? 	\
			size : ndata.len) == 0) {		\
		return type;					\
	} else

// Keyword		
static int isKeyword(node_data ndata) 
{
	int size = 0;

	TERM_CMP("suppose",    IF)	
	TERM_CMP("however",    ELSE)
	TERM_CMP("proven",     RETURN)
	TERM_CMP("break",      BREAK)
	TERM_CMP("Theorem",    THEOREM)
	TERM_CMP("Given",      GIVEN)
	TERM_CMP("Proof",      PROOF)
	TERM_CMP("QED",        QED)
	TERM_CMP("Consider",   WHILE)
	TERM_CMP("assuming",   ASSUME)
	TERM_CMP("perfomed",   PERF)
	TERM_CMP("expression", EXPR)
	TERM_CMP("therefore",  THEREF)
	TERM_CMP("Select",     SELECT)
	TERM_CMP("set",        SET)
	TERM_CMP("of",         OF)
	TERM_CMP("elements",   ELEM)

	return 0;
}

#undef TERM_CMP

static void print_token(TNODE *node, FILE *file) 
{
	if (!node)
		return;
	fprintf(file, "node : [  %p  ], datatype : [  %d  ],\t", 
			node, node->data.data_type);
	
	switch (node->data.data_type) {
	case CONST:
		fprintf(file, "data : [  %lg  ], ", node->data.value.num);
		break;
	case OPER:
		fprintf(file, "data : [  %c  ], ", STR(node));
		break;
	case RELOP:
		fprintf(file, "data : [  %s  ]  ", getRelopName(STR(node)));
		break;
	default:
		fprintf(file, "data : [  %.*s  ], ", LEN(node), ID(node));
		break;
	}

	fprintf(file, "\tleft : [  %p  ],\tright : [  %p  ],\tparent : [  %p  ]\n", 
			node->left, node->right, node->parent);                             
}

TNODE *_GetG(parsed_arr *token_arr)
{$
	ERRNUM_CHECK(NULL);
	CHECK_SET_ERR(!token_arr, TREE_NULL_NODE, NULL);

	TNODE *token = NULL;

	token = GetStmts();

	Require('$');
	return token;
}

TNODE *_GetStmts(parsed_arr *token_arr)
{$ 
	if (ID_MATCH('$') || ID_MATCH('}') || TYPE_MATCH(QED)) {
		return NULL;
	} else {
		CREATE_TYPE_TOKEN(node, STMT);
		
		TNODE *stmt  = GetStmt();
		TNODE *stmts = GetStmts();

		connect(node, stmts, stmt);

		return node;
	}
}

TNODE *_GetStmt(parsed_arr *token_arr)
{$
	switch (TYPE(TOKEN)) {
	case THEOREM:
		return GetFunc();	
	case ID:
		return process_id(token_arr);
	case IF: 
		return process_if(token_arr);
	case RETURN:
		return process_return(token_arr);
	case PROOF:
		{
			RequireT(PROOF);
			TNODE *stmt = GetStmts();
			RequireT(QED);

			return stmt;
		}
	case TERM:
		{
			Require('{');
			TNODE *stmt = GetStmts();
			Require('}');

			return stmt;
		}
	case WHILE:
		return process_while(token_arr);
	case SELECT:
		{ // TODO in func
			RequireT(SELECT);
			RequireT(SET);

			TNODE *arr = GetId();

			RequireT(OF);

			TNODE *size = GetN();

			RequireT(ELEM);
			Require(';');


			CREATE_TYPE_TOKEN(init, ARR_INIT);

			connect(init, arr, NULL);
			connect(arr, size, NULL);

			return init;
		}
	default:
		TNODE *node = GetE();
		Require(';');
		
		return node;
	}
}

static TNODE *process_id(parsed_arr *token_arr)
{
	IT++;

	if (ID_MATCH('(')) {
		IT--;

		TNODE *token = GetCallF();

		Require(';');
		return token;
	}

	IT--;

	TNODE *id = GetArr();//GetId();
	
	if (!SYMB_MATCH(OPER, OP_ASG))
		SyntaxError();

	TNODE *assign = TOKEN;
	IT++;
	TNODE *expr = GetE();

	connect(assign, id, expr);
	
	Require(';');

	return assign;
}

static TNODE *process_if(parsed_arr *token_arr)
{
	TNODE *cond = TOKEN;
	IT++;

	Require('(');	
	TNODE *expr = GetRel();
	Require(')');

	RequireT(PERF);
	RequireT(THEREF);

	TNODE *stmt = GetStmt();
		
	CREATE_TYPE_TOKEN(decs, DECS);

	connect(cond, expr, decs);

	if (TYPE(TOKEN) == ELSE) {
		RequireT(ELSE);

		TNODE *els = GetStmt();
		connect(decs, els, stmt);
	} else {
		connect(decs, NULL, stmt);
	}

	return cond;
}

static TNODE *process_return(parsed_arr *token_arr)
{
	TNODE *ret = TOKEN;
	IT++;

	TNODE *token = GetRel();
	Require(';');
	
	connect(ret, NULL, token);

	return ret;
}

static TNODE *process_while(parsed_arr *token_arr)
{
	TNODE *whileloop = TOKEN;
	IT++;

	TNODE *stmt = GetStmt();

	RequireT(ASSUME);
	RequireT(EXPR);

	Require('(');	
	TNODE *expr = GetRel();
	Require(')');

	RequireT(PERF);
	Require(';');

	connect(whileloop, expr, stmt);

	return whileloop;
}

TNODE *_GetFunc(parsed_arr *token_arr)
{$
	ERRNUM_CHECK(NULL);
	
	RequireT(THEOREM);

	Require('(');
	TNODE *name = TOKEN;
	IT++;
	Require(')');

	RequireT(GIVEN);

	Require('(');
	TNODE *arg = GetArgs();
	Require(')');
		
	if (!TYPE_MATCH(PROOF))
		SyntaxError();
	
	TNODE *body   = GetStmt();
	
	CREATE_TYPE_TOKEN(define, DEFINE);
	CREATE_TYPE_TOKEN(func,   FUNC);

	connect(define, func, body);
	connect(func, name, arg);
	
	return define;
}

TNODE *_GetArgs(parsed_arr *token_arr)
{$
	if (ID_MATCH(')'))
		return NULL;

	TNODE *arg   = GetId();

	CREATE_TYPE_TOKEN(param, PARAM);

	connect(param, GetArgs(), arg);
	
	return param;
}

TNODE *_GetCallF(parsed_arr *token_arr)
{$
	TNODE *name = TOKEN;

	IT++;

	Require('(');
	TNODE *args = GetCallArgs();
	Require(')');

	CREATE_TYPE_TOKEN(call, CALL);

	connect(call, name, args);

	return call;
}

TNODE *_GetCallArgs(parsed_arr *token_arr)
{$
	if (ID_MATCH(')'))
		return NULL;

	TNODE *arg   = GetRel();

	CREATE_TYPE_TOKEN(param, PARAM);

	connect(param, GetCallArgs(), arg);
	
	return param;
}

TNODE *_GetRel(parsed_arr *token_arr)
{$
	TNODE *token = GetE();
	
	if (TYPE(TOKEN) == RELOP) {
		TNODE *relop = TOKEN;
		IT++;
		TNODE *token2 = GetE();

		CHECK_SET_ERR(!relop, LANG_NULLPTR_ERR, NULL);
		CHECK_SET_ERR(!token, LANG_NULLPTR_ERR, NULL);

		connect(relop, token, token2);

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
		connect(op, token, token2);

		token = op;
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
		
		connect(op, token, token2);

		token = op;
	}

	return token;
}

TNODE *_GetP(parsed_arr *token_arr)
{$
	if (TYPE(TOKEN) == TERM && ID(TOKEN)[0] == '(') {
		Require('(');

		TNODE *token = GetRel();
		Require(')');	

		return token;
	} else if (TYPE(TOKEN) == CONST){
		return GetN();
	} else if (TYPE(TOKEN) == ID) {
		IT++;
		if (ID_MATCH('(')) {
			IT--;
			return GetCallF();
		}
		IT--;
		return GetArr();
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

TNODE *_GetArr(parsed_arr *token_arr)
{
	TNODE *id = GetId();

	if (ID_MATCH('[')) {
		Require('[');
		TNODE *addr = GetRel();
		Require(']');

		connect(id, addr, NULL);
	}

	return id;
}

TNODE *_GetId(parsed_arr *token_arr)
{$
	print_token(TOKEN);
	if (TYPE(TOKEN) == ID) {
		TNODE *tmp = TOKEN;
		IT++;
		return tmp;
	} else {
		SyntaxError();
	}
}

static void connect(TNODE *parent, TNODE *lchild, TNODE *rchild)
{
	CHECK_SET_ERR(!parent, TREE_NULL_NODE, );

	parent->left   = lchild;
	parent->right  = rchild;
	if (lchild)
		lchild->parent = parent;
	if (rchild)
		rchild->parent = parent;	
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

int _RequireT(int type, parsed_arr *token_arr, const char *func, const int line)
{
	if (TYPE(TOKEN) == type) {
		TreeDeleteNode(&TOKEN);
		IT++;
		return 1;
	} else  {
		printf("Require [ %d ] but got [ %.*s ].\n Error: failed on line %d, function %s\n", 
				type, LEN(TOKEN), ID(TOKEN), line, func);
		SyntaxError();
	}
}

int LexerError(char symb, int line)
{
	fprintf(stderr, "Lexer ERROR: unknown symbol \'%c\' on line %d\n!!!",
			symb, line);

	ERRNUM = LANG_LEXER_ERR;

	assert(!"Lexer ERROR");
}

int _SyntaxError(const char *func, const int line)
{
	fprintf(stderr, "!!!!!\n\nSyntax Error on line %d of func %s\n\n!!!!!\n", 
			line, func);
	assert(!"Syntax ERROR!");
}
