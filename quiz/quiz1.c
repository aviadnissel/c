#include <stdio.h>

float average(int arr[], int size) 
{    
	int sum = 0;
    int i;
    if (size == 0) {
        return 0;
    }

    for(i = 0; i < size; i++) {
        sum += arr[i];
	printf("%d\n", sum);
    }
    return sum / size;
}


int main()
{

	int arr[] = { 4, 2, 4, 2, 3 };
	printf("%.3f\n", average(arr, 5)); 
return 0;

}    
