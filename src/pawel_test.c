
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include "mymalloc.h"

//#define SIZE 65536
#define SIZE 999999
#define OPS 200
#define SEED 42
#define PSIZE (SIZE/50)

static int block[SIZE];
static int sizes[PSIZE];
static int * pointers[PSIZE];
static int valid_pointers=0;
static int count = 0;

static int sizes[PSIZE];
static int valid_sizes=0;
static int freeSpace = SIZE;

int main() {
	int i;
	clock_t start = clock();

	srand(SEED);
	while (1) {
		if (valid_sizes == 0 || ((valid_sizes < PSIZE) && ((rand() & 7) < 5)))
		{
			int size = rand() % (1 << (rand() % 12)) + 1;

			if (freeSpace >= size) {
					sizes[valid_sizes] = size;
					valid_sizes++;
					count++;
					freeSpace -= size;
			} else {
					printf("Possible mymallocs = %d ( overhead: %fs )\n",
							count,
							((double) clock() - start) / CLOCKS_PER_SEC);
					break;
			}

		} else {
			/* free a block  */
			int fp = rand() % valid_sizes;
			freeSpace += sizes[fp];
			while (++fp < valid_sizes)
				sizes[fp - 1] = sizes[fp];
			valid_sizes--;
		}
	}

	int max = count;
	count = 0;

	start = clock();

	srand(SEED);
	if (myinit(block, SIZE) == 0)
		exit(EXIT_FAILURE);

	while (1) {
		if (valid_pointers == 0 || ((valid_pointers < PSIZE) && ((rand() & 7) < 5))) {

			int size = rand() % (1 << (rand() % 12)) + 1;

			int * fp = mymalloc(block, size);

			if (fp != (int*) 0) {
				pointers[valid_pointers] = fp;
				sizes[valid_pointers] = size;
				valid_pointers++;
				count++;
				for (i = 0; i < size; i++) fp[i] = -(i + 42);
			} else {
				printf("Successful mymallocs = %d ( %f%%, valid pointers: %d ) in %fs\n",
						count,
						100. * count / max,
						valid_pointers,
						((double) clock() - start) / CLOCKS_PER_SEC);

				exit(EXIT_SUCCESS);
			}
		} else {
			/* free a block  */
				int fp = rand() % valid_pointers;

				for (i = 0; i < sizes[fp]; i++) {
					if (pointers[fp][i] != -(i + 42)) {
						printf("!!!! Heap corrupted !!!!\n");
						exit(EXIT_FAILURE);
					}
				}

				if (myfree(block, pointers[fp]) == 0)
					exit(EXIT_FAILURE);

				while (++fp < valid_pointers) {
					pointers[fp - 1] = pointers[fp];
					sizes[fp - 1] = sizes[fp];
				}
				valid_pointers--;
		}
	}
}
