# Laporan Penjelasan Soal Shift Modul 3

## Anggota

- Mohammad Faderik Izzul Haq (05111940000023)
- Jonathan Timothy Siregar (05111940000120)
- Abiya Sabitta Ragadani (05111940000166)

## SOAL 1
Pada soal ini, diminta untuk menangani program server-client dengan socket programming dan thread.

A. Pada saat client tersambung dengan server, terdapat dua pilihan pertama, yaitu register dan login. Jika memilih register, client akan diminta input id dan passwordnya untuk dikirimkan ke server. User juga dapat melakukan login. Login berhasil jika id dan password yang dikirim dari aplikasi client sesuai dengan list akun yang ada didalam aplikasi server. Sistem ini juga dapat menerima multi-connections. Koneksi terhitung ketika aplikasi client tersambung dengan server. Jika terdapat 2 koneksi atau lebih maka harus menunggu sampai client pertama keluar untuk bisa melakukan login dan mengakses aplikasinya. Keverk menginginkan lokasi penyimpanan id dan password pada file bernama akun.txt dengan format : `id:password`
<p align="center">
  <img src="soal1/login.png" width="800">
</p>
<p align="center">
  <img src="soal1/reg.png" width="800">
</p>

#### Client
- User diminta memasukan credential : id dan password, lalu digabungkan menjadi string sesuai dengan format, kemudian client mengirim string tersebut ke server.
```c
int logIn(int sock)
{
    ...
    printf("Username : ");
    scanf("%s", uname);
    printf("Password: ");
    scanf("%s", pwd);
    sprintf(cred, "%s:%s", uname, pwd);
    printf("Cred : %s\n", cred);

    send(sock, cred, sizeof(cred), 0);
    int valread = read(sock, buffer, sizeof(buffer));

    if (buffer[0] == 1 + '0')
    {
        printf("Login berhasil\n");
        return 1;
    }
    else
    {
        printf("Login gagal\n");
        return 0;
    }
}

void reg(int sock)
{
    ...
    printf("Username : ");
    scanf("%s", uname);
    printf("Password: ");
    scanf("%s", pwd);
    sprintf(cred, "%s:%s", uname, pwd);
    printf("Cred : %s\n", cred);

    send(sock, cred, sizeof(cred), 0);
    int valread = read(sock, buffer, sizeof(buffer));

    if (buffer[0] == 1 + '0')
    {
        printf("Reg berhasil\n");
    }
    else
    {
        printf("Reg gagal\n");
    }
}
```
#### Server
- Pada server perlu inisialisasi server dan membuat thread untuk dapat menghandle multi-connection
- Fungsi `handle_new_connection()` digunakan untuk menghandle aplikasi utama untuk setiap koneksi baru yang terhubunf ke server
```c
void launchServer()
{
    ...
    pthread_t tid[100];
    int connections = 0;

    // inisialisasi server socket

    while (1)
    {
        printf("While launchServer()\n");
        if ((socketfd = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            perror("accept failed");
            exit(EXIT_FAILURE);
        }

        pthread_create(&(tid[connections]), NULL, &handle_new_connection, &socketfd);
        pthread_join(tid[connections], NULL);
        connections++;
    }
}

void *handle_new_connection(void *arg)
{
    char buffer[BUFSIZ];
    int valread;
    int socketfd = *(int *)arg;

    clear_buffer(buffer);
    valread = read(socketfd, buffer, BUFSIZ);
    fprintf(stdout, "Receive connection handshake %s\n", buffer);

    send(socketfd, SUCCESS, strlen(SUCCESS), 0);
    fprintf(stdout, "Sent response %s\n", SUCCESS);

    mainApp(socketfd);
}
```
- Fungsi `login()` akan menerima credential dari client dan mengecek kevalidan data `id:pwd` dengan yang telah terdaftar di `akun.txt` , jika berhasil credential direturn sebagai string ke mainApp untuk disimpan
- Fungsi `reg()` digunakan untuk menghadle register dari client, server menyimpan string yang diterima dari client `id:pwd` ke file `akun.txt`
```c
int reg(int socketfd)
{
    FILE *fp;
    fp = fopen("akun.txt", "a+");
    ...
    while (fscanf(fp, "%s", cred) != EOF)
    {
        if (!strcmp(cred, buffer))
        {
            printf("Cred : %s Already exist\n", cred);
            isReg = 0;
            break;
        }
    }
    if (isReg != 0)
    {
        fprintf(fp, "%s\n", buffer);
        printf("Cred %s Registered\n", buffer);
        isReg = 1;
    }

    sprintf(sent, "%d", isReg);
    send(socketfd, sent, BUFSIZ, 0);

    fclose(fp);
    return isReg;
}

char *logIn(int sock)
{
    FILE *fp = fopen("akun.txt", "r");
    ...
    int isLogin = 0;

    memset(buffer, 0, sizeof(buffer));
    int valread = read(sock, buffer, BUFSIZ);

    while (fscanf(fp, "%s", cred) != EOF)
    {
        if (!strcmp(cred, buffer))
        {
            isLogin = 1;
            break;
        }
        else
        {
            for (q = 0; q < 100; q++)
                cred[q] = '\0';
        }
    }

    sprintf(sent, "%d", isLogin);
    send(sock, sent, sizeof(sent), 0);

    return cred;
}
```
- 


## Soal 2

### Sub Soal A

Pada soal ini, kita diminta untuk melakukan suatu operasi pada dua buah matrix, yakni matrixA 4 * 3 dan matrixB 3 * 6. Kedua matrix tersebut akan dimasukkan menggunakan fungsi `scanf`, kemudian dilakukan perkalian kedua matrix tersebut dan terakhir akan dilakukan penyimpanan matrixHasil untuk selanjutnya digunakan pada [soal2a](soal2/soal2a.c).

#### Memasukkan 2 buah matrix, matrixA 4 * 3 dan matrixB 3 * 6.

``` cpp
printf("Enter the elements of MatrixA\n");

  for (i = 0 ; i < 4 ; i++ )
    for ( j = 0 ; j < 3 ; j++ )
      scanf("%d", &MatrixA[i][j]);



    printf("Enter the elements of MatrixB\n");

    for ( i = 0 ; i < 3 ; i++ )
      for ( j = 0 ; j < 6 ; j++ )
        scanf("%d", &MatrixB[i][j]);
```

#### Mengoperasikan kedua matrix dengan operasi perkalian

``` cpp
printf("Product of entered Matrix:\n");

    for ( i = 0 ; i < 4 ; i++ )
    {
      for ( j = 0 ; j < 6 ; j++ )
      {
        for ( k = 0 ; k < 3 ; k++ )
        {
          value[i][j] += MatrixA[i][k]*MatrixB[k][j];
        }

        printf("%d\t", value[i][j]);
      }
	printf("\n");
    }
 
 ```

#### Menyimpan hasil perkalian matrix dalam suatu variabel value

``` cpp
    key_t key = 1234;
  	int (*value)[6];

  	int shmid = shmget(key, sizeof(int[4][6]), IPC_CREAT | 0666);
  	value = shmat(shmid, NULL, 0);
    
    shmdt(value);

```


### Sub Soal B

Pada soal ini, kita diminta untuk mengoperasikan matriks hasil perkalian [Sub soal 2a](README.md#sub-soal-a) dengan suatu matrix matrixC yang memiliki ukuran yang sama (4 * 6). Pada sub soal ini, kita kembali diminta memasukkan isi dari matrixC. Nantinya, faktorial dari setiap elemen matrix hasil operasi sub soal sebelumnya akan dilakukan operasi pembagian dengan faktorial dari setiap elemen matrixC. 

#### Memasukkan matrixC

``` cpp
    printf("Enter the elements of MatrixC\n");

    for (i = 0 ; i < 4 ; i++ ){
    	for ( j = 0 ; j < 3 ; j++ ){
      		scanf("%d", &MatrixC[i][j]);
	}
    }
```
    
#### Melakukan pengecekan

``` cpp
    for(i = 0; i < 4; i++){
        for(j = 0; j < 6; j++){
	    nullCheck = false;
            long long *val = malloc(sizeof(long long[4][6]));
            *val = value[i][j];

            subs = value[i][j] - MatrixC[i][j];			// Melakukan pengurangan tiap elemen antar matrix
            if(value[i][j] == 0 || MatrixC[i][j] == 0) 
	    	nullCheck = true;				// Apabila ada elemen yang 0, maka nullCheck bernilai true
```


#### Fungsi-fungsi yang digunakan

##### Faktorial

``` cpp
long long faktorial (int n) {
    if (n <= 1) return 1;				// untuk bilangan 0 atau 1, mengembalikan nilai 1
    else return n * faktorial( n - 1);
}

long long reducer(int n){
	if (n == subs) return 1;
        else return n * reducer(n - 1);
}

void *process(void* arg){
	long long num = *(long long*)arg;
	if(nullCheck)					// Apabila selisih elemen antar matrix = 0 (nullCheck true)
		printf("0 ");				// maka dicetak nilai 0
	else if(subs < 1)				// Apabila selisih elemen antar matrix < 1 
		printf("%lld ", faktorial(num));	// dicetak faktorial bilangan itu sendiri
        else 						// Apabila selisih elemen antar matrix > 1 
		printf("%lld ", reducer(num));		// dicetak reducer bilangan tersebut
}
```

##### Pembuatan thread

``` cpp
    int num;
    key_t key = 1234;
    int (*value)[6];

    int shmid = shmget(key, sizeof(int[4][6]), IPC_CREAT | 0666);
    value = shmat(shmid, NULL, 0);

    pthread_t tid[24];
    
                pthread_create(&tid[m], NULL, &process, val);
            sleep(1);
            m++;
        }
        printf("\n");
    }

    for (i = 0; i < m; i++) {
        pthread_join(tid[i], NULL);
    }

    shmdt(value);
    shmctl(shmid, IPC_RMID, NULL);
    

```
