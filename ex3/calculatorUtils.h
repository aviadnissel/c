#ifndef CALCULATOR_UTILS_H
#define CALCULATOR_UTILS_H

#include "stack.h"
#include "input.h"

#define ADD '+'
#define SUB '-'
#define MUL '*'
#define DIV '/'
#define POW '^'

#define RIGHT_PARENTHESIS ')'
#define LEFT_PARENTHESIS '('

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
