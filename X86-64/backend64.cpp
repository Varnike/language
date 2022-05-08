#include "backend64.h"
#include "asm_instr.h"

static int update_hdr(comp_data *cdata);
static int write_programm(comp_data *cdata, const char *pr_name);
static int set_ex_headers(comp_data *cdata);
static int func_epilog(comp_data *cdata, name_table *table);
static int func_prolog(comp_data *cdata);
static int set_label_src(comp_data *cdata);
static int upd_label_src(comp_data *cdata);
static int write_std_func(comp_data *cdata);
static int addstd_n_link(comp_data *cdata);

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
static int trav_funcdef_node
	(TNODE *node,  comp_data *cdata);
static int trav_call_node
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
	int rsp = func_prolog(cdata);
	
	//WRITE_OP(movabs_rax);
	//WRITE_IM(777, int64_t);

	
	return rsp;
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

	LabelLinkCtor(&cdata.labels);
	ERRNUM_CHECK(ERRNUM);

	set_ex_headers(&cdata);

	FILE *file_out = fopen(name_out, "w");
	CHECK_(!file_out, FOPEN_ERR);

	table.rsp_pos = test(&cdata);
	trav_compile(root, &table, &cdata);
	program_exit0(&cdata, &table);
	
	addstd_n_link(&cdata);
	
	update_hdr(&cdata);
	write_programm(&cdata, name_out);

	return 0;
}

#define STD_FUNC_CMP(name, type, size)				\
	if (strncmp(ID(node), name,				\
		(size > LEN(node)) ? size : LEN(node)) == 0) {	\
		return type;					\
	} else

int std_func_check(TNODE *node)
{
	if (!node || TYPE(node) != ID)
		return -1;

	STD_FUNC_CMP("Introduce",  STD_SCAN, 9)
	STD_FUNC_CMP("Conclusion", STD_PRINT, 10)
	STD_FUNC_CMP("Show",       STD_SHOW, 4)

	return -1;
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
	$
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


	*(int32_t *)&cdata->buff[table->rsp_pos] = table->var_cnt * 8;
	WRITE_OP(add_rsp);
	WRITE_IM(table->var_cnt * 8, int32_t);
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
	case CALL:
		return trav_call_node(node, table, cdata);
	case DEFINE:
		return trav_funcdef_node(node, cdata);
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
			if (addr == 0)
				addr = TableInsert(table, LEFT);
			printf("ASIGN ADDRESS : %d\n", addr);
			WRITE_OP(mov_mrbp_rax);
			WRITE_IM(addr, int32_t);
			return 0;
		} else {
			trav_oper_node(node, table,  cdata);
			return 0;
		}
		break;
	default:
		break;
	}
	
/////////////////////////////////////////////////
	VISIT(LEFT);
	VISIT(RIGHT);
/////////////////////////////////////////////////
	
	switch (TYPE(node)) {
	case RETURN:
		func_epilog(cdata, table);
		WRITE_OP(ret);
		break;
	case OPER:	
		break;
	case CONST:
		WRITE_OP(movabs_rax);
		WRITE_IM(NUM(node), int64_t);
		break;
	case ID:
		{
		int addr = TableFind(table, node);
		if (addr == 0) {
			fprintf(stderr, "ERROR: cant find var %.*s\n", 
					LEN(node), ID(node));
			return ERRNUM = TRANSL_UNINIT_VAR;
		}

		WRITE_OP(mov_rax_mrbp);
		WRITE_IM(addr, int32_t);	// TODO in const
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
		WRITE_OP(setl_al);
		break;
	case RELOP_LE:
		WRITE_OP(setle_al);
		break;
	case RELOP_EQ:
		WRITE_OP(sete_al);
		break;
	case RELOP_NE:
		WRITE_OP(setne_al);
		break;
	case RELOP_GT:
		WRITE_OP(setb_al);
		break;
	case RELOP_GE:
		WRITE_OP(setbe_al);
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

static int trav_init_args(TNODE *node, name_table *table)
{
	if (LEFT) 
		trav_init_args(LEFT, table);
	if (RIGHT) 
		trav_init_args(RIGHT, table);
		
	if (TYPE(node) == ID)
		TableAddArg(table, node);
	
	return 0;
}

int trav_funcdef_node(TNODE *node, comp_data *cdata)
{
	name_table func_table = {};
	TableCtor(&func_table);
	ERRNUM_CHECK(ERRNUM);

	WRITE_OP(jmp);
	SET_ENTRY(skip_f);
	LabelLinkAddEntry(&cdata->labels, LEFT->left, 
				cdata->buff + cdata->ip);


	trav_init_args(LEFT->right, &func_table);
	
	func_table.rsp_pos = func_prolog(cdata);
	trav_compile(RIGHT, &func_table, cdata);

	UPDATE_ENTRY(skip_f);

	TableDtor(&func_table);
	return 0;
}

int trav_call_node(TNODE *node, name_table *table,  comp_data *cdata)
{
	int func_t = std_func_check(LEFT);
	/*
	switch (func_t) {	
	case STD_SCAN:
		WRITE_OP(call);
		WRITE_IM(STDIN_FUNC_ADDR - cdata->ip - 4, int32_t);
		return 0;
	case STD_PRINT:
		VISIT(RIGHT);
		WRITE_OP(call);
		WRITE_IM(STDOUT_FUNC_ADDR - cdata->ip - 4, int32_t);

		return 0;
	default:
		break;
	}
	*/

	int arg_cnt = 0;
	TNODE *func_name = LEFT;

	node = RIGHT;
	do {
		VISIT(RIGHT);
		WRITE_OP(push_rax);
		arg_cnt++;
	} while (node = LEFT);

	WRITE_OP(call);
	LabelLinkAddCall(&cdata->labels, func_name, 
			cdata->buff +cdata->ip);
	WRITE_IM(0x14881488, int32_t);

	WRITE_OP(add_rsp);
	WRITE_IM(arg_cnt * 8, int32_t);

	return 0;
}

int write_std_func(comp_data *cdata)
{
	for ( ; cdata->ip != STDOUT_FUNC_ADDR; )
		WRITE_OP(nop);

	WRITE_OP(std_print);

	for ( ; cdata->ip != STDIN_FUNC_ADDR; )
		WRITE_OP(nop);

	WRITE_OP(std_read);

	return 0;
}

// NOTE: do not use LabelLink struct after this function
int addstd_n_link(comp_data *cdata)
{
	char stdin_name[]  = "Introduce";
	char stdout_name[] = "Conclusion";

	DATA std_dat = {};
	std_dat.id = stdin_name;

	TNODE node_in = {};
	node_in.data = {
		.value     = std_dat,
		.len       = strlen(stdin_name),
		.data_type = ID
	};

	LabelLinkAddEntry(&cdata->labels, &node_in, 
				cdata->buff + cdata->ip);
	WRITE_OP(std_read);

	TNODE node_out = node_in;
	node_out.data.value.id = stdout_name;
	node_out.data.len      = strlen(stdout_name);
	LabelLinkAddEntry(&cdata->labels, &node_out, 
				cdata->buff + cdata->ip);
	
	WRITE_OP(std_print);

	LabelLinkConnect(&cdata->labels);

	return 0;
}

#undef SET_ENTRY
#undef UPDATE_ENTRY
#undef SET_DST
#undef IP

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
