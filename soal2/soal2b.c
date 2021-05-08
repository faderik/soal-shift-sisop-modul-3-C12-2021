#include <stdio.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <pthread.h>
#include<sys/wait.h>
#include<sys/types.h>


int MatrixC[4][6], subs;
int i, j;
int MatrixF[4][6], nC = 0;
void *process(void* arg);
int hasil;


int faktorial(int n) {
    if (n <= 1) return 1;
    else return n * faktorial( n - 1);
}


void *process(void* arg){
	long long n = *(long long*)arg;
	if(nC = 1)
		printf("0 ");
	else
        hasil = faktorial(n)/faktorial(subs);
		printf("%d ", hasil);
}

int main(){
    int m = 0;

	key_t key = 1234;
    int (*value)[6];

    int shmid = shmget(key, sizeof(int[4][6]), IPC_CREAT | 0666);
    value = shmat(shmid, NULL, 0);

    printf("Enter the elements of MatrixC\n");

    int MatrixC[4][6];
    for (i = 0 ; i < 4 ; i++ )
    for ( j = 0 ; j < 3 ; j++ )
      scanf("%d", &MatrixC[i][j]);


    pthread_t tid[24];
    for(i = 0; i < 4; i++){
        for(j = 0; j < 6; j++){

            *val = value[i][j];

            long long *val = malloc(sizeof(long long[4][6]));
            subs = value[i][j] - MatrixC[i][j];
            if(value[i][j] == 0 || MatrixC[i][j] == 0) nC = 1;
            pthread_create(&tid[m], NULL, &process, val);
            sleep(5);
            m++;
        }
        printf("\n\n");
    }

    for (i = 0; i < m; i++) {
        pthread_join(tid[i], NULL);
    }

    shmdt(value);
    shmctl(shmid, IPC_RMID, NULL);

	return 0;
}
