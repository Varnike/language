#include "lang.h"
#include <cstdlib>

int main()
{
	LangProcces("check.txt");
	ERRNUM_CHECK(ERRNUM);
	//lang LANG = lang("check.txt");
	//printf("Answer is %f\n", LANG.GetG());
}
