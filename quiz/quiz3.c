#include<stdio.h>
#include<stdlib.h>
#define MAXROW 5
#define MAXCOL 24

int main()

{

    int (*p)[MAXCOL];

    p = (int (*) [MAXCOL])malloc(MAXROW *sizeof(*p));

    return 0;

}
