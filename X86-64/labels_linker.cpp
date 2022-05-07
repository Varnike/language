#include "labels_linker.h"

int LabelLinkCtor(label_link *labl)
{
	CHECK_(!labl, 	LANG_NULLPTR_ERR);

	labl->entry = (func_entry*)calloc(sizeof(func_entry), 
			MAX_LL_BUFF_SIZE);
	labl->calls = (func_calls*)calloc(sizeof(func_calls), 
			MAX_LL_BUFF_SIZE);
	CHECK_(!labl->entry || !labl->calls, CALLOC_ERR);

	return 0;
}

int LabelLinkDtor(label_link *labl)
{
	CHECK_(!labl, 	LANG_NULLPTR_ERR);

	free(labl->entry);
	free(labl->calls);

	return 0;
}

int LabelLinkAddEntry(label_link *labl, TNODE *node, unsigned char *addr)
{
	CHECK_(!labl, 		LANG_NULLPTR_ERR);
	CHECK_(!labl->entry, 	LANG_NULLPTR_ERR);
	CHECK_(!labl->calls, 	LANG_NULLPTR_ERR);

	labl->entry[labl->e_size].node = node;
	labl->entry[labl->e_size++].addr = addr;

	return 0;
}

#define ID(node_name)			(node_name).node->data.value.id
#define LEN(node_name)			(node_name).node->data.len
#define MAX(x, y) 			(x > y ? x : y)
#define MIN(x, y) 			(x < y ? x : y)

int LabelLinkAddCall(label_link *labl, TNODE *node, unsigned char *addr)
{
	CHECK_(!labl, 		LANG_NULLPTR_ERR);
	CHECK_(!labl->entry, 	LANG_NULLPTR_ERR);
	CHECK_(!labl->calls, 	LANG_NULLPTR_ERR);

	labl->calls[labl->c_size].node = node;
	labl->calls[labl->c_size++].addr = addr;

	printf("Added CALL[%d]: <%.*s>, <%p>\n", labl->c_size - 1,
		      	node->data.len, node->data.value.id, addr);

	return 0;
}

#define  $ 			printf("\t\t\t---ON LINE %d IN FUNCTION %s---\n", __LINE__, __func__);

int LabelLinkConnect(label_link *labl)
{	
	CHECK_(!labl, 		LANG_NULLPTR_ERR);
	CHECK_(!labl->entry, 	LANG_NULLPTR_ERR);
	CHECK_(!labl->calls, 	LANG_NULLPTR_ERR);

	for (int it = 0; it < labl->c_size; it++)
	for (int srch = 0; srch < labl->e_size; srch++) {
		printf("it = %d, srch = %d", it, srch);

		if (LEN(labl->calls[it]) == LEN(labl->entry[srch]) && 
		    strncmp(ID(labl->calls[it]), ID(labl->entry[srch]),
			       	LEN(labl->calls[it])) == 0) {

			*(int32_t *)labl->calls[it].addr = 
				(int64_t)labl->entry[srch].addr - 
				(int64_t)labl->calls[it].addr - 
				sizeof(int32_t);
		}
	}
	
	return 0;
}

#undef ID
#undef LEN
#undef MAX
#undef MIN
