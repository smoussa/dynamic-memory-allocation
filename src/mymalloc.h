
// Function declarations

int myinit(int *array, int size);
int *mymalloc(int *array, int size);
int myfree(int *array, int *block);
int mydispose(int *array);

int myinit_mt(int *array, int size);
int *mymalloc_mt(int *array, int size);
int myfree_mt(int *array, int *block);
int mydispose_mt(int *array);
