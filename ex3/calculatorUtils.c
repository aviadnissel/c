/**
 * @file calculatrUtils.c
 * @author Aviad Nissel <aviad.nissel@mail.huji.ac.il>
 *
 * Implementation for the calculator utils.
 */

#include "calculatorUtils.h"

struct Input popInput(struct Stack* stack)
{
	struct Input input;
	pop(stack, &input);
	return input;
}

void pushInput(struct Stack* stack, struct Input input)
{
	push(stack, &input);
}

/**
 * Coding style checks thinks there's no documentation for this.
 * Added this documentation to remove the warning.
 * See actual documentation in header file.
 */
struct Input peekInput(struct Stack* stack)
{
	struct Input headData;
	headData = popInput(stack);
	pushInput(stack, headData);
	return headData;
}

int isChar(struct Input input)
{
	return input.type == CHAR_TYPE;
}

int isOperator(struct Input input)
{
	char c;
	if (!isChar(input))
	{
		return 0;
	}
	c = (char) input.value;
	return c == POW || c == ADD || c == SUB || c == MUL || c == DIV;
}

int isRightParenthesis(struct Input input)
{
	char c;
	if (!isChar(input))
	{
		return 0;
	}
	c = (char) input.value;
	return c == LEFT_PARENTHESIS;
}

int isLeftParenthesis(struct Input input)
{
	char c;
	if (!isChar(input))
	{
		return 0;
	}
	c = (char) input.value;
	return c == RIGHT_PARENTHESIS;
}

int isOperand(struct Input input)
{
	return input.type == NUMBER_TYPE;
}

void printInputs(struct Input* inputs, int inputsSize)
{
	int i;
	struct Input curInput;
	for (i = 0; i < inputsSize; i++)
	{
		curInput = inputs[i];
		if (isOperand(curInput))
		{
			printf(" %d ", curInput.value);
		}
		else
		{
			printf("%c", curInput.value);
		}
	}
	printf("\n");
}
