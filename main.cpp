#include "lang.h"
#include <cstdlib>

int main()
{
	char *str = NULL;
	scanf("%ms", &str);
	printf("%s\n", str);

	char *old = str;
	printf("Answer is %f\n", GetG((const char **)(&str)));

	free(old);
}
