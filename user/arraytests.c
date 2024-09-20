#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"
#include "user/array.h"

int
main(void){
	int arr[10] = {5, -4, 2, 22, 12, -56, 3, 8, 34, 2};

	uint64 size = sizeof(arr) / sizeof(arr[0]);
	printf("size: %d\n", size);

	printf("Array before: ");
	for(int  i = 0; i < size; i++){
		printf("%d ", arr[i]);
	}

	printf("\n");

	reverseArray(arr, size);

	printf("Array in reverse: ");
	
	for(int i = 0; i < size ; i++){
		printf("%d ", arr[i]);
	}

	printf("\n");

	uint64 min = getMin(arr, size);
	printf("Get min value: ");
	printf("%d \n", min);

	uint64 max = getMax(arr, size);
	printf("Get max value: ");
	printf("%d \n", max);

	sort(arr, 0, size - 1);
	
	printf("sorted array: ");
	for(int i = 0; i < size; i++){
		printf("%d ", arr[i]);
	}


	
	return 0;
}

