#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <pthread.h>

#define PORT 8080
#define HANDSHAKE "800"
#define SUCCESS "200"

void uplodFile(int sock, char filePth[])
{
    int fd, remData;
    off_t offset;
    int sentBytes = 0;
    struct stat file_stat;

    fd = open(filePth, O_RDONLY);
    if (fstat(fd, &file_stat) < 0)
        printf("errorrr");

    char file_size[50];
    sprintf(file_size, "%ld", file_stat.st_size);

    send(sock, file_size, sizeof(file_size), 0);
    fprintf(stdout, "Sent file size %s bytes\n", file_size);

    remData = file_stat.st_size;
    offset = 0;
    /* Sending file data */
    while (((sentBytes = sendfile(sock, fd, &offset, BUFSIZ)) > 0) && (remData > 0))
    {
        remData -= sentBytes;
    }
    close(fd);
}

void clear_buffer(char *b)
{
    int i;
    for (i = 0; i < BUFSIZ; i++)
        b[i] = '\0';
}

void downFile(int sock, char fileName[])
{
    int fileSize;
    FILE *rcvdFile;
    int i = 0;
    int remData;
    ssize_t len;
    char bufFile[BUFSIZ];
    char filePth[100];
    clear_buffer(bufFile);

    recv(sock, bufFile, BUFSIZ, 0);
    fileSize = atoi(bufFile);
    fprintf(stdout, "File size : %d\n", fileSize);

    sprintf(filePth, "FILES/%s", fileName);
    printf("FILE PATH : %s\n", filePth);
    rcvdFile = fopen(filePth, "w");
    if (rcvdFile == NULL)
    {
        printf("Failed to open file\n");

        exit(EXIT_FAILURE);
    }

    remData = fileSize;

    while ((remData > 0) && ((len = recv(sock, bufFile, BUFSIZ, 0)) > 0))
    {
        fwrite(bufFile, sizeof(char), len, rcvdFile);
        remData -= len;
    }
    fclose(rcvdFile);
}

char *logIn(int sock)
{
    FILE *fp = fopen("akun.txt", "r");
    char *cred = malloc(sizeof(char) * 100);
    int q;
    for (q = 0; q < 100; q++)
        cred[q] = '\0';

    char buffer[BUFSIZ];
    char sent[BUFSIZ];
    int isLogin = 0;

    memset(buffer, 0, sizeof(buffer));
    int valread = read(sock, buffer, BUFSIZ);

    while (fscanf(fp, "%s", cred) != EOF)
    {
        if (!strcmp(cred, buffer))
        {
            // printf("Cred : %s Logged In\n", cred);
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

int checkFile(char fileName[])
{
    FILE *fp = fopen("files.tsv", "r");
    char ss[100];

    while (fscanf(fp, "%s", ss) != EOF)
    {
        if (strstr(ss, fileName) != NULL)
        {
            printf("File : %s EXIST\n", fileName);
            return 1;
        }
    }
    printf("File : %s NOT FOUND\n", fileName);
    fclose(fp);
    return 0;
}

int reg(int socketfd)
{
    FILE *fp;
    fp = fopen("akun.txt", "a+");
    char buffer[BUFSIZ];
    char sent[BUFSIZ];
    int isReg = 1;

    memset(buffer, 0, BUFSIZ);
    int valread = read(socketfd, buffer, BUFSIZ);

    char cred[100];
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

void deleteLine(int line_number)
{
    FILE *fp, *fpTemp;
    char ss[BUFSIZ];
    int counter = 0;
    char ch;

    fp = fopen("files.tsv", "r");
    fpTemp = fopen("filesTemp.tsv", "w");

    while (!feof(fp))
    {
        strcpy(ss, "\0");
        fgets(ss, BUFSIZ, fp);
        if (!feof(fp))
        {
            counter++;
            if (counter != line_number)
            {
                fprintf(fpTemp, "%s", ss);
            }
        }
    }
    fclose(fp);
    fclose(fpTemp);
    remove("files.tsv");
    rename("filesTemp.tsv", "files.tsv");
}

int deleteFile(char fileName[])
{
    char line[256];

    FILE *fp = fopen("files.tsv", "r");
    int cLine = 0;

    while (fgets(line, sizeof line, fp))
    {
        cLine++;
        if (strstr(line, fileName) != NULL)
        {
            char filePth[BUFSIZ + 100];
            sprintf(filePth, "FILES/%s", fileName);

            char rmvFilePth[100];
            sprintf(rmvFilePth, "FILES/old-%s", fileName);

            deleteLine(cLine);

            rename(filePth, rmvFilePth);
            break;
        }
    }

    fclose(fp);
}

int see(int sock)
{
    char line[256], temp[256];
    char *buf[4];
    char *file[2];

    // char temp[256];
    // char *sent = malloc(sizeof(char) * BUFSIZ);
    char sent[BUFSIZ];
    clear_buffer(sent);

    FILE *fp = fopen("files.tsv", "r");
    if (fp == NULL)
    {
        printf("Failed to open file\n");
        exit(EXIT_FAILURE);
    }
    char nama[20], pub[20], thn[20], eks[20], pth[20];

    printf("Pokok Masok\n");
    while (fgets(line, sizeof line, fp))
    {
        // printf("MASOK WHILE\n");
        // printf("LINE : %s\n", line);

        int i = 0;
        buf[i] = strtok(line, "\t");
        while (buf[i] != NULL)
        {
            buf[++i] = strtok(NULL, "\t");
        }

        int j = 0;
        file[j] = strtok(buf[0], ".");
        while (file[j] != NULL)
        {
            file[++j] = strtok(NULL, ".");
        }

        strcpy(nama, file[0]);
        strcpy(eks, file[1]);
        strcpy(pub, buf[1]);

        strcpy(thn, buf[2]);
        int idx = strlen(thn) - 1;
        memmove(&thn[idx], &thn[idx + 1], strlen(thn) - (idx));

        sprintf(pth, "/FILES/%s.%s", file[0], file[1]);

        sprintf(
            temp,
            "Nama: %s\nPublisher: %s\nTahun Publishing: %s\nEkstensi File: %s\nFilePath: %s\n\n",
            nama, pub, thn, eks, pth);

        strcat(sent, temp);
    }

    fclose(fp);
    send(sock, sent, BUFSIZ, 0);

    return 1;
}

int find(int sock, char key[])
{
    char line[256], temp[256];
    char *buf[4];
    char *file[2];

    char sent[BUFSIZ];
    clear_buffer(sent);

    FILE *fp = fopen("files.tsv", "r");

    while (fgets(line, sizeof line, fp))
    {
        if (strstr(line, key) != NULL)
        {
            char nama[20], pub[20], thn[20], eks[20], pth[20];
            int i = 0;
            buf[i] = strtok(line, "\t");
            while (buf[i] != NULL)
            {
                buf[++i] = strtok(NULL, "\t");
            }

            int j = 0;
            file[j] = strtok(buf[0], ".");
            while (file[j] != NULL)
            {
                file[++j] = strtok(NULL, ".");
            }

            strcpy(nama, file[0]);
            if (strstr(nama, key) == NULL)
            {
                // nama not contain key
                continue;
            }
            strcpy(eks, file[1]);
            strcpy(pub, buf[1]);

            strcpy(thn, buf[2]);
            int idx = strlen(thn) - 1;
            memmove(&thn[idx], &thn[idx + 1], strlen(thn) - (idx));

            sprintf(pth, "/FILES/%s.%s", file[0], file[1]);

            sprintf(
                temp,
                "Nama: %s\nPublisher: %s\nTahun Publishing: %s\nEkstensi File: %s\nFilePath: %s\n\n",
                nama, pub, thn, eks, pth);

            strcat(sent, temp);
        }
    }

    fclose(fp);
    send(sock, sent, BUFSIZ, 0);

    return 1;
}

void mainApp(int new_socket)
{
    char buffer[BUFSIZ];
    int valread;
    char cmd[BUFSIZ] = {0};
    int isLoggedIn = 0;
    char cred[100];
    int q;
    for (q = 0; q < 100; q++)
        cred[q] = '\0';

    while (1)
    {
        // memset(buffer, 0, sizeof buffer); // reset buffer
        clear_buffer(buffer);

        if (!isLoggedIn)
        {
            valread = read(new_socket, buffer, BUFSIZ);
            if (!strcmp(buffer, "R"))
            {
                printf("Register ==\n");
                if (reg(new_socket) == 0)
                {
                    printf("Reg Failed\n");
                }
            }
            else if (!strcmp(buffer, "L"))
            {
                printf("Login ==\n");
                // cred = logIn(new_socket);
                strcpy(cred, logIn(new_socket));
                isLoggedIn = 1;
                printf("##%s##\n", cred);
                if (cred[0] == '\0')
                {
                    isLoggedIn = 0;
                    printf("Login Failed\n");
                }
            }
        }
        else if (isLoggedIn)
        {
            read(new_socket, cmd, BUFSIZ);
            printf("Command : %s\n", cmd);
            if (strstr(cmd, "a"))
            {
                char *buf[3];
                int i = 0;
                // clear_buffer(buffer);
                valread = read(new_socket, buffer, BUFSIZ);

                buf[i] = strtok(buffer, "\t");
                while (buf[i] != NULL)
                {
                    buf[++i] = strtok(NULL, "\t");
                }

                // -----------------------------------------------------------
                downFile(new_socket, buf[0]);
                // -----------------------------------------------------------

                FILE *fp;
                fp = fopen("files.tsv", "a+");

                fprintf(fp, "%s\t%s\t%s\n", buf[0], buf[1], buf[2]);
                send(new_socket, "s", BUFSIZ, 0);

                fclose(fp);

                FILE *flog = fopen("running.log", "a");
                fprintf(flog, "Tambah : %s (%s)\n", buf[0], cred);
                fclose(flog);
            }
            else if (strstr(cmd, "d"))
            {
                valread = read(new_socket, buffer, BUFSIZ);

                printf("||%s||\n", buffer);
                char filePth[BUFSIZ + 100];
                sprintf(filePth, "FILES/%s", buffer);
                if (checkFile(buffer))
                {
                    send(new_socket, "s", BUFSIZ, 0);
                    uplodFile(new_socket, filePth);
                }
                else
                {
                    send(new_socket, "x", BUFSIZ, 0);
                    continue;
                }
            }
            else if (strstr(cmd, "r"))
            {
                valread = read(new_socket, buffer, BUFSIZ);

                printf("||%s||\n", buffer);
                char filePth[BUFSIZ + 100];
                sprintf(filePth, "FILES/%s", buffer);
                if (checkFile(buffer))
                {
                    send(new_socket, "s", BUFSIZ, 0);
                    // uplodFile(new_socket, filePth);
                    deleteFile(buffer);
                    FILE *flog = fopen("running.log", "a");
                    fprintf(flog, "Hapus : %s (%s)\n", buffer, cred);
                    fclose(flog);
                }
                else
                {
                    send(new_socket, "x", BUFSIZ, 0);
                    continue;
                }
            }
            else if (strstr(cmd, "l") != NULL)
            {
                if (see(new_socket))
                {
                    printf("SEE Result Sent!\n");
                    send(new_socket, "s", BUFSIZ, 0);
                }
                else
                {
                    send(new_socket, "x", BUFSIZ, 0);
                    continue;
                }
            }
            else if (strstr(cmd, "f") != NULL)
            {
                read(new_socket, buffer, BUFSIZ);
                printf("Find Key : %s\n", buffer);
                if (find(new_socket, buffer))
                {
                    printf("FIND Result Sent!\n");

                    send(new_socket, "s", BUFSIZ, 0);
                }
                else
                {
                    send(new_socket, "x", BUFSIZ, 0);
                    continue;
                }
            }
            else if (strstr(cmd, "n") != NULL)
            {
                printf("BEFORE BREAKED !\n");
                sleep(2);
                break;
                printf("AFTER BREAKED !\n");
            }
        }
    }
    printf("BREAKED !\n");
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

void launchServer()
{
    int server_fd, socketfd, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[BUFSIZ] = {0};

    pthread_t tid[100];
    int connections = 0;

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0)
    {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

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

int main(int argc, char const *argv[])
{
    launchServer();

    return 0;
}