#include <stdio.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


  int i, j, k, sum = 0;
  int MatrixA[4][3], MatrixB[3][6], MatrixHasil[4][6];

int main()
{

    key_t key = 1234;
  	int (*value)[6];

  	int shmid = shmget(key, sizeof(int[4][6]), IPC_CREAT | 0666);
  	value = shmat(shmid, NULL, 0);


  printf("Enter the elements of MatrixA\n");

  for (i = 0 ; i < 4 ; i++ )
    for ( j = 0 ; j < 3 ; j++ )
      scanf("%d", &MatrixA[i][j]);



    printf("Enter the elements of MatrixB\n");

    for ( i = 0 ; i < 3 ; i++ )
      for ( j = 0 ; j < 6 ; j++ )
        scanf("%d", &MatrixB[i][j]);



    for ( i = 0 ; i < 4 ; i++ )
    {
      for ( j = 0 ; j < 6 ; j++ )
      {
        for ( k = 0 ; k < 3 ; k++ )
        {
          sum = sum + MatrixA[i][k]*MatrixB[k][j];
        }

        MatrixHasil[i][j] = sum;
        sum = 0;
      }
    }

    printf("Product of entered matrices:-\n");

    for ( i = 0 ; i < 4 ; i++ )
    {
      for ( j = 0 ; j < 6 ; j++ )
        printf("%d\t", MatrixHasil[i][j]);

      printf("\n");
    }


      for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 6; j++)
        {
          for (k = 0; k< 3; k++){
      			value[i][j] += MatrixA[i][k] * MatrixB[k][j];
			}
   	 	}
    	printf("\n");
  	}

    shmdt(value);
    shmctl(shmid, IPC_RMID, NULL);
    return 0;


}
