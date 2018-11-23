#include <errno.h>
#include "input.h"
#include "calculatorUtils.h"

// TODO Add makefile

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
        case '+':
        case '-':
            return 1;
        case '*':
        case '/':
            return 2;
        case '^':
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
                value += atoi(&c); // This is allowed, since we know it's a digit
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
                value = atoi(&c); // TODO do before if
            }
        }
        else {
            if (middleOfNumber) {
                inputs[inputsSize - 1].value = value;
                middleOfNumber = 0;
                value = 0;
            }
            if (!isSpace(c))
            {
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
    int postfixLocation;
    int i;
    struct Input input;
    struct Input stackData;
    Stack *stack;

    stack = stackAlloc(sizeof(struct Input));
    postfixLocation = 0;
    postfix = malloc(sizeof(struct Input));

    for(i = 0; i < infixSize; i++)
    {
        input = infix[i];
        if (isOperand(input))
        {
            postfix = realloc(postfix, sizeof(struct Input) * postfixLocation + 1);
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
                postfix = realloc(postfix, sizeof(struct Input) * postfixLocation + 1);
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
                while(!isEmptyStack(stack) && !isRightParenthesis(peekInput(stack)) && precedence(peekInput(stack)) > precedence(input))
                {
                    postfix = realloc(postfix, sizeof(struct Input) * postfixLocation + 1);
                    postfix[postfixLocation] = popInput(stack);
                    postfixLocation++;
                }
                pushInput(stack, input);
            }
        }
    }

    while(!isEmptyStack(stack))
    {
        postfix = realloc(postfix, sizeof(struct Input) * postfixLocation + 1);
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
        case '+':
            return b + a;
        case '-':
            return b - a;
        case '*':
            return b * a;
        case '/':
            return b / a;
        case '^':
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
            push(stack, &res);
        }
    }
    pop(stack, &res);
    freeStack(&stack);

    return res;
}


int main(int argc, char *argv[]) {
    char str[101]; // TODO const
    size_t strLen;
    struct Input *inputs;
    struct Input *postfixInputs;
    int inputsSize;
    int postfixInputsSize;
    int i;
    struct Input curInput;

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

        printf("The value is %d\n", calculate(postfixInputs, postfixInputsSize));
        free(inputs);
        free(postfixInputs);
    }
}