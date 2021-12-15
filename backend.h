#ifndef BACKEND_H
#define BACKEND_Y

#include <ctype.h>
#include <stdio.h>
#include <assert.h>
#include <cstdlib>
#include "tree.h"
#include "config.h"
#include "name_table.h"

int LangTranslate(TNODE *root, const char *name_out);

#endif // BACKEND_H
