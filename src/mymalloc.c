/*
 * Authors:
 * Samir Moussa ( sm28g11@ecs.soton.ac.uk )
 * Sean Corrigan ( sc2g10@soton.ac.uk )
 *
 * Solution:
 * The solution uses a best fit algorithm.
 * The size of the array initialised is held in the first index of the array.
 * Each block allocated has header pointer which points to the next free or allocated block.
 * If the value of the header is negative, the block is free.
 * If the value of the header is positive, the block is taken.
 *
 * The multi threaded solution uses mutexes to lock threads when others are allocating.
 */

#include "mymalloc.h"
#include <pthread.h>

pthread_mutex_t mymutex;

/*
 * myinit
 */

int myinit(int *array, int size) {

	if (size <= 2)
		return 0;

	array[0] = size;	// save size of the array
	array[1] = 1 - size;	// set first free header
	array[2] = size;

	return 1;
}

int myinit_mt(int *array, int size) {

	if (myinit(array, size) == 0)
		return 0;

	pthread_mutex_init(&mymutex, NULL);

	return 1;
}

/*
 * mymalloc
 */

int *mymalloc(int *array, int size) {

	int *arrayEnd = &array[0] + array[0];	// get end of array
	if (size <= 0 || size > (array[0] - 1))	// check if size fits in array
		return 0;

	int *ptr = &array[1];	// point to the first space in array
	int *best = &array[0];
	size += 1;	// include the header

	while (ptr < arrayEnd) {

		if (*ptr < -1 && -(*ptr) >= size) {
			if(-(*ptr) == size) {
				best = ptr;
				break;
			}
			if (*best > 0 || *ptr >= *best)
				best = ptr;
		}
		//ptr = ((*ptr < 0) ? -(*ptr) : *ptr) + ptr; // this should move the pointer to the next block
		if (*ptr < -1)
			ptr += -(*(ptr + 1));
		else
			ptr += *ptr;
	}

	if (size < -(*best)) {	// if size is smaller than free block

		int *blockPtr = best;
		int previousSize = *best;

		*best = size; // add the block header
		best += size; // move pointer
		*best = previousSize + size; // set next header

		return ++blockPtr;
	}

	if (size == -(*best)) {	// if size is exactly the size of free block
		*best = size;
		return ++best;
	}


	return (int *) 0;
}

int *mymalloc_mt(int *array, int size) {

	pthread_mutex_lock(&mymutex);
	int *ptr = mymalloc(array, size);
	pthread_mutex_unlock(&mymutex);

	return ptr;
}

/*
 * myfree
 */

int myfree(int *array, int *block) {

	int *arrayEnd = &array[0] + array[0];
	--block; // get header

	if (block < array || block > arrayEnd || *block <= 0) {
		return 0;
	}

	*block = -(*block); // free the block

	int *ptr = &array[1];
	int *prevHead = ptr;
	int *prevFree = ptr;

	while (ptr < block) {	// loop until you find the block's previous block
		prevHead = ptr;
		if (*ptr < 0) {
			prevFree = ptr;
			ptr += -(*ptr);
		} else {
			ptr += *ptr;
		}
	}

	if (*prevHead < 0 && prevHead != ptr) // if the previous block is free
		*prevHead += *ptr; // merge with block
	else
		*(prevFree + 1) +=
	ptr += -(*ptr); // move pointer to block after initial block
	if (*ptr < 0) {
		if (*prevHead < 0) // if free, merge
			*prevHead += *ptr;
		else
			*block += *ptr;
	}

	return 1;
}

int myfree_mt(int *array, int *block) {

	pthread_mutex_lock(&mymutex);
	int result = myfree(array, block);
	pthread_mutex_unlock(&mymutex);

	return result;
}

/*
 * mydispose
 */

int mydispose(int *array) { // all free blocks will be merged and so array will be free

	if(array[0] == (1 - array[1])) // check if size is equal to first free block header
		return 1;
	return 0;

}

int mydispose_mt(int *array) {

	pthread_mutex_lock(&mymutex);
	int result = mydispose(array);
	pthread_mutex_unlock(&mymutex);

	return result;
}

