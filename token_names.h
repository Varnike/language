#ifndef TOKEN_NAMES_H
#define TOKEN_NAMES_H

#include "include/error.h"
#include "terminals.h"

const char *get_unoper_name(int type);
const char *getRelopName(int type);
const char *getTermName(int type);
const char *getAsmRelop(int type);
const char *getStdfName(int type);
const char *getAsmOper(int type);

#endif // TOKEN_NAMES_H
