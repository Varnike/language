#ifndef ASM_INSTR
#define ASM_INSTR

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#if 0
enum REGS {
	RAX, RBX, RCX, RDX,
	RSI, RDI, RSB, RBP,
	R9,  R9,  R10, R11,
	R12, R13, R14, R15
};
#endif
/**
 * writes array of opcodes into program buffer
 * in structure named cdata(comp_data)
 */
#define WRITE_OP(opcodes)					\
	do {							\
		for (size_t i = 0; 				\
			i!= ARRAY_SIZE(opcodes); i++)		\
			cdata->buff[cdata->ip++] = opcodes[i];	\
	} while(0)

/**
 * writes immidiate of specified type into program buffer
 * in structure named cdata(comp_data)
 */
#define WRITE_IM(imm, type)				\
	do {						\
	$	*(type *)(cdata->buff +cdata->ip) = imm;\
		cdata->ip += sizeof(type);		\
	} while(0);

/*	mov instructions 			*/
const unsigned char mov_ab[] = {0x48, 0x89, 0xd8};
const unsigned char mov_ba[] = {0x48, 0x89, 0xc3};
const unsigned char mov_ac[] = {0x48, 0x89, 0xc8};
const unsigned char mov_ca[] = {0x48, 0x89, 0xc1};
const unsigned char mov_bc[] = {0x48, 0x89, 0xcb};
const unsigned char mov_cb[] = {0x48, 0x89, 0xd9};
const unsigned char mov_bpsp[] = {0x48, 0x89, 0xe5};

const unsigned char movabs_rax[] = {0x48, 0xb8};
const unsigned char movabs_rbx[] = {0x48, 0xbb};
const unsigned char movabs_rcx[] = {0x48, 0xb9};

/*	xor					*/
const unsigned char xor_rdx[]  = {0x48, 0x31, 0xd2};

/*	push/pop instructions			*/
const unsigned char push_rax[]    = {0x50};
const unsigned char push_rbx[]    = {0x53};
const unsigned char push_rbp[]	  = {0x55};
const unsigned char pop_rax[]     = {0x58};
const unsigned char pop_rbx[]     = {0x5b};
const unsigned char pop_rbp[]	  = {0x5d};
const unsigned char push[]        = {0x68};
const unsigned char pop_mem_rbp[] = {0x8f, 0x85};

/*	arithmetics instructions		*/
const unsigned char add_rax_rbx[]   = {0x48, 0x01, 0xd8};
const unsigned char sub_rax_rbx[]   = {0x48, 0x29, 0xd8};
const unsigned char imul_rax_rbx[]  = {0x48, 0x0f, 0xaf, 0xc3};
const unsigned char div_rbx[]	    = {0x48, 0xf7, 0xfb};

/*	programm alighn after headers		*/
const uint64_t PROGRAMM_ALIGN = 0;

/*	exit(0)					*/
const unsigned char exit0[] = {0xb8, 0x01, 0x00, 0x00, 0x00, 0xcd, 0x80};
#endif // ASM_INSTR
