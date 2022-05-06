#include "backend64.h"
#include "asm_instr.h"

static int update_hdr(comp_data *cdata);
static int write_programm(comp_data *cdata, const char *pr_name);
static int set_ex_headers(comp_data *cdata);
static int func_epilog(comp_data *cdata, name_table *table);
static int func_prolog(comp_data *cdata);
static int set_label_src(comp_data *cdata);
static int upd_label_src(comp_data *cdata);

static int trav_while_node
	(TNODE *node, name_table *table,  comp_data *cdata);
static int trav_oper_node
	(TNODE *node, name_table *table,  comp_data *cdata);
static int trav_relop_node
	(TNODE *node, name_table *table,  comp_data *cdata);
static int set_cmp_cond
	(TNODE *node, name_table *table,  comp_data *cdata);
static int trav_if_node
	(TNODE *node, name_table *table,  comp_data *cdata);


/** To determine whether it is necessary
 *  to push or pop args(if it is function params)
 */
int X64_ARG_POP = 0;


int test(comp_data *cdata)
{
	printf("sizes: %d, %d\n", 
			ARRAY_SIZE(mov_rax_rbx), ARRAY_SIZE(exit0));
	WRITE_IM(PROGRAMM_ALIGN, uint64_t);
	$$
	//WRITE_OP(jne);
	//WRITE_IM(0, int32_t);
	return func_prolog(cdata);
}


inline void program_exit0(comp_data *cdata, name_table *table)
{
	*(int32_t *)&cdata->buff[table->rsp_pos] = table->size * 8;
	WRITE_OP(add_rsp);
	WRITE_IM(table->size * 8, int32_t);
	WRITE_OP(pop_rbp);
	WRITE_OP(exit0);
}


int lang64_compile(TNODE *root, const char *name_out)
{
	comp_data cdata = {};
	cdata.buff = (unsigned char *)
		calloc(COMP_BUFF_SIZE, sizeof(char));
	CHECK_(!cdata.buff, CALLOC_ERR);

	name_table table = {};
	TableCtor(&table);
	ERRNUM_CHECK(ERRNUM);

	set_ex_headers(&cdata);

	FILE *file_out = fopen(name_out, "w");
	CHECK_(!file_out, FOPEN_ERR);


	table.rsp_pos = test(&cdata);
	trav_compile(root, &table, &cdata);
	program_exit0(&cdata, &table);
	

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


static int func_epilog(comp_data *cdata, name_table *table)
{
	CHECK_(!cdata, COMP_NULLPTR_ERR);
	CHECK_(!cdata->buff, COMP_NULL_BUFF);
	CHECK_(!table, COMP_NULLPTR_ERR);


	*(int32_t *)&cdata->buff[table->rsp_pos] = table->size * 8;
	WRITE_OP(add_rsp);
	WRITE_IM(table->size * 8, int32_t);
	WRITE_OP(pop_rbp);

	return 0;
}


static int func_prolog(comp_data *cdata)
{
	CHECK_(!cdata, COMP_NULLPTR_ERR);
	CHECK_(!cdata->buff, COMP_NULL_BUFF);

	WRITE_OP(push_rbp);
	WRITE_OP(mov_rbp_rsp);
	WRITE_OP(sub_rsp);

	int rsp_pos = cdata->ip;
	WRITE_IM(0x1488, int32_t);

	return rsp_pos;
}


int trav_compile
	(TNODE *node, name_table *table,  comp_data *cdata)
{
	if (ERRNUM)
		return ERRNUM;

	switch (TYPE(node)) {
	case WHILE:
		return trav_while_node(node, table, cdata);
	case IF:
		return trav_if_node(node, table, cdata);
	case RELOP:
		return trav_relop_node(node, table,  cdata);
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

			WRITE_OP(mov_mrbp_rax);
			WRITE_IM(addr, int32_t);
			return 0;
		} else {
			trav_oper_node(node, table,  cdata);
			return 0;
		}
		break;
	}
	
/////////////////////////////////////////////////
	VISIT(LEFT);
	VISIT(RIGHT);
/////////////////////////////////////////////////
	
	switch (TYPE(node)) {
	case OPER:	
		break;
	case CONST:
		WRITE_OP(movabs_rax);
		WRITE_IM(NUM(node), int64_t);
		break;
	case ID:
		{
		int addr = TableFind(table, node);
		if (addr < 0) {
			fprintf(stderr, "ERROR: cant find var %.*s\n", 
					LEN(node), ID(node));
			return ERRNUM = TRANSL_UNINIT_VAR;
		}

		WRITE_OP(mov_rax_mrbp);
		WRITE_IM(addr * (-8), int32_t);	// TODO in const
						//
		}
		break;
	default:
		break;
	}
}

int trav_oper_node(TNODE *node, name_table *table,  comp_data *cdata)
{
	VISIT(LEFT);
	WRITE_OP(push_rax);

	VISIT(RIGHT);
	WRITE_OP(mov_rbx_rax);
	WRITE_OP(pop_rax);

	switch (TYPE(node)) {
	case OPER:
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
	}

	return 0;
}

int trav_relop_node(TNODE *node, name_table *table, comp_data *cdata)
{
		VISIT(LEFT);
		WRITE_OP(push_rax);

		VISIT(RIGHT);

		WRITE_OP(mov_rbx_rax);
		WRITE_OP(pop_rax);
		WRITE_OP(cmp_rax_rbx);

		set_cmp_cond(node, table, cdata);
		return 0;
}

int set_cmp_cond(TNODE *node, name_table *table,  comp_data *cdata)
{
	WRITE_OP(movabs_rax);
	WRITE_IM(0x0, int64_t);

	switch (STR(node)) {
	case RELOP_LT:
		WRITE_OP(setb_al);
		break;
	case RELOP_LE:
		WRITE_OP(setbe_al);
		break;
	case RELOP_EQ:
		WRITE_OP(sete_al);
		break;
	case RELOP_NE:
		WRITE_OP(setne_al);
		break;
	case RELOP_GT:
		WRITE_OP(seta_al);
		break;
	case RELOP_GE:
		WRITE_OP(setae_al);
		break;
	}

	return 0;
}

#define SET_ENTRY(name)					\
	int name = cdata->ip;				\
	WRITE_IM(UNSET_DST, int32_t);

#define UPDATE_ENTRY(src)				\
	*(int32_t *)&cdata->buff[src] = 		\
			cdata->ip - src - sizeof(int32_t);

#define SET_DST(dst)					\
	int dst = cdata->ip;

#define IP		cdata->ip
int trav_if_node(TNODE *node, name_table *table,  comp_data *cdata)
{
	VISIT(LEFT);

	WRITE_OP(test_rax_rax);
	WRITE_OP(jne);
	SET_ENTRY(if_dst);

	VISIT(RIGHT->left);

	WRITE_OP(jmp);
	SET_ENTRY(jmp_skip);	
	printf("!!!\t%d ***** %d ***** %d\n",
			cdata->ip, if_dst, 2);
	UPDATE_ENTRY(if_dst);

	VISIT(RIGHT->right);

	UPDATE_ENTRY(jmp_skip);

	return 0;
}

int trav_while_node(TNODE *node, name_table *table,  comp_data *cdata)
{
	WRITE_OP(jmp);
	SET_ENTRY(jmp_cond);
	SET_DST(while_start);

	VISIT(RIGHT);
	
	UPDATE_ENTRY(jmp_cond);

	VISIT(LEFT);

	WRITE_OP(jne);
	WRITE_IM(while_start - IP - 4, int32_t);

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
