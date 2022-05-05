#include "backend.h"
#include "X86-64/backend64.h"

static int asm_if(TNODE *node, name_table *table, FILE *file);
static int asm_define(TNODE *node, name_table *table, FILE *file);
static int asm_call(TNODE *node, name_table *table, FILE *file);
static int asm_while(TNODE *node, name_table *table, FILE *file);
static int asm_id(TNODE *node, name_table *table, FILE *file);
static int trav_translate(TNODE *node, name_table *table, FILE *file);
static int std_func_check(TNODE *node);                               

// To determine whether it is necessary to push or pop args if function params
int ASM_ARG_POP = 0;

int LangTranslate(TNODE *root, const char *name_out)
{
	CHECK_(!name_out, LANG_NULL_FILENAME);
	TREE_CHECK(root, ERRNUM);

	lang64_compile(root, name_out);
	return 0;

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

	printf("=====			ASM CODE			=====\n\n");
	system("cat asm.txt");

	return 0;
}

#define $
//#define  $                              printf("< %s >\t%d\n", __func__,__LINE__);
#define NUM(node_name) 			(node_name)->data.value.num
#define STR(node_name) 			(node_name)->data.value.str
#define TYPE(node_name)			(node_name)->data.data_type
#define ID(node_name)			(node_name)->data.value.id
#define LEN(node_name)			(node_name)->data.len


#define LEFT				node->left
#define RIGHT				node->right
#define PARENT				node->parent
#define VISIT(node)			if (node) trav_translate(node, table, file);
#define VISIT_NEW_TABLE(node, newt)	if (node) 				\
						trav_translate(node, newt, file);
#define PRINT(...)			fprintf(file, __VA_ARGS__)

int trav_translate(TNODE *node, name_table *table, FILE *file)
{
	ERRNUM_CHECK(ERRNUM);		
	
	switch (TYPE(node)) {
	case STMT: 
		VISIT(RIGHT);
		VISIT(LEFT);

		return 0;
	case OPER:
		if (STR(node) == OP_ASG) {
			int addr = TableFind(table, LEFT);
			
			VISIT(RIGHT);
			
			if (LEFT->left) {
				ASM_ARG_POP = 1;
				VISIT(LEFT);
				ASM_ARG_POP = 0;
			} else { 
				PRINT("\tpop [bx+%d]\n", 
				(addr >= 0) ? addr : TableInsert(table,
				       	LEFT));
			}

			return 0;
		}
		break;
	case IF:
		return asm_if(node, table, file);
	case WHILE:
		return asm_while(node, table, file);
	case CALL:
		return asm_call(node, table, file);
	case DEFINE:
		return asm_define(node, table, file);
	case PARAM:
		if (ASM_ARG_POP == 1) {
			VISIT(RIGHT);
			VISIT(LEFT);

			return 0;
		}
		break;
	case ARR_INIT:
		{
			int addr = TableFind(table, LEFT);
			if (addr >= 0)
				return ERRNUM = TRANSL_DOUBLE_INIT;

			TableInsert(table, LEFT, (int)NUM(LEFT->left));
			return 0;
		}
	default:
		break;
	}

/////
	VISIT(LEFT);
	VISIT(RIGHT);
/////
	switch (TYPE(node)) {
	case OPER:
		PRINT("\t%s\n", getAsmOper(STR(node)));
		break;
	case CONST:
		fprintf(file, "\tpush %f\n", NUM(node));
		break;
	case ID:
		asm_id(node, table, file);
		break;
	case RELOP:	
		PRINT("\t%s relt%p\n", getAsmRelop(STR(node)), node);
		PRINT("\tpush 0\n");
		PRINT("\tjmp rels%p\n", node);
		PRINT("relt%p:\n", node);
		PRINT("\tpush 1\n");
		PRINT("rels%p:\n", node);

		break;
	case RETURN:
		PRINT("\tpop ax\n\tret\n");

		break;
	case PARAM:
	case FUNC:
		break;
	default:
$		return ERRNUM = LANG_UNKNOWN_TYPE;
	}

	return 0;
}

static int asm_if(TNODE *node, name_table *table, FILE *file)
{
	VISIT(LEFT);

	PRINT("\tpush 0\n");
	PRINT("\tjne if_t%p:\n", node);

	VISIT(RIGHT->left);

	PRINT("\tjmp if_e%p\n", node);
	PRINT("if_t%p:\n", node);

	VISIT(RIGHT->right);
	PRINT("if_e%p:\n", node);

	return 0;
}

static int asm_define(TNODE *node, name_table *table, FILE *file)
{
	name_table func_table = {};
	TableCtor(&func_table);
	ERRNUM_CHECK(ERRNUM);

	PRINT("\tjmp skipf%p\n", node);

	ASM_ARG_POP = 1;
	VISIT_NEW_TABLE(LEFT, &func_table);
	ASM_ARG_POP = 0;

	PRINT("\tpush cx\n");

	VISIT_NEW_TABLE(RIGHT, &func_table);

	PRINT("skipf%p:\n", node);
	TableDtor(&func_table);

	return 0;
}

static int asm_call(TNODE *node, name_table *table, FILE *file)
{
	int func_t = std_func_check(LEFT);
	switch (func_t) {	
	case STD_SCAN:
		PRINT("\t%s\n", getStdfName(STD_SCAN));	
		PRINT("\tpop [bx+%d]\n", TableFind(table, RIGHT->right));
		break;
	case STD_PRINT:
	case STD_SHOW:
		VISIT(RIGHT);
		PRINT("\t%s\n", getStdfName(func_t));
		break;
	default:	
		VISIT(RIGHT);

		PRINT("\tpush bx\n");
		PRINT("\tpush %d\n",table->size);
		PRINT("\tadd\n");
		PRINT("\tpop bx\n");
		PRINT("\tcall %.*s\n", LEN(LEFT), ID(LEFT));
		PRINT("\tpush bx\n");
		PRINT("\tpush %d\n", table->size);
		PRINT("\tsub\n");
		PRINT("\tpop bx\n");

		fprintf(file, "\tpush ax\n");
		break;
	}
	return 0;
}

static int asm_while(TNODE *node, name_table *table, FILE *file)
{
	PRINT("while%p:\n", node);

	VISIT(LEFT);

	PRINT("\tpush 0\n");
	PRINT("\tjne wskip%p\n", node);
	PRINT("\tjmp wend%p\n", node);
	PRINT("wskip%p:\n", node);

	VISIT(RIGHT);

	PRINT("\tjmp while%p", node);
	PRINT("\nwend%p:\n", node);

	return 0;	
}

static int asm_id(TNODE *node, name_table *table, FILE *file)
{
	if (TYPE(PARENT) == FUNC) {
		PRINT("%.*s:\n\tpop cx\n",LEN(node), ID(node));

	} else if (TYPE(PARENT) == PARAM && ASM_ARG_POP) {
		int addr = TableInsert(table, node);
		ERRNUM_CHECK(ERRNUM);

		PRINT("\tpop [bx+%d]\n", addr);
	} else {
		int addr = TableFind(table, node);
		if (addr < 0) {
			fprintf(stderr, "ERROR: cant find var %.*s\n", 
					LEN(node), ID(node));
			return ERRNUM = TRANSL_UNINIT_VAR;
		}

		if (LEFT) {
			PRINT("\tpop dx\n");
			PRINT("\tpush bx\n");
			PRINT("\tpush dx\n");
			PRINT("\tadd\n");
			PRINT("\tpop bx\n");
		}
		if (TYPE(PARENT) == ID)	
			PRINT("\tpush [bx+%d]\n", addr);
		else 
			PRINT("\t%s [bx+%d]\n", 
					(ASM_ARG_POP == 1) ? "pop" : "push", addr);



		if (LEFT) {
			PRINT("\tpush bx\n");
			PRINT("\tpush dx\n");
			PRINT("\tsub\n");
			PRINT("\tpop bx\n");
		}
	}

	return 0;
}

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
	STD_FUNC_CMP("Show",       STD_SHOW, 4)

	return -1;
}

#undef STD_FUNC_CMP
#undef VISIT
#undef LEFT
#undef RIGHT
#undef PRINT
#undef NUM
#undef STR
#undef TYPE
#undef ID
#undef LEN
#undef $
