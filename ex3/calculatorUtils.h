#ifndef CALCULATOR_UTILS_H
#define CALCULATOR_UTILS_H

#include "stack.h"
#include "Calculator.h"

struct Input popInput(struct Stack* stack);

void pushInput(struct Stack* stack, struct Input input);

struct Input peekInput(struct Stack* stack);

#endif