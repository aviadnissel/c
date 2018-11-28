#include <errno.h>
#include "input.h"
#include "calculatorUtils.h"

#define MAX_INPUT_LENGTH 101

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

int stringToInputs(const char* str, size_t strLen, struct Input** inputsPtr)
{
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
		else {
			if (middleOfNumber) {
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

int infixToPostfix(struct Input* infix, int infixSize, struct Input** postfixPtr)
{
	struct Input* postfix;
	struct Input* allocatedPostfix;
	int postfixLocation;
	int i;
	struct Input input;
	struct Input stackData;
	Stack *stack;

	stack = stackAlloc(sizeof(struct Input));
	postfixLocation = 0;
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
			allocatedPostfix = realloc(postfix, sizeof(struct Input) * (postfixLocation + 1));
			if(!allocatedPostfix)
			{
				return -ENOMEM;
			}
			postfix = allocatedPostfix;
			postfix[postfixLocation] = input;
			postfixLocation++;
		}
		if(isRightParenthesis(input))
		{
			pushInput(stack, input);
		}
		if(isLeftParenthesis(input))
		{
			while(!isEmptyStack(stack) && !isRightParenthesis(stackData = popInput(stack)))
			{
				allocatedPostfix = realloc(postfix, sizeof(struct Input) * (postfixLocation + 1));
				if(!allocatedPostfix)
				{
					return -ENOMEM;
				}
				postfix = allocatedPostfix;
				postfix[postfixLocation] = stackData;
				postfixLocation++;
			}
		}
		if(isOperator(input))
		{
			if(isEmptyStack(stack) || isRightParenthesis(peekInput(stack)))
			{
				pushInput(stack, input);
			}
			else
			{
				while(!isEmptyStack(stack) && !isRightParenthesis(peekInput(stack)) && precedence(peekInput(stack)) >= precedence(input))
				{
					allocatedPostfix = realloc(postfix, sizeof(struct Input) * (postfixLocation + 1));
					if(!allocatedPostfix)
					{
						return -ENOMEM;
					}
					postfix = allocatedPostfix;
					postfix[postfixLocation] = popInput(stack);
					postfixLocation++;
				}
				pushInput(stack, input);
			}
		}
	}

	while(!isEmptyStack(stack))
	{
		allocatedPostfix = realloc(postfix, sizeof(struct Input) * (postfixLocation + 1));
		if(!allocatedPostfix)
		{
			return -ENOMEM;
		}
		postfix = allocatedPostfix;
		postfix[postfixLocation] = popInput(stack);
		postfixLocation++;
	}

	if (postfixLocation == 0)
	{
		free(postfix);
	}

	freeStack(&stack);
	*postfixPtr = postfix;
	return postfixLocation;
}

int evaluate(int a, int b, char operator)
{
	switch (operator) {
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


int main(int argc, char *argv[]) {
	char str[MAX_INPUT_LENGTH];
	size_t strLen;
	struct Input *inputs;
	struct Input *postfixInputs;
	int inputsSize;
	int postfixInputsSize;
	int calculatedValue;

	while (scanf("%s", str) != EOF)
	{
		strLen = strlen(str);
		inputsSize = stringToInputs(str, strLen, &inputs);

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
		}
		else
		{
			printf("The value is %d\n", calculatedValue);
		}
		free(inputs);
		free(postfixInputs);
	}
}
