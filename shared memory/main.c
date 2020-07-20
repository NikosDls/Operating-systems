/* Ergastiriaki Askisi 2
 *
 * Kallitsopoulou Styliani	A.M.: 2113121
 * Dollas Nikolaos		A.M.: 2113007
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/stat.h>

#define MAX_COLUMNS 50
#define MAX_ROWS 50
#define INT_DIGITS 19		/* enough for 64 bit integer */

	int readArraysFromFile(FILE *, char [], int [][MAX_COLUMNS], int [][MAX_COLUMNS], int *, int *, int *, int *);
	void writeArray(int [][MAX_COLUMNS], int, int);
	char* itoa(int);

int main (int argc, char **argv){
	FILE *fp;
	int i, j;
	int l = 1, m1 = 1, m2 = 1, n = 1;
	int A[MAX_ROWS][MAX_COLUMNS];
	int B[MAX_ROWS][MAX_COLUMNS];
	int retValue;
	int segment_id;
	int *shared_memory;
	pid_t pid;
	char row[2], column[2], lC[2], mC[2], nC[2], id[7], *temp;	
	char currentDirectory[128];

	if(argc != 2){
		printf("\n./[executable name] [file name]\n\n");
		return 1;
	}

	retValue = readArraysFromFile(fp, argv[1], A, B, &l, &m1, &m2, &n);
	if(retValue == 1){
		printf("\nWrong file name!\n\n");
		return 1;	
	}				
	
	if(m1 != m2){
		printf("\nTables must be Alxm and Bmxn!\n\n");
		return 1;
	}

	printf("----------\nArray A:");
	writeArray(A, l, m1);
	printf("----------\n\n");

	printf("----------\nArray B:");
	writeArray(B, m2, n);
	printf("----------\n\n");

	int C[l][n];

	segment_id = shmget(IPC_PRIVATE, (l * m1) + (m2 * n) + (l * n), S_IRUSR | S_IWUSR);
	shared_memory = (int*)shmat(segment_id, NULL, 0);

	for(i = 0; i < l; i++){
 		for(j = 0; j < m1; j++){
			shared_memory[(m1 * i) + j] = A[i][j]; 
//printf("%d ", (m1 * i) + j);
//printf("A: %d SM:%d\t",A[i][j], shared_memory[(m1 * i) + j]);
		}
//		printf("\n");	
	}

//	printf("\n");

	for(i = 0; i < m2; i++){
 		for(j = 0; j < n; j++){
			shared_memory[(n * i) + j + (l * m1)] = B[i][j]; 
//printf("%d ", (n * i) + j + (l * m1));
//printf("B: %d SM:%d\t",B[i][j], shared_memory[(n * i) + j + (l * m1)]);
		}
//		printf("\n");	
	}	
	
	printf("----------\nShare memory(%d):\n\n", segment_id);
	for(i = 0; i < (l * m1) + (m2 * n); i++){
		printf("%d ", shared_memory[i]);	
	}
	printf("\n----------\n");	
	//return 1;
	
	for(i = 0; i < l; i++){
		for(j = 0; j < n; j++){
			pid = fork();
			if(pid >= 0){	// fork was successful
				if(pid == 0){	// child

					temp = itoa(i);
					strcpy(row, temp);
//printf("%s\n", row);
					temp = itoa(j);
					strcpy(column, temp);
//printf("%s\n", column);
					temp = itoa(l);					
					strcpy(lC, temp);
//printf("%s\n", lC);
					temp = itoa(m1);	// temp = itoa(m2); its the same because m1 = m2
					strcpy(mC, temp);				
//printf("%s\n", mC);
					temp = itoa(n);	
					strcpy(nC, temp);
//printf("%s\n", nC);
					temp = itoa(segment_id);
					strcpy(id, temp);
//printf("%s\n", id);
					getcwd(currentDirectory, 128);
					strcat(currentDirectory, "/compute");
//printf("%s\n", currentDirectory);
					execl(currentDirectory, "compute", row, column, lC, mC, nC, id, NULL);
		//printf("%d", retValue);
				exit(0);
				}
				else{	// parent			
//					if(waitpid(pid, &status, 0) != pid){	// parent wating for childs
//					}
					wait(0);
				}	
			}	
		}	
	}
	
	printf("\n\n----------\nArray C:\n\n");
	for(i = 0; i < l; i++){
		for(j = 0; j < n; j++){
			C[i][j] = shared_memory[(i * n) + j + (l * m1) + (m2 * n)];
			//printf("%d ", (i * n) + j + (l * m1) + (m2 * n));
			printf("%d\t", C[i][j]);		
		}
		printf("\n");
	}
	printf("----------\n\n");	
	
	shmdt(shared_memory);
	
	shmctl(segment_id, IPC_RMID, NULL);
	
return 0;
}

int readArraysFromFile(FILE* fp, char fileName[64], int array [][MAX_COLUMNS], int array2 [][MAX_COLUMNS], int *l, int *m1, int *m2, int *n){
	int i, j;
	int temp[4];

	fp = fopen(fileName, "r");

	if(fp == NULL){
		return 1;	
	}		
		
	fscanf(fp, "%d %d", &temp[0], &temp[1]);
//printf("%d %d", temp[0], temp[1]);
	for(i = 0; i < temp[0]; i++){
 		for(j = 0; j < temp[1]; j++){
			fscanf(fp, "%d", &array[i][j]);
//			printf("%d ", array[i][j]);
		}
//	printf("\n");
	}
	
	fscanf(fp, "%d %d", &temp[2], &temp[3]);
//printf("%d %d", temp[2], temp[3]);
	for(i = 0; i < temp[2]; i++){
 		for(j = 0; j < temp[3]; j++){
			fscanf(fp, "%d", &array2[i][j]);
//			printf("%d ", array2[i][j]);
		}
//	printf("\n");
	}

	fclose(fp);
	*l = temp[0];
	*m1 = temp[1];
	*m2 = temp[2];
	*n = temp[3];

return 0;
}

void writeArray(int array [][MAX_COLUMNS], int rows, int columns){
	int i, j;

	printf("\n\n");
	for(i = 0; i < rows; i++){
 		for(j = 0; j < columns; j++){
			printf("%d\t", array[i][j]);
		}
		printf("\n");
	}

return;
}

char *itoa(int i){
  /* Room for INT_DIGITS digits, - and '\0' */
  static char buf[INT_DIGITS + 2];
  char *p = buf + INT_DIGITS + 1;	/* points to terminating '\0' */
  if (i >= 0) {
    do {
      *--p = '0' + (i % 10);
      i /= 10;
    } while (i != 0);
    return p;
  }
  else {			/* i < 0 */
    do {
      *--p = '0' - (i % 10);
      i /= 10;
    } while (i != 0);
    *--p = '-';
  }
  return p;
}
