/**
 * @file calculatrUtils.h
 * @author Aviad Nissel <aviad.nissel@mail.huji.ac.il>
 *
 * Header for the calculator utils.
 */

#ifndef CALCULATOR_UTILS_H
#define CALCULATOR_UTILS_H


/* --- Includes --- */

#include "stack.h"
#include "input.h"


/* --- Constants --- */

/** Addition operator. */
#define ADD '+'

/** Substraction operator. */
#define SUB '-'

/** Multiplication operator. */
#define MUL '*'

/** Division operator. */
#define DIV '/'

/** Power operator. */
#define POW '^'

/** Right parenthesis. */
#define RIGHT_PARENTHESIS ')'

/** Left parenthesis. */
#define LEFT_PARENTHESIS '('


/* --- Functions --- */

/**
 * Pops an Input from given Stack.
 * @param stack the stack to pop from, should be an Input stack.
 * @return the popped input.
 */
struct Input popInput(struct Stack* stack);

/**
 * Pushes an Input to given Stack.
 * @param stack the stack to push to, should be an Input stack.
 * @param input the Input to push.
 */
void pushInput(struct Stack* stack, struct Input input);

/**
 * Peeks the head of the Stack without removing it.
 *
 * @param stack the stack to peek, should be an Input stack.
 * @return the peeked Input.
 */
struct Input peekInput(struct Stack* stack);

/**
 * Checks if given Input is a char type.
 *
 * @param input the Input to check.
 * @return 1 for true, 0 for false.
 */
int isChar(struct Input input);

/**
 * Checks if given Input contains an operator.
 *
 * @param input the Input to check.
 * @return 1 for true, 0 for false.
 */
int isOperator(struct Input input);

/**
 * Checks if given Input contains left parenthesis.
 *
 * @param input the Input to check.
 * @return 1 for true, 0 for false.
 */
int isRightParenthesis(struct Input input);

/**
 * Checks if given Input contains right parenthesis.
 *
 * @param input the Input to check.
 * @return 1 for true, 0 for false.
 */
int isLeftParenthesis(struct Input input);

/**
 * Checks if given Input contains an operand.
 *
 * @param input the Input to check.
 * @return 1 for true, 0 for false.
 */
int isOperand(struct Input input);

/**
 * Prints the given Input array.
 * @param inputs the inputs to print.
 * @param inputsSize the size of the input array.
 */
void printInputs(struct Input* inputs, int inputsSize);

#endif
