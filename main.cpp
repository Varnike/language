#include "lang.h"
#include <cstdlib>
#include "X86-64/backend64.h"

int main()
{
	LangProcces("test64.in");
	//lang64_compile(NULL, "out");
	
	//label_link label = {};
	//LabelLinkCtor(&label);
	//LabelLinkDtor(&label);
	ERRNUM_CHECK(ERRNUM);
	//lang LANG = lang("check.txt");
	//printf("Answer is %f\n", LANG.GetG());
}
