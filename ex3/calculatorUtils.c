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

struct Input peekInput(Stack* stack)
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

int isSpace(char c)
{
    return c == ' ';
}

void printInputs(struct Input* inputs, int inputsSize)
{
    int i;
    struct Input curInput;
    for (i = 0; i < inputsSize; i++) {
        curInput = inputs[i];
        if (isOperand(curInput)) {
            printf(" %d ", curInput.value);
        } else {
            printf("%c", curInput.value);
        }
    }
    printf("\n");
}
