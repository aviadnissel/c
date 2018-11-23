#include "stackUtils.h"

struct Input popInput(struct stack* stack)
{
    struct Input input;
    pop(stack, &input);
    return input;
}

void pushInput(struct stack* stack, struct Input input)
{
    push(stack, &input);
}