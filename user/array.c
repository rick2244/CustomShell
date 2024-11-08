#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

//
// wrapper so that it's OK if main() does not call exit().
//


/*
*reverses the values of an array of any type in java
*/
void
reverseArray(int* arr, uint64 size){
	int start = 0;
	int end = size - 1;

	//switches the values at opposite ends until indexes start and end meet
	while(start < end){
		int temp = arr[start];
		arr[start++] = arr[end];
		arr[end--] = temp;
	}	
}

/*
*helper method used to swap elements
*/
void
swap(int *arr, uint64 a, uint64 b){
	int temp = arr[a];
	arr[a] = arr[b];
	arr[b] = temp;
}

/*
*splits the integer array in subarrays that will be sorted
*/
uint64
partition(int *arr, uint64 low, uint64 high){
	int pivot = arr[high];// makes the last element the pivot
	uint64 i = low - 1; //index of the smaller element

	for(uint64 j = low; j < high; j++){
		//if current element is smaller than or equal to pivot
		//it goes on the left side of the sub array
		if(arr[j] <= pivot){
			i++;
			//swap is performed between the indexs
			swap(arr, j, i);
		}
	}
	//pivot is in it's correct position
	swap(arr, i + 1, high);
	return i + 1; //returns the partition index
}

/*
*recursively sorts the elements using quick sort algorithm
*/
void
sort(int* arr, uint64 low, uint64 high){
	if(low < high){
		uint64 pt_index = partition(arr, low, high);

		sort(arr, low, pt_index - 1);
		sort(arr, pt_index + 1, high);
	}
	
}

/*
*gets the max value of an integer array
*/
int
getMax(int* arr, uint size){
	int max = -2147483648;

	//loops through whole array and find the max
	for(uint64 i = 0; i < size - 1; i++){
		if(arr[i] > max){
			max = arr[i];
		}
	}
	
	return max;
}

/*
*gets the smallest intefer value inside an array
*/
int
getMin(int* arr, uint64 size){
	int min = 2147483647;

	//loops through the whole array and finds min
	for(uint64 i = 0; i < size - 1; i++){
		if(arr[i] <  min){
			min = arr[i];
		}
	}
	return min;	
}
