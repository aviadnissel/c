#ifndef CALCULATOR_UTILS_H
#define CALCULATOR_UTILS_H

#include "stack.h"
#include "input.h"


static const int PLUS = '+';

static const int MINUS = '-';

static const int MUL = '*';

static const int DIV = '/';

static const int POW = '^';

static const int LEFT_PARENTHESIS = '(';

static const int LEFT_PARENTHESIS = ')';

struct Input popInput(struct Stack* stack);
void pushInput(struct Stack* stack, struct Input input);
struct Input peekInput(struct Stack* stack);

int isChar(struct Input input);
int isOperator(struct Input input);
int isRightParenthesis(struct Input input);
int isLeftParenthesis(struct Input input);
int isOperand(struct Input input);
void printInputs(struct Input* inputs, int inputsSize);

#endif
