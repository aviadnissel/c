#include <stdlib.h>
#include <stdio.h>
#include "stack.h"
#include <string.h>
#include <math.h>
// TODO organize includes

#define NUMBER_TYPE 0
#define CHAR_TYPE 1

struct Thing // TODO rename
{
    int type;
    void* value;
};
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
            return pow(b, a);
        default:
            return 0;
    }
}
char peekChar(Stack* stack)
{
    char headData;
    pop(stack, &headData);
    push(stack, &headData);
    return headData;
}

char popChar(Stack* stack)
{
    char headData;
    pop(stack, &headData);
    return headData;
}

void pushChar(Stack* stack, char c)
{
    push(stack, &c);
}

int precedence(char op)
{
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

int isOperator(char c)
{
    return c == '^' || c == '+' || c == '-' || c == '*' || c == '/'; // TODO consts
}

int isParenthesis(char c)
{
    return c == '(' || c == ')';
}

int isOperand(char c)
{
    return !(isOperator(c) || isParenthesis(c));
}

int infixToPostfix(char* infix, int infixSize, char** postfixPtr)
{
    char* postfix = *postfixPtr;
    int postfixLocation;
    int i;
    char c;
    char stackData;
    Stack *stack;

    stack = stackAlloc(sizeof(char));
    postfixLocation = 0;

    for(i = 0; i < infixSize; i++)
    {
        c = infix[i];
        if (isOperand(c))
        {
            postfix[postfixLocation] = c;
            postfixLocation++;
        }
        if(c == '(') // TODO func?
        {
            push(stack, &c);
        }
        if(c == ')')
        {
            while(!isEmptyStack(stack) && (stackData = popChar(stack)) != '(')
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

int calculate(char* postfix, int postfixSize)
{
    int i;
    char c;
    int val;
    Stack* stack;
    int a, b;
    int res;

    stack = stackAlloc(sizeof(int));

    for(i = 0; i < postfixSize; i++)
    {
        c = postfix[i];
        if(isOperand(c))
        {
            val = c - '0';
            push(stack, &val);
        }
        if(isOperator(c))
        {
            pop(stack, &a);
            pop(stack, &b);
            res = evaluate(a, b, c);
            push(stack, &res);
        }
    }

    freeStack(&stack);

    pop(stack, &res);
    return res;
}

int main()
{
    // TODO chars is not good...
    char *infix = "7*(2+6)-5/3";

    int infixLength = strlen(infix);
    int postfixLength;
    char *postfix = malloc(infixLength + 1);
    postfixLength = infixToPostfix(infix, infixLength, &postfix);
    postfix[postfixLength + 1] = '\0';
    printf("%s\n", postfix);
    printf("%d\n", calculate(postfix, postfixLength));

    free(postfix);
    return 0;
}