#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include "stack.h"

// TODO Add makefile

#define NUMBER_TYPE 0
#define CHAR_TYPE 1

struct Input // TODO rename
{
    int type;
    int value;
};

int stringToInputs(char* str, size_t strLen, struct Input** inputsPtr)
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

int main(int argc, char *argv[])
{
    char *str = "7*(2+6)-5/3";
    size_t strLen = strlen(str);
    struct Input *inputs;
    int inputsSize;
    int i;
    struct Input curInput;

    inputsSize = stringToInputs(str, strLen, &inputs);
    for(i = 0; i < inputsSize; i++)
    {
        curInput = inputs[i];
        if (curInput.type == NUMBER_TYPE)
        {
            printf("%d", curInput.value);
        }
        else
        {
            printf("%c", curInput.value);
        }
    }
    printf("\n");

}