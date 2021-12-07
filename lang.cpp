#include "lang.h"

static void print_token(TNODE *node);

lang::lang(char *namein)
{// TODO
	init("check.txt");

	str = btext.buff;
	parse();

	for (int i = 0; i != token_arr.size; i++) {
		print_token(token_arr.data[i]);
	}
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

int lang::parse()
{
	CHECK_(str == nullptr, LANG_NULLPTR_ERR);
	
	node_data node_val = {};

	token_arr.data = (TNODE **)calloc(MAX_TOKEN_CNT, sizeof(TNODE *));

	while (*str != '\0') {
$		if (isspace(*str)) {
			str++;
			continue;
		}
		else if (isdigit(*str))
			node_val = parse_no();
		else if (isalpha(*str))
			node_val = parse_id();
		else if (isOP(*str))
			node_val = parse_op();
		else 
			;//TODO smth
		
		ERRNUM_CHECK(ERRNUM);
		
		if (token_arr.size >= MAX_TOKEN_CNT) {
			ERRNUM = LANG_BUFFER_OVERFLOW;
			goto err_free_buffer;
		}

		TreeCtor((token_arr.data + token_arr.size++), node_val);
	}

	return 0; // TODO ???

err_free_buffer:
	free(token_arr.data);
	
	return ERRNUM;
}

node_data lang::parse_no()
{//TODO read double values
	$
	node_data tmp_data = {};

	double val = strtod(str, &str);	

	printf("number: %f\n str: %s\n", val, str);	

	tmp_data.value.num = val;
	tmp_data.data_type = CONST;

	return tmp_data;
}

node_data lang::parse_id()
{	
$	node_data tmp_data = {};
	
	CHECK_SET_ERR(!str, LANG_NULLPTR_ERR, tmp_data);	

	//char *token_str	 = (char *)calloc(MAX_ID_LEN, sizeof(char));
	//CHECK_SET_ERR(!str, CALLOC_ERR, tmp_data);
	DATA_ID(tmp_data) = str;

	int it = 0;

	while (isdigit(*str) || isalpha(*str)) {
		//token_str[it++] = *str++;
		str++;
		it++;
		/*if (it >= MAX_ID_LEN - 1) {
			free(token_str);
			ERRNUM = LANG_BUFFER_OVERFLOW;

			return tmp_data;
		}*/
	}

	//token_str[it] = '\0';

	if (isTerm(tmp_data))
		tmp_data.data_type = TERM;
	else 
		tmp_data.data_type = ID;

	tmp_data.len  = it;

	return tmp_data;
}

node_data lang::parse_op()
{//TODO char *
$	char op = *str++;

	node_data tmp_data = {data_un_c(op), OPER};

	return tmp_data;
}

int lang::isOP(char symb)
{
$	return (symb == OP_ADD || symb == OP_MUL || symb == OP_DIV || 
			symb == OP_SUB || symb == OP_PWR || symb == OP_EQ);
}

int lang::isTerm(node_data ndata)
{
	if (strncmp(DATA_ID(ndata), "if", ndata.len) == 0)
		return 1;
	
	return 0;
}

static void print_token(TNODE *node) {
	if (!node)
		return;
	printf("node : [  %p  ], datatype : [  %d  ], ", node, node->data.data_type);
	
	switch (node->data.data_type) {
	case CONST:
		printf("data : [  %lg  ], ", node->data.value.num);
		break;
	case OPER:
		printf("data : [  %c  ], ", node->data.value.str);
		break;
	case ID:
		printf("data : [  %s  ], ", node->data.value.id);
		break;
	default:
		break;
	}

	printf("\n");
//	fprintf(fout, "\tleft : [  %p  ],\tright : [  %p  ],\tparent : [  %p  ]\n", 
//			node->left, node->right, node->parent);                             

}

double lang::GetG()
{$
	double val = GetE();
	Require(*str == '$');

	return val;
}

double lang::GetE()
{$
	double val = GetT();

	while(*str == '+' || *str == '-') {
		char op = *((str)++);
		double val2 = GetT();
		
		if (op == '+')
			val += val2;
		else 
			val -= val2;
	}

	return val;
}

double lang::GetT()
{$
	double val = GetP();
	
	while(*str == '*' || *str == '/') {
		char op = *((str)++);
		double val2 = GetP();
		
		if (op == '*')
			val *= val2;
		else 
			val /= val2;
	}

	return val;
}

double lang::GetP()
{$
	if (*str == '(') {
		(str)++;

		int val = lang::GetE();
		Require(*str == ')');
		(str)++;

		return val;
	} else {
		return GetN();
	}
}

double lang::GetN()
{$
	double val = 0;
	int cnt = 0;

	while ('0' <= *str && *str <= '9') {
		val = val * 10 + (*str - '0');

		(str)++;
		cnt++;
	}

	if (cnt == 0)
		SyntaxError();

	return val;
}

int lang::Require(char cmp_symb)
{
	if (*str != cmp_symb)
		SyntaxError();
}

int lang::SyntaxError()
{
	assert(!"Syntax ERROR!");
}
