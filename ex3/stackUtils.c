#include "stackUtils.h"

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