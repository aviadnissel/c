
#include "Calculator.h"

// TODO Add makefile

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
    return c == '^' || c == '+' || c == '-' || c == '*' || c == '/'; // TODO consts
}

int isRightParenthesis(struct Input input)
{
    char c;
    if (!isChar(input))
    {
        return 0;
    }
    c = (char) input.value;
    return c == '(';
}

int isLeftParenthesis(struct Input input)
{
    char c;
    if (!isChar(input))
    {
        return 0;
    }
    c = (char) input.value;
    return c == ')';
}

int isOperand(struct Input input)
{
    return input.type == NUMBER_TYPE;
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
        else
        {
            if(middleOfNumber)
            {
                inputs[inputsSize - 1].value = value;
                middleOfNumber = 0;
                value = 0;
            }
            inputsSize++;
            inputs = realloc(inputs, sizeof(struct Input) * inputsSize);
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
    struct Input* postfix = *postfixPtr;
    int postfixLocation;
    int i;
    struct Input input;
    struct Input stackData;
    Stack *stack;

    stack = stackAlloc(sizeof(struct Input));
    postfixLocation = 0;

    for(i = 0; i < infixSize; i++)
    {
        input = infix[i];
        if (isOperand(input))
        {
            postfix[postfixLocation] = input;
            postfixLocation++;
        }
        if(isRightParenthesis(input)) // TODO func?
        {
            pushInput(stack, input);
        }
        if(isLeftParenthesis(input))
        {
            while(!isEmptyStack(stack) && (stackData = pop(stack)) != '(')
            {
                postfix[postfixLocation] = stackData;
                postfixLocation++;
            }
        }
        if(isOperator(c))
        {
            if(isEmptyStack(stack) || peekChar(stack) == '(')
            {
                pushChar(stack, c);
            }
            else
            {
                while(!isEmptyStack(stack) && peekChar(stack) != '(' && precedence(peekChar(stack)) > precedence(c))
                {
                    postfix[postfixLocation] = popChar(stack);
                    postfixLocation++;
                }
                pushChar(stack, c);
            }
        }
    }

    while(!isEmptyStack(stack))
    {
        postfix[postfixLocation] = popChar(stack);
        postfixLocation++;
    }

    freeStack(&stack);
    return postfixLocation - 1;
}

int main(int argc, char *argv[])
{
    char *str = "7*(2+6)-54/3";
    size_t strLen = strlen(str);
    struct Input *inputs;
    int inputsSize;
    int i;
    struct Input curInput;

    inputsSize = stringToInputs(str, strLen, &inputs);


}