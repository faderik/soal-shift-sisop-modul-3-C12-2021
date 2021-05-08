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

#define PORT 8080
#define BUFFER_LENGTH 1024
#define SEPARATOR ":"
#define SUCCESS_CODE "1"
#define FAILED_CODE "0"
#define LOGIN_CODE "1"
#define REGISTER_CODE "2"

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
void downFileREF(int socketfd, char filename[])
{
    char buffer[BUFSIZ];
    int valread;

    int file_size;
    int remain_data;
    ssize_t len;
    FILE *received_file;

    // send(socketfd, SUCCESS, strlen(SUCCESS), 0);
    // fprintf(stdout, "Sent status %s\n", SUCCESS);

    clear_buffer(buffer);
    recv(socketfd, buffer, BUFSIZ, 0);
    file_size = atoi(buffer);

    fprintf(stdout, "Received file size : %d\n", file_size);

    char file_path[100];
    sprintf(file_path, "FILES/%s", filename);

    received_file = fopen(file_path, "w");
    if (received_file == NULL)
    {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    remain_data = file_size;

    while ((remain_data > 0) && ((len = recv(socketfd, buffer, BUFSIZ, 0)) > 0))
    {
        fwrite(buffer, sizeof(char), len, received_file);
        fprintf(stdout, "%s\n", buffer);
        remain_data -= len;
        fprintf(stdout, "Receive %ld bytes and %d bytes remaining\n", len, remain_data);
    }
    fclose(received_file);
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

int logIn(int sock)
{
    FILE *fp = fopen("akun.txt", "r");
    char cred[100];

    char buffer[BUFFER_LENGTH];
    char sent[BUFFER_LENGTH];
    int isLogin = 0;

    memset(buffer, 0, sizeof(buffer));
    int valread = read(sock, buffer, BUFFER_LENGTH);

    while (fscanf(fp, "%s", cred) != EOF)
    {
        if (!strcmp(cred, buffer))
        {
            printf("Cred : %s Logged In\n", cred);
            isLogin = 1;
            break;
        }
    }

    sprintf(sent, "%d", isLogin);
    send(sock, sent, sizeof(sent), 0);
    return isLogin;
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

    return 0;
}

int reg(int socketfd)
{
    FILE *fp;
    fp = fopen("akun.txt", "a+");
    char buffer[BUFFER_LENGTH];
    char sent[BUFFER_LENGTH];
    int isReg = 1;

    memset(buffer, 0, BUFFER_LENGTH);
    int valread = read(socketfd, buffer, BUFFER_LENGTH);

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
    send(socketfd, sent, BUFFER_LENGTH, 0);

    fclose(fp);
    return isReg;
}

int main(int argc, char const *argv[])
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char cmd[2] = {0};

    printf("SERVER EMPAN\n");
    char cwd[100];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        printf("Current working dir: %s\n", cwd);
        // mkdir("FILES", 0777);
    }

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
        perror("listen");
        exit(EXIT_FAILURE);
    }

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    int isLoggedIn = 0;

    while (1)
    {
        // memset(buffer, 0, sizeof buffer); // reset buffer
        clear_buffer(buffer);

        if (!isLoggedIn)
        {
            valread = read(new_socket, buffer, BUFFER_LENGTH);
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
                isLoggedIn = logIn(new_socket);
                if (isLoggedIn == 0)
                {
                    printf("Login Failed\n");
                }
            }
        }
        else if (isLoggedIn)
        {
            read(new_socket, cmd, 2);
            printf("Command : %s\n", cmd);
            if (strstr(cmd, "a"))
            {
                char *buf[3];
                int i = 0;
                // clear_buffer(buffer);
                valread = read(new_socket, buffer, BUFFER_LENGTH);

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
                send(new_socket, "s", BUFFER_LENGTH, 0);

                //--
                fclose(fp);
            }
            else if (strstr(cmd, "d"))
            {
                valread = read(new_socket, buffer, BUFFER_LENGTH);

                char *buf[2];
                int i = 0;
                buf[i] = strtok(buffer, " ");
                while (buf[i] != NULL)
                {
                    buf[++i] = strtok(NULL, " ");
                }
                printf("||%s||%s||\n", buf[0], buf[1]);
                char filePth[100];
                sprintf(filePth, "FILES/%s", buf[1]);
                if (checkFile(buf[1]))
                {
                    send(new_socket, "s", BUFFER_LENGTH, 0);
                    uplodFile(new_socket, filePth);
                }
                else
                {
                    send(new_socket, "x", BUFFER_LENGTH, 0);
                    continue;
                }
            }
        }
    }

    return 0;
}