
#include "input.h"

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
    size_t i;
    int inputsSize;
    char c;
    int value;
    int middleOfNumber;

    inputsSize = 0;
    middleOfNumber = 0;
    value = 0;

    inputs = malloc(sizeof(struct Input));

    // TODO check allocations

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
                inputs = realloc(inputs, sizeof(struct Input) * inputsSize);
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
                inputs = realloc(inputs, sizeof(struct Input) * inputsSize);
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
        postfixLocation++;
    }

    freeStack(&stack);
    *postfixPtr = postfix;
    return postfixLocation - 1;
}

int main(int argc, char *argv[]) {
    char str[101];
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

        printf("Infix: ");
        printInputs(inputs, inputsSize + 1);

        postfixInputsSize = infixToPostfix(inputs, inputsSize, &postfixInputs);

        printf("Postfix: ");
        printInputs(postfixInputs, postfixInputsSize + 1);

        free(inputs);
    }
}