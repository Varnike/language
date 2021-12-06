#include "lang.h"

double GetG(const char **str)
{
$	double val = GetE(str);
	Require(**str == '$');

	return val;
}

double GetE(const char **str)
{
$	double val = GetT(str);
	
	while(**str == '+' || **str == '-') {
		char op = *((*str)++);
		double val2 = GetT(str);
		
		if (op == '+')
			val += val2;
		else 
			val -= val2;
	}

	return val;
}

double GetT(const char **str)
{
$	double val = GetP(str);
	
	while(**str == '*' || **str == '/') {
		char op = *((*str)++);
		double val2 = GetP(str);
		
		if (op == '*')
			val *= val2;
		else 
			val /= val2;
	}

	return val;
}

double GetP(const char **str)
{
$	if (**str == '(') {
		(*str)++;

		int val = GetE(str);
		Require(**str == ')');
		(*str)++;

		return val;
	} else {
		return GetN(str);
	}
}

double GetN(const char **str)
{
	double val = 0;
	int cnt = 0;

	while ('0' <= **str && **str <= '9') {
$		val = val * 10 + (**str - '0');

		(*str)++;
		cnt++;
	}

	if (cnt == 0)
		SyntaxError();

	return val;
}

int Require(int cond)
{
$	if (!cond)
		SyntaxError();
}

int SyntaxError()
{
$	assert(!"Syntax ERROR!");
}
