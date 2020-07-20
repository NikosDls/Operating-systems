#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/stat.h>

int main(int argc, char** argv){
	int i;
	int *shared_memory;
	int row, column, l, m, n, segment_id, sum;		

//	printf("HI!\n");
	
	row = atoi(argv[1]);
	column = atoi(argv[2]);
	l = atoi(argv[3]);
	m = atoi(argv[4]);
	n = atoi(argv[5]);
	segment_id = atoi(argv[6]);
//	printf("%d %d %d %d %d %d", row, column, l, m, n, segment_id);
	
	shared_memory = (int*)shmat(segment_id, NULL, 0);
/*
	printf("----------\nShare memory(%d):\n\n", segment_id);
	for(i = 0; i < (l * m) + (m * n); i++){
		printf("%d ", shared_memory[i]);	
	}
	printf("\n----------\n");	
*/
	shmctl(segment_id, SHM_LOCK, NULL);

	sum = 0;
	for(i = 0; i < m; i++){
		sum += shared_memory[(m * row) + i] * shared_memory[(n * i) + column + (l * m)];	// sum += A[row][i] * B[i][column];
		//printf("%d %d = %d\t", shared_memory[(m * row) + i], shared_memory[(n * i) + column], sum);
		//printf("shared_memory[%d] = %d\n", (m * row) + i, shared_memory[(m * row) + i]);
	}
//printf("\n");
//	printf("%d %d", row, column);
	shared_memory[(n * row) + column + (l * m) + (m * n)] = sum;	// c[row][column] = sum;
//	printf("shared_memory[%d] = %d\n", (n * row) + column + (l * m) + (m * n), sum);
	shmctl(segment_id, SHM_UNLOCK, NULL);

	shmdt(shared_memory);		

return 0;
}
