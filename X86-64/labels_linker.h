#ifndef LABEL_LINKER
#define LABEL_LINKER
#include "../tree.h"
struct func_entry {
	unsigned char *addr = 0;
	TNODE *node;
};

struct func_calls {
	unsigned char *addr = 0;
	TNODE *node;
};

const int MAX_LL_BUFF_SIZE = 50;

struct label_link {
	func_entry *entry;
	func_calls *calls;
	int e_size = 0;
	int c_size = 0;
};

int LabelLinkCtor(label_link *labl);
int LabelLinkDtor(label_link *labl);
int LabelLinkAddEntry(label_link *labl, TNODE *node,unsigned char *addr);
int LabelLinkAddCall(label_link *labl, TNODE *node, unsigned char *addr);
int LabelLinkConnect(label_link *labl);
#endif // LABEL_LINKER
