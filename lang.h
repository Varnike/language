#ifndef LANG_H
#define LANG_H

#include "stdio.h"
#include "assert.h"

#define  $ 			printf("\t\t\t---ON LINE %d IN FUNCTION %s---\n", __LINE__, __func__);

double GetG(const char **str);
double GetE(const char **str);
double GetT(const char **str);
double GetP(const char **str);
double GetN(const char **str);

int Require(int cond);
int SyntaxError();
#endif // LANG_H
