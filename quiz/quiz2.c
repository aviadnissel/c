#include <stdio.h>
#include <stdlib.h>

int main()
{
    int len;
    char* str;
    int i;
    scanf("%d", &len);
    str = malloc(sizeof(char) * len);
    scanf("%s", str);
    for(i = 0; i < len / 2; i++)
    {
        if (str[i] != str[len - i - 1])
        {
            return 0;
        }
    }
    printf("%s\n", str);
}   
