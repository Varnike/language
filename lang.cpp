#include "lang.h"

static void print_token(TNODE *node);
static void connect(TNODE *parent, TNODE *lchild, TNODE *rchild);
static int trav_translate(TNODE *node, name_table *table, FILE *file);
static uint32_t djb_hash(const char* data, size_t length);
static int std_func_check(TNODE *node);

// To determine whether it is necessary to push or pop args if function params
int ASM_ARG_POP = 0;

int LangProcces(char *namein)
{
	textBuff btext = {};
	parsed_arr token_arr = {};
	char *src_str = NULL;

	do {
		init("check.txt", &btext);
		CHECK_BREAK(ERRNUM);

		src_str = btext.buff;
		lexer_process(&btext, &token_arr);
		CHECK_BREAK(ERRNUM);

		for (int i = 0; i != token_arr.size; i++) {
			print_token(token_arr.data[i]);
		}

		TNODE *root = _GetG(&token_arr);
		CHECK_BREAK(ERRNUM);

		for (int i = 0; i != token_arr.size; i++) {
			print_token(token_arr.data[i]);
		}

		TreeDump(root);
		LangTranslate(root, "asm.txt");
		TreeDtor(root);	
	} while(0);
	
	if (token_arr.data)
		free(token_arr.data);
	if (src_str)
		free(src_str);

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

node_data tokenize_op(textBuff *btext)
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
	} else {						\
		els_code					\
		return tmp_data;				\
	}							\
	break;

node_data tokenize_relop(textBuff *btext)
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
		SyntaxError(); // TODO Lexer Error!
		break;
	}
}

#undef RELOP_CMP

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

#define TERM_CMP(name, type, size)				\
	if (strncmp(DATA_ID(ndata), name,			\
		(size > ndata.len) ? size : ndata.len) == 0) {	\
		return type;					\
	} else
		
int isTerm(node_data ndata) 
{ // TODO in arr
	TERM_CMP("suppose",    IF, 7) // TODO strlen
	TERM_CMP("however",    ELSE, 7)
	TERM_CMP("proven",     RETURN, 6)
	TERM_CMP("break",      BREAK, 5)
	TERM_CMP("Theorem",    THEOREM, 7)
	TERM_CMP("Given",      GIVEN, 5)
	TERM_CMP("Proof",      PROOF, 5)
	TERM_CMP("QED",        QED, 3)
	TERM_CMP("Consider",   WHILE, 8)
	TERM_CMP("assuming",   ASSUME, 8)
	TERM_CMP("perfomed",   PERF, 8)
	TERM_CMP("expression", EXPR, 10)
	TERM_CMP("therefore",  THEREF, 9)
	TERM_CMP("sin",        UOPER, 3)
	TERM_CMP("cos",        UOPER, 3) // in LIB
	return 0;
}

#undef TERM_CMP

static void print_token(TNODE *node) 
{
	if (!node)
		return;
	printf("node : [  %p  ], datatype : [  %d  ],\t", node, node->data.data_type);
	
	switch (node->data.data_type) {
	case CONST:
		printf("data : [  %lg  ], ", node->data.value.num);
		break;
	case OPER:
		printf("data : [  %c  ], ", STR(node));
		break;
	case RELOP:
		printf("data : [  %s  ]  ", getRelopName(STR(node)));
		break;
	default:
		printf("data : [  %.*s  ], ", LEN(node), ID(node));
		break;
	}

	printf("\tleft : [  %p  ],\tright : [  %p  ],\tparent : [  %p  ]\n", 
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
{$ //Require ()
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
		{
			return GetF();	
		}
	case ID:
		{	//TODO make pretty
			IT++;
			if (ID_MATCH('(')) {
				IT--;

				TNODE *token = GetCF();

				Require(';');
				return token;
			}

			IT--;

			TNODE *id = GetId();
			
			if (!SYMB_MATCH(OPER, OP_ASG))
				SyntaxError();

			TNODE *assign = TOKEN;
			IT++;
			TNODE *expr = GetE();

			connect(assign, id, expr);
			
			Require(';');

			return assign;
		}
	case IF: //functions
		{$
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
	case RETURN:
		{
			TNODE *ret = TOKEN;
			IT++;

			TNODE *token = GetRel();
			Require(';');
			
			connect(ret, NULL, token);

			return ret;
		}
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
	default:
		TNODE *node = GetE();
		Require(';');
		
		return node;
	}
}
//TODO rename
TNODE *_GetF(parsed_arr *token_arr)
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

	TNODE *arg   = GetRel();
	// TODO GetId

	CREATE_TYPE_TOKEN(param, PARAM);

	connect(param, GetArgs(), arg);
	
	return param;
}

TNODE *_GetCF(parsed_arr *token_arr)
{$
	TNODE *name = TOKEN;

	IT++;

	Require('(');
	TNODE *args = GetArgs();
	Require(')');

	CREATE_TYPE_TOKEN(call, CALL);

	connect(call, name, args);

	return call;
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
			return GetCF();
		}
		IT--;
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
//TODO 2 -> 1
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
//TODO MOVE
//			--------TRANSLATION-------				//

int LangTranslate(TNODE *root, const char *name_out)
{
	CHECK_(!name_out, LANG_NULL_FILENAME);
	TREE_CHECK(root, ERRNUM);

	FILE *file_out = fopen(name_out, "w");
	CHECK_(!file_out, FOPEN_ERR);

	name_table table = {};
	TableCtor(&table);
	ERRNUM_CHECK(ERRNUM);

	trav_translate(root, &table, file_out);
	ERRNUM_CHECK(ERRNUM);

	TableDtor(&table);
	fprintf(file_out, "hlt\n");
	fclose(file_out);
	system("cat asm.txt");

	return 0;
}

#define LEFT				node->left
#define RIGHT				node->right
#define PARENT				node->parent
#define VISIT(node)			if (node) trav_translate(node, table, file);
#define VISIT_NEW_TABLE(node, newt)	if (node) 				\
						trav_translate(node, newt, file);
int trav_translate(TNODE *node, name_table *table, FILE *file)
{$
	ERRNUM_CHECK(ERRNUM);		
	
	switch (TYPE(node)) {
	case STMT: // fokin Standard XD)
		VISIT(RIGHT);
		VISIT(LEFT);

		return 0;
	case OPER:
		if (STR(node) == OP_ASG) {
			int addr = TableFind(table, LEFT);
			
			VISIT(RIGHT);

			fprintf(file, "\tpop [bx+%d]\n", 
				(addr >= 0) ? addr : TableInsert(table, LEFT));

			return 0;
		}
		break;
	case IF:
		VISIT(LEFT);
		fprintf(file, "\tpush 0\n\tjne if_t%p:\n", node);

		VISIT(RIGHT->left);
		fprintf(file, "\tjmp if_e%p\nif_t%p:\n", node, node);

		VISIT(RIGHT->right);
		fprintf(file, "if_e%p:\n", node);

		return 0;
	case WHILE:
		fprintf(file, "while%p:\n", node);

		VISIT(LEFT);
		fprintf(file, "\tpush 0\n\tjne wskip%p\n\tjmp wend%p\nwskip%p:\n",
			       	node, node, node);

		VISIT(RIGHT);
		fprintf(file, "\tjmp while%p\nwend%p:\n", node, node);

		return 0;	
	case CALL:
		switch (std_func_check(LEFT)) {	
		case STD_SCAN:
			fprintf(file, "\t%s\n", getStdfName(STD_SCAN));	
			fprintf(file, "\tpop [bx+%d]\n", 
					TableFind(table, RIGHT->right));
			break;
		case STD_PRINT:
			VISIT(RIGHT);
			fprintf(file, "\t%s\n", getStdfName(STD_PRINT));
			break;
		default:	
			VISIT(RIGHT);
			fprintf(file,"\tpush bx\n\tpush %d\n\tadd\n\tpop bx\n"
					, table->size);
			fprintf(file, "\tcall %.*s\n", LEN(LEFT), ID(LEFT));
			fprintf(file,"\tpush bx\n\tpush %d\n\tsub\n\tpop bx\n"
					, table->size);

			fprintf(file, "\tpush ax\n");
			break;
		}
		return 0;
	case DEFINE: // TODO ERR CHECK
		{
		
			name_table func_table = {};
			TableCtor(&func_table);
			ERRNUM_CHECK(ERRNUM);

			fprintf(file, "\tjmp skipf%p\n", node);

			ASM_ARG_POP = 1;
			VISIT_NEW_TABLE(LEFT, &func_table);
			ASM_ARG_POP = 0;
	
			fprintf(file, "\tpush cx\n");

			VISIT_NEW_TABLE(RIGHT, &func_table);

			fprintf(file, "skipf%p:\n", node);
			TableDtor(&func_table);

		}
		return 0;
	case PARAM:
		if (ASM_ARG_POP == 1) {
			VISIT(RIGHT);
			VISIT(LEFT);

			return 0;
		}
		break;
	default:
		break;
	}

/////
	VISIT(LEFT);
////

/////
	VISIT(RIGHT);
/////
	print_token(node);

	switch (TYPE(node)) {
	case OPER:
		switch (STR(node)) {
		case OP_ADD:
			fprintf(file, "\tadd\n");
			break;
		case OP_MUL:
			fprintf(file, "\tmul\n");
			break;
		case OP_DIV:
			fprintf(file, "\tdiv\n");
			break;
		case OP_SUB:
			fprintf(file, "\tsub\n");
			break;	
		default:
$			return ERRNUM = LANG_UNKNOWN_TYPE; // TODO err type?
		}
		break;
	case CONST:
		fprintf(file, "\tpush %f\n", NUM(node));
		break;
	case ID:
		if (TYPE(PARENT) == FUNC) {
			fprintf(file, "%.*s:\n\tpop cx\n", 
					LEN(node), ID(node));
		} else if (TYPE(PARENT) == PARAM && ASM_ARG_POP) {
			int addr = TableInsert(table, node);
			ERRNUM_CHECK(ERRNUM);

			fprintf(file, "\tpop [bx+%d]\n", addr);
		} else  {
			int addr = TableFind(table, node);
			if (addr < 0)
				SyntaxError();

			fprintf(file, "\tpush [bx+%d]\n", addr);
		}
		break;
	case RELOP:	
		fprintf(file, "\t%s relt%p\n", getAsmRelop(STR(node)), node);
		fprintf(file, "\tpush 0\n\tjmp rels%p\nrelt%p:\n\tpush 1\nrels%p:\n",
				node, node, node);
		break;
	case RETURN:
		fprintf(file, "\tpop ax\n\tret\n");
		break;
	case PARAM:
	case FUNC:
		break;
	default:
$		return ERRNUM = LANG_UNKNOWN_TYPE;
	}

	return 0;
}
#undef LEFT
#undef RIGHT


#define STD_FUNC_CMP(name, type, size)				\
	if (strncmp(ID(node), name,				\
		(size > LEN(node)) ? size : LEN(node)) == 0) {	\
		return type;					\
	} else

static int std_func_check(TNODE *node)
{
	if (!node || TYPE(node) != ID)
		return -1;

	STD_FUNC_CMP("Introduce",  STD_SCAN, 9)
	STD_FUNC_CMP("Conclusion", STD_PRINT, 10)
	return -1;
}

#undef STD_FUNC_CMP

static uint32_t djb_hash(const char* data, size_t length)
{$
	unsigned int hash = 5381; // MAGIC NUMBER
	
	for (int i = 0; i < length; ++i) {
		hash = (hash << 5) + hash + data[i];
	}

	return hash;
}

int TableCtor(name_table *table)
{$
	CHECK_(!table, NTABLE_BAD_NODE);

	table->data = (table_node*)calloc(sizeof(table_node), MAX_TABLE_SIZE);
	CHECK_(table->data == NULL, CALLOC_ERR);

	return 0;
}

int TableInsert(name_table *table, TNODE *token, int addr)
{$
	CHECK_(!table, 				NTABLE_BAD_NODE);
	CHECK_(!table->data, 			NTABLE_BAD_NODE);
	CHECK_(table->size >= MAX_TABLE_SIZE, 	NTABLE_OVERFLOW);
	CHECK_(token == NULL,			TREE_NULL_NODE);

	if (addr < 0)
		addr = table->size;
// SLOW SLOW SLOW SLOW! // TODO REMOVE
	if (TableFind(table, token) >= 0)
		return ERRNUM = NTABLE_REDEFINE_ERR;

	printf("---inserting %.*s---\n", LEN(token), ID(token));
	uint32_t srch = djb_hash(ID(token), LEN(token));
	table_node new_name = {srch, TYPE(token), addr};

	table->data[table->size++] = new_name;

	return addr;
}

int TableFind(name_table *table, TNODE *key)
{$
	CHECK_(!table, 				NTABLE_BAD_NODE);
	CHECK_(!table->data, 			NTABLE_BAD_NODE);
	CHECK_(table->size >= MAX_TABLE_SIZE, 	NTABLE_OVERFLOW);
	CHECK_(!key,				TREE_NULL_NODE);

	uint32_t keyh = djb_hash(ID(key), LEN(key));

	for (int it = 0; it != table->size; it++)
		if (TYPE(key) == table->data[it].type && table->data[it].name == keyh)
			return table->data[it].addr; 

	return -1;	
}

int TableDtor(name_table *table)
{
	CHECK_(!table, 				NTABLE_BAD_NODE);
	CHECK_(!table->data, 			NTABLE_BAD_NODE);

	free(table->data);
	table->data = NULL;

	return 0;
}
