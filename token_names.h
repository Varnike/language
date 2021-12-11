#ifndef TOKEN_NAMES_H
#define TOKEN_NAMES_H

#include "include/error.h"
#include "terminals.h"

//TODO one big func
const char *get_unoper_name(int type);
const char *getRelopName(int type);
const char *getTermName(int type);

#endif // TOKEN_NAMES_H
