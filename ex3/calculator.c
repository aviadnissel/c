/**
 * @file calculator.c
 * @author Aviad Nissel <aviad.nissel@mail.huji.ac.il>
 *
 * Calculates a given mathematical expression by converting it to infix,
 * to postfix, and then calculating the result.
 */


/* --- Includes --- */

#include <errno.h>
#include "input.h"
#include "calculatorUtils.h"


/* --- Constants ---*/

/** The maximum input length from user. */
#define MAX_USER_INPUT_LENGTH 101


/* --- Functions --- */

/**
 * Finds the precedence of a given input with an operator.
 *
 * @param input an Input that should contain an operator.
 * @return the precedence (higher comes first), -1 if input is not an operator.
 */
int precedence(struct Input input)
{
	char op;
	if(!isOperator(input))
	{
		return -1;
	}
	op = (char) input.value;
	switch (op)
	{
		case ADD:
		case SUB:
			return 1;
		case MUL:
		case DIV:
			return 2;
		case POW:
			return 3;
	}
	return 0;
}

/**
 * Converts the given string to an array of Inputs, in infix order.
 *
 * @param str the string to convert.
 * @param inputsPtr a pointer to the returned array of Inputs.
 * @return The length of the converted Inputs, or 0 if none were converted.
 */
int stringToInfix(const char *str, struct Input **inputsPtr)
{
	size_t strLen;
	struct Input* inputs;
	struct Input* allocatedInputs;
	size_t i;
	int inputsSize;
	char c;
	char *endPtr;
	int value;
	int middleOfNumber;

	inputsSize = 0;
	middleOfNumber = 0;
	value = 0;

	strLen = strlen(str);

	inputs = malloc(sizeof(struct Input));

	if(!inputs)
	{
		return -ENOMEM;
	}

	for(i = 0; i < strLen; i++)
	{

		c = str[i];
		if (isdigit(c))
		{
			if (middleOfNumber)
			{
				value *= 10;
			}
			else
			{
				middleOfNumber = 1;
				inputsSize++;
				allocatedInputs = realloc(inputs, sizeof(struct Input) * inputsSize);
				if(!allocatedInputs)
				{
					return -ENOMEM;
				}
				inputs = allocatedInputs;
				inputs[inputsSize - 1].type = NUMBER_TYPE;
				value = 0;
			}
			value += strtol(&c, &endPtr, 10);
			// No need to check endPtr since we know it's a digit
		}
		else
		{
			if (middleOfNumber)
			{
				inputs[inputsSize - 1].value = value;
				middleOfNumber = 0;
				value = 0;
			}
			inputsSize++;
			allocatedInputs = realloc(inputs, sizeof(struct Input) * inputsSize);
			if(!allocatedInputs)
			{
				return -ENOMEM;
			}
			inputs = allocatedInputs;
			inputs[inputsSize - 1].type = CHAR_TYPE;
			inputs[inputsSize - 1].value = c;
		}

	}

	if(middleOfNumber)
	{
		inputs[inputsSize - 1].value = value;
	}

	if (inputsSize == 0)
	{
		// No input was found
		free(inputs);
		inputs = NULL;
	}
	*inputsPtr = inputs;
	return inputsSize;
}

/**
 * Handles the postfix conversion in the case of an operand.
 *
 * @param input the current Input.
 * @param postfixPtr a pointer to the postfix Input list, to which the new Inputs will be added.
 * @param postfixIndex the current postfix index.
 * @return the new postfix index.
 */
int handlePostfixOperand(struct Input input, struct Input **postfixPtr, int postfixIndex)
{
    struct Input *postfix = *postfixPtr;
    struct Input *allocatedPostfix;
    allocatedPostfix = realloc(postfix, sizeof(struct Input) * (postfixIndex + 1));
    if(!allocatedPostfix)
    {
        return -ENOMEM;
    }
    postfix = allocatedPostfix;
    postfix[postfixIndex] = input;
    postfixIndex++;
    *postfixPtr = postfix;
    return postfixIndex;
}

/**
 * Handles the postfix conversion in the case of left parenthesis.
 *
 * @param input the current Input.
 * @param postfixPtr a pointer to the postfix Input list, to which the new Inputs will be added.
 * @param postfixIndex the current postfix index.
 * @return the new postfix index.
 */
int handlePostfixLeftParenthesis(struct Stack* stack, struct Input **postfixPtr, int postfixIndex)
{
    struct Input *postfix = *postfixPtr;
    struct Input *allocatedPostfix;
    struct Input stackData;
    while(!isEmptyStack(stack) && !isRightParenthesis(stackData = popInput(stack)))
    {
        allocatedPostfix = realloc(postfix, sizeof(struct Input) * (postfixIndex + 1));
        if(!allocatedPostfix)
        {
            return -ENOMEM;
        }
        postfix = allocatedPostfix;
        postfix[postfixIndex] = stackData;
        postfixIndex++;
    }
    *postfixPtr = postfix;
    return postfixIndex;
}

/**
 * Handles the postfix conversion in the case of an operator.
 *
 * @param input the current Input.
 * @param postfixPtr a pointer to the postfix Input list, to which the new Inputs will be added.
 * @param postfixIndex the current postfix index.
 * @return the new postfix index.
 */
int handlePostfixOperator(struct Stack *stack, struct Input **postfixPtr,
                          struct Input input, int postfixIndex)
{
    struct Input *allocatedPostfix;
    struct Input *postfix = *postfixPtr;

    if(isEmptyStack(stack) || isRightParenthesis(peekInput(stack)))
    {
        pushInput(stack, input);
    }
    else
    {
        while(!isEmptyStack(stack) && !isRightParenthesis(peekInput(stack)) &&
              precedence(peekInput(stack)) >= precedence(input))
        {
            allocatedPostfix = realloc(postfix, sizeof(struct Input) * (postfixIndex + 1));
            if (!allocatedPostfix)
            {
                return -ENOMEM;
            }
            postfix = allocatedPostfix;
            postfix[postfixIndex] = popInput(stack);
            postfixIndex++;
        }
        pushInput(stack, input);
    }
    *postfixPtr = postfix;
    return postfixIndex;
}

/**
 * Converts given Inputs in infix order to a postfix order.
 *
 * @param infix the infix Inputs.
 * @param infixSize the size of the infix Inputs array.
 * @param postfixPtr a pointer to the returned array of inputs.
 * @return the length  of the postfix array size, or 0 if none were converted.
 */
int infixToPostfix(struct Input* infix, int infixSize, struct Input** postfixPtr)
{
	struct Input* postfix;
	struct Input* allocatedPostfix;
	int postfixIndex;
	int i;
	struct Input input;
	Stack *stack;

	stack = stackAlloc(sizeof(struct Input));
	postfixIndex = 0;
	postfix = malloc(sizeof(struct Input));

	if(!postfix)
	{
		return -ENOMEM;
	}

	for(i = 0; i < infixSize; i++)
	{
		input = infix[i];
		if (isOperand(input))
		{
            postfixIndex = handlePostfixOperand(input, &postfix, postfixIndex);
            if (postfixIndex < 0)
            {
                return postfixIndex;
            }
		}
		if(isRightParenthesis(input))
		{
			pushInput(stack, input);
		}
		if(isLeftParenthesis(input))
		{
		    postfixIndex = handlePostfixLeftParenthesis(stack, &postfix, postfixIndex);
            if (postfixIndex < 0)
            {
                return postfixIndex;
            }
		}
		if(isOperator(input))
		{
		    postfixIndex = handlePostfixOperator(stack, &postfix, input, postfixIndex);
		    if (postfixIndex < 0)
            {
		        return postfixIndex;
            }
		}
	}

	while(!isEmptyStack(stack))
	{
		allocatedPostfix = realloc(postfix, sizeof(struct Input) * (postfixIndex + 1));
		if(!allocatedPostfix)
		{
			return -ENOMEM;
		}
		postfix = allocatedPostfix;
		postfix[postfixIndex] = popInput(stack);
		postfixIndex++;
	}

	if (postfixIndex == 0)
	{
		free(postfix);
	}

	freeStack(&stack);
	*postfixPtr = postfix;
	return postfixIndex;
}

/**
 * Evaluates the given a operator b.
 *
 * @param a the first number.
 * @param b the second number.
 * @param operator the operator to use with a and b. Can be /+-*^.
 * @return The result.
 * 		   In case of division by zero, return 0 and sets errno to EINVAL.
 */
int evaluate(int a, int b, char operator)
{
	switch (operator)
	{
		case ADD:
			return b + a;
		case SUB:
			return b - a;
		case MUL:
			return b * a;
		case DIV:
			if (a == 0)
			{
				errno = EINVAL;
				return 0;
			}
			return b / a;
		case POW:
			return (int) pow(b, a);
		default:
			return 0;
	}
}

/**
 * Calculates the given Inputs with postfix order.
 *
 * @param postfix the Inputs array.
 * @param postfixSize The Inputs array's size.
 * @return the calculation result.
 * 		   In case of division by zero, return 0 and sets errno to EINVAL.
 */
int calculate(struct Input* postfix, int postfixSize)
{
	int i;
	struct Input input;
	int val;
	Stack* stack;
	int a, b;
	int res;

	stack = stackAlloc(sizeof(int));

	for(i = 0; i < postfixSize; i++)
	{
		input = postfix[i];
		if(isOperand(input))
		{
			val = input.value;
			push(stack, &val);
		}
		if(isOperator(input))
		{
			pop(stack, &a);
			pop(stack, &b);
			res = evaluate(a, b, (char) input.value);
			if (errno == EINVAL)
			{
				freeStack(&stack);
				return 0;
			}
			push(stack, &res);
		}
	}
	pop(stack, &res);
	freeStack(&stack);

	return res;
}


/* --- Main --- */

/**
 * Main function.
 *
 * @return 0 on success, else otherwise.
 */
int main()
{
	char str[MAX_USER_INPUT_LENGTH];
	struct Input *inputs;
	struct Input *postfixInputs;
	int inputsSize;
	int postfixInputsSize;
	int calculatedValue;

	while (scanf("%s", str) != EOF)
	{
		inputsSize = stringToInfix(str, &inputs);

		if(inputsSize < 0)
		{
			fprintf(stderr, "Error while converting string: %d\n", -inputsSize);
			free(inputs);
			free(postfixInputs);
			exit(1);
		}
		printf("Infix: ");
		printInputs(inputs, inputsSize);

		postfixInputsSize = infixToPostfix(inputs, inputsSize, &postfixInputs);

		if(postfixInputsSize < 0)
		{
			fprintf(stderr, "Error while converting infix to postfix: %d\n", -postfixInputsSize);
			free(inputs);
			free(postfixInputs);
			exit(1);
		}
		printf("Postfix: ");
		printInputs(postfixInputs, postfixInputsSize);

		calculatedValue = calculate(postfixInputs, postfixInputsSize);
		if (errno == EINVAL)
		{
			fprintf(stderr, "Error: Division by zero\n");
			free(inputs);
			free(postfixInputs);
			return 1;
		}
		else
		{
			printf("The value is %d\n", calculatedValue);
		}
		free(inputs);
		free(postfixInputs);
	}
}
