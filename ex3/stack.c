#include "stack.h"

#include <string.h>
#include <stdio.h>
#include <assert.h>

Stack* stackAlloc(size_t elementSize)
{
  Stack* stack = (Stack*)malloc(sizeof(Stack));
  stack->_top = NULL;
  stack->_elementSize = elementSize;
  return stack;
}

void freeStack(Stack** stack)
{
  Node* p1;
  Node* p2;
  if (!(*stack == NULL))
    {
      p1= (*stack)->_top;
      while(p1)
	{
	  p2= p1;
	  p1= p1->_next;
	  free(p2->_data);
	  free(p2);
	}
      free(*stack);
      *stack = NULL;
    }
}

void push(Stack* stack, void *data)
{
  //you should check allocation success
  Node* node = (Node*)malloc(sizeof(Node));
  node->_data = malloc(stack->_elementSize);
  memcpy(node->_data, data, stack->_elementSize);
  node->_next = stack->_top;
  stack->_top = node;
}

void pop(Stack* stack, void *headData) 
{
  assert(stack != NULL);
  if(stack->_top == NULL)
    {
      fprintf(stderr, "The stack is empty\n");
      return;
    }

  Node *node = stack->_top;
  memcpy(headData, node->_data,stack->_elementSize);
  stack->_top = node->_next;  
  free(node->_data);
  free(node);
}

int isEmptyStack(Stack* stack) 
{
  assert(stack != NULL); 
  return stack->_top == NULL; 
}
