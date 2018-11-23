#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include "stack.h"
#include "calculatorUtils.h"

#define NUMBER_TYPE 0
#define CHAR_TYPE 1

struct Input // TODO rename
{
    int type;
    int value;
};

#endif