#include "backend64.h"
#include "asm_instr.h"

static int update_hdr(comp_data *cdata);
static int write_programm(comp_data *cdata, const char *pr_name);
static int set_ex_headers(comp_data *cdata);
static int trav_oper_node
	(TNODE *node, name_table *table, FILE *file, comp_data *cdata);

/** To determine whether it is necessary
 *  to push or pop args(if it is function params)
 */
int X64_ARG_POP = 0;

void test(comp_data *cdata)
{
	printf("sizes: %d, %d\n", 
			ARRAY_SIZE(mov_ab), ARRAY_SIZE(exit0));
	$$
	WRITE_IM(PROGRAMM_ALIGN, uint64_t);
	WRITE_OP(push_rbp);
	WRITE_OP(mov_bpsp);
/*	$$
	WRITE_OP(movabs_rax);
	WRITE_IM(1488, uint64_t);
	WRITE_OP(mov_ab);
	WRITE_OP(mov_ac);
	$$
	WRITE_OP(exit0);
	$$
*/
}

inline void program_exit0(comp_data *cdata)
{
	WRITE_OP(pop_rbp);
	WRITE_OP(exit0);
}

int lang64_compile(TNODE *root, const char *name_out)
{
	comp_data cdata = {};
	cdata.buff = (unsigned char *)
		calloc(COMP_BUFF_SIZE, sizeof(char));
	CHECK_(!cdata.buff, CALLOC_ERR);

	set_ex_headers(&cdata);
	test(&cdata);

	FILE *file_out = fopen(name_out, "w");
	CHECK_(!file_out, FOPEN_ERR);

	name_table table = {};
	TableCtor(&table);
	ERRNUM_CHECK(ERRNUM);

	trav_compile(root, &table, NULL, &cdata);
	program_exit0(&cdata);
	

	update_hdr(&cdata);
	write_programm(&cdata, name_out);

	return 0;
}


static int set_ex_headers(comp_data *cdata)
{
	CHECK_(!cdata, COMP_NULLPTR_ERR);
	CHECK_(!cdata->buff, COMP_NULL_BUFF);

	setup_elfhdr((Elf64_Ehdr *)(cdata->buff));
	cdata->ip += sizeof(Elf64_Ehdr);
	setup_phdr((Elf64_Phdr *)(cdata->buff + cdata->ip));
	cdata->ip += sizeof(Elf64_Phdr);

	return 0;
}

static int update_hdr(comp_data *cdata)
{
	CHECK_(!cdata, COMP_NULLPTR_ERR);
	CHECK_(!cdata->buff, COMP_NULL_BUFF);

	int memsz = cdata->ip - sizeof(PROGRAMM_ALIGN) 
		- sizeof(Elf64_Ehdr)- sizeof(Elf64_Phdr);

	((Elf64_Phdr *)(cdata->buff + sizeof(Elf64_Ehdr)))->p_memsz
	       	= memsz;

	((Elf64_Phdr *)(cdata->buff + sizeof(Elf64_Ehdr)))->p_filesz
	       	= memsz;

	return 0;
}

static int write_programm(comp_data *cdata, const char *pr_name)
{
	CHECK_(!cdata, COMP_NULLPTR_ERR);
	CHECK_(!cdata->buff, COMP_NULL_BUFF);

	FILE *out = fopen(pr_name, "wb");
	CHECK_(!out, FOPEN_ERR);

	fwrite(cdata->buff, cdata->ip, sizeof(char), out);
	fclose(out);
}

int trav_compile
	(TNODE *node, name_table *table, FILE *file, comp_data *cdata)
{
	if (ERRNUM)
		return ERRNUM;

	switch (TYPE(node)) {
	case STMT:
		VISIT(RIGHT);
		VISIT(LEFT);

		return 0;
	case OPER:
		if (STR(node) == OP_ASG) {
			//TODO optimization
			VISIT(RIGHT);

			int addr = TableFind(table, LEFT);
			if (addr < 0)
				addr = TableInsert(table, LEFT);
			
			addr *= -8;
			WRITE_OP(pop_mem_rbp);
			WRITE_IM(addr, int32_t);
			return 0;
		} else {
			trav_oper_node(node, table, file, cdata);
		}
		break;
	}
	$
/////////////////////////////////////////////////
	VISIT(LEFT);
	VISIT(RIGHT);
/////////////////////////////////////////////////
	
	switch (TYPE(node)) {
	case OPER:	
		break;
	case CONST:
		WRITE_OP(push);
		WRITE_IM(NUM(node), uint32_t);
		break;
	}
}

int trav_oper_node
	(TNODE *node, name_table *table, FILE *file, comp_data *cdata)
{
	if (LEFT)
		trav_oper_node(LEFT, table, file, cdata);
	if (RIGHT)	
		trav_oper_node(RIGHT, table, file, cdata);

	switch (TYPE(node)) {
	case OPER:
		if (TYPE(RIGHT) == CONST) {
			WRITE_OP(movabs_rbx);
			WRITE_IM(NUM(RIGHT), uint64_t);
		} else {
			WRITE_OP(pop_rbx);
		}

		if (TYPE(LEFT) == CONST) {
			WRITE_OP(movabs_rax);
			WRITE_IM(NUM(LEFT), uint64_t);
		} else {
			WRITE_OP(pop_rax);
		}

		switch (STR(node)) {
		case OP_ADD:
			WRITE_OP(add_rax_rbx);
			break;
		case OP_MUL:
			WRITE_OP(imul_rax_rbx);
#ifdef FLOAT_AS_INT
			WRITE_OP(xor_rdx);
			WRITE_OP(movabs_rbx);
			WRITE_IM(1000, uint32_t);
			WRITE_OP(div_rbx);
#endif
			break;
		case OP_DIV:
			WRITE_OP(xor_rdx);
			WRITE_OP(div_rbx);
			break;
		case OP_SUB:
			WRITE_OP(sub_rax_rbx);
			break;
		default:
			ERRNUM = LANG_UNKNOWN_TYPE;
			return NULL;	
		}

		WRITE_OP(push_rax);
		break;
	case CONST:
		// TODO for args
		break;
	}

	return 0;
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
