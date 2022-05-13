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
const unsigned char mov_rax_rbx[] = {0x48, 0x89, 0xd8};
const unsigned char mov_rbx_rax[] = {0x48, 0x89, 0xc3};
const unsigned char mov_rax_rcx[] = {0x48, 0x89, 0xc8};
const unsigned char mov_rcx_rax[] = {0x48, 0x89, 0xc1};
const unsigned char mov_rbx_rcx[] = {0x48, 0x89, 0xcb};
const unsigned char mov_rcx_rbx[] = {0x48, 0x89, 0xd9};
const unsigned char mov_rbp_rsp[] = {0x48, 0x89, 0xe5};

const unsigned char movabs_rax[] = {0x48, 0xb8};
const unsigned char movabs_rbx[] = {0x48, 0xbb};
const unsigned char movabs_rcx[] = {0x48, 0xb9};

const unsigned char mov_mrbp_rax[] = {0x48, 0x89, 0x85};
const unsigned char mov_rax_mrbp[] = {0x48, 0x8b, 0x85};

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
const unsigned char imul_rax_imm[]  = {0x48, 0x69, 0xc0};
const unsigned char div_rbx[]	    = {0x48, 0xf7, 0xfb};
const unsigned char sub_rsp[]	    = {0x48, 0x81, 0xec};
const unsigned char add_rsp[]	    = {0x48, 0x81, 0xc4};
const unsigned char and_rax[]	    = {0x48, 0x25};

/*	cmp instructions			*/
const unsigned char cmp_rax_rbx[]  = {0x48, 0x39, 0xd8};
const unsigned char test_rax_rax[] = {0x48, 0x85, 0xc0};

/*	set instructions			*/
const unsigned char setl_al[]  = {0x0f, 0x9c, 0xc0};
const unsigned char setle_al[] = {0x0f, 0x9e, 0xc0};
const unsigned char sete_al[]  = {0x0f, 0x94, 0xc0};
const unsigned char setne_al[] = {0x0f, 0x95, 0xc0};
const unsigned char setb_al[]  = {0x0f, 0x9f, 0xc0};
const unsigned char setbe_al[] = {0x0f, 0x9d, 0xc0};

/*	jump instructions			*/
const unsigned char jmp[] = {0xe9};
const unsigned char je[]  = {0x0f, 0x84};
const unsigned char jne[] = {0x0f, 0x85};
const unsigned char jb[]  = {0x0f, 0x82};
const unsigned char jbe[] = {0x0f, 0x86};
const unsigned char ja[]  = {0x0f, 0x87};
const unsigned char jae[] = {0x0f, 0x83};

/*	control flow instructions		*/
const unsigned char ret[]  = {0xc3};
const unsigned char call[] = {0xe8};

/*	programm alighn after headers		*/
const uint64_t PROGRAMM_ALIGN = 0;

/*	exit(0)					*/
const unsigned char exit0[] = {
	0x48, 0xC7, 0xC0, 		/* mov rax, 1 		*/
	0x01, 0x00, 0x00, 0x00,
	0x48, 0xC7, 0xC3,		/* mov rbx, 0 		*/
       	0x00, 0x00, 0x00, 0x00,
	0xCD, 0x80 			/* int 0x80 		*/
};

/*	nop					*/
const unsigned char nop[] = {0x90};

/*	std functions				*/
const unsigned char std_print[] = {
	0x55, 				/* push rbp 		*/
	0x48, 0x89, 0xe5,		/* mov rbp, rsp		*/
	0xbb, 0xcd, 0xcc, 0xcc, 0xcc,	/* mov ebx, 0xcccccccd 	*/
	0x48, 0x31, 0xff,		/* xor rdi, rdi 	*/
	0x89, 0xc1,			/* mov ecx, eax 	*/
	0xf7, 0xe3,			/* mul ebx 		*/
	0xc1, 0xea, 0x03,		/* shr edx, 3 		*/
	0x89, 0xd0,			/* mov eax, edx 	*/
	0x67, 0x8d, 0x14, 0x92,		/* lea edx, [edx + edx*4] */
	0x67, 0x8d, 0x14, 0x55,		/* lea edx, [edx*2 - 0x30]*/
       	0xd0, 0xff, 0xff, 0xff,		
	0x29, 0xd1,			/* sub ecx, edx 	*/
	0x48, 0xc1, 0xe7, 0x08,		/* shl rdi, 8 		*/
	0x48, 0x01, 0xcf,		/* add rdi, rcx 	*/
	0x48, 0x83, 0xf8, 0x00,		/* cmp rax, 0 		*/
	0x0f, 0x85, 0xd8, 0xff, 	/* jne 0xd8ffffff 	*/
	0xff, 0xff,			
	0x48, 0x89, 0xf8,		/* mov rax, rdi 	*/
	0x50,				/* push rax 		*/
	0x48, 0xc7, 0xc2, 		/* mov rdx, 8 		*/
	0x08, 0x00, 0x00, 0x00,		
	0x48, 0xc7, 0xc0,		/* mov rax, 1 		*/
       	0x01, 0x00, 0x00, 0x00,		
	0x48, 0xc7, 0xc7, 		/* mov rdi, 1 		*/
	0x01, 0x00, 0x00, 0x00,		
	0x48, 0x89, 0xee,		/* mov rsi, rbp 	*/
	0x48, 0x83, 0xee, 0x08,		/* sub rsi, 8 		*/
	0x0f, 0x05,			/* syscall 		*/
	0x58,				/* pop rax 		*/
	0x5d,				/* pop rbp 		*/
	0xc3				/* ret 			*/
};

const unsigned char std_read[] = {
	0x55, 				/* push rbp		*/
	0x48, 0x89, 0xe5, 		/* mov rbp, rsp		*/
	0x31, 0xc0, 			/* xor eax, eax		*/
	0x31, 0xff, 			/* xor edi, edi		*/
	0x48, 0x8d, 0x75, 0xf8, 	/* lea rsi, [var_8h]	*/
	0xba, 0x08, 0x00, 0x00, 0x00, 	/* mov edx, 8		*/
	0x0f, 0x05, 			/* syscall		*/
	0x48, 0x8b, 0x45, 0xf8,		/* mov rax, qword [var_8h]*/
	0x48, 0x8d, 0x7d, 0xf8,		/* lea rdi, [var_8h]	*/
	0x48, 0x31, 0xc0,		/* xor rax, rax		*/
	0x48, 0x0f, 0xb6, 0x0f,		/* movzx rcx, byte [rdi]*/
	0x48, 0x83, 0xe9, 0x30,		/* sub rcx, 0x30	*/
	0x72, 0x13,			/* jb 0x13 		*/
	0x48, 0x83, 0xf9, 0x09,		/* cmp rcx, 9 		*/
	0x77, 0x0d,			/* ja 0x0d 		*/
	0x48, 0x8d, 0x04, 0x80,		/* lea rax,[rax + rax*4]*/
	0x48, 0x8d, 0x04, 0x41,		/* lea rax,[rcx + rax*2]*/
	0x48, 0xff, 0xc7, 		/* inc rdi 		*/
	0xeb, 0xe3, 			/* jmp 0xe3 		*/
	0x5d,				/* pop rbp 		*/
	0xc3				/* ret 			*/
};
#endif // ASM_INSTR
