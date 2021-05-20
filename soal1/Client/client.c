#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/sendfile.h>

#define PORT 8080
#define HANDSHAKE "800"
#define SUCCESS "200"

int login(int);
void reg(int);
void clear_buffer(char *);
void uploadFile(int, char a[]);
void downFile(int, char a[]);
int create_socket();
void menuApp(int);
void connect_to_server(int);

int logIn(int sock)
{
    char buffer[BUFSIZ];
    char uname[100];
    char pwd[100];
    char cred[201];

    printf("Username : ");
    scanf("%s", uname);
    printf("Password: ");
    scanf("%s", pwd);
    sprintf(cred, "%s:%s", uname, pwd);
    printf("Cred : %s\n", cred);

    send(sock, cred, sizeof(cred), 0);
    int valread = read(sock, buffer, sizeof(buffer));
    // printf("||%s||\n", buffer);

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
    // memset(buffer, 0, sizeof(buffer));
}

void reg(int sock)
{
    char buffer[BUFSIZ];
    char uname[100];
    char pwd[100];
    char cred[201];

    printf("Username : ");
    scanf("%s", uname);
    printf("Password: ");
    scanf("%s", pwd);
    sprintf(cred, "%s:%s", uname, pwd);
    printf("Cred : %s\n", cred);

    send(sock, cred, sizeof(cred), 0);
    int valread = read(sock, buffer, sizeof(buffer));
    // printf("||%s||\n", buffer);

    if (buffer[0] == 1 + '0')
    {
        printf("Reg berhasil\n");
    }
    else
    {
        printf("Reg gagal\n");
    }
    memset(buffer, 0, sizeof(buffer));
}

void clear_buffer(char *b)
{
    int i;
    for (i = 0; i < BUFSIZ; i++)
        b[i] = '\0';
}

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

    sprintf(filePth, "%s", fileName);
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

int create_socket()
{
    struct sockaddr_in address;
    int socketfd, valread;
    struct sockaddr_in serv_addr;
    char buffer[BUFSIZ] = {0};
    if ((socketfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        fprintf(stdout, "\n Socket creation error \n");
        return -1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        fprintf(stdout, "\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(socketfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        fprintf(stdout, "\nConnection Failed \n");
        return -1;
    }

    return socketfd;
}

void menuApp(int sock)
{
    char buffer[BUFSIZ];
    int valread;
    int isLoggedIn = 0;
    char cmd[BUFSIZ];
    char pkt[BUFSIZ];
    char pub[100];
    char thn[4];
    char filePth[100];

    while (1)
    {
        clear_buffer(buffer);
        clear_buffer(pkt);

        if (isLoggedIn == 0)
        {
            printf("== Chose One (Case UNsensitive) ==\nL : Login\nR : Register\n");
            printf("Enter Your Choise : ");
            scanf("%s", pkt);

            if (strlen(pkt) == 1 && (pkt[0] == 'L' || pkt[0] == 'l'))
            {
                // printf("Choice : Login\n");
                strcpy(pkt, "L");
                send(sock, pkt, sizeof(pkt), 0);
                isLoggedIn = logIn(sock);
                printf("%d\n", isLoggedIn);
            }
            else if (strlen(pkt) == 1 && (pkt[0] == 'R' || pkt[0] == 'r'))
            {
                // printf("Choice : Register\n");
                strcpy(pkt, "R");
                send(sock, pkt, sizeof(pkt), 0);
                reg(sock);
            }
            else
            {
                printf("Choice not valid\n");
            }
        }
        else if (isLoggedIn == 1)
        {
            int fd, remData;
            off_t offset;
            int sentBytes = 0;
            struct stat file_stat;
            sleep(1);

            printf("Enter command : ");
            scanf(" %[^\n]c", cmd);

            if (!strcmp(cmd, "add"))
            {
                printf("Command : add\n");
                send(sock, "a", BUFSIZ, 0);
                char temp[100];

                printf("Publisher: ");
                scanf("%s", pub);
                strcpy(temp, pub); //opoiki
                printf("Tahun Publikasi: ");
                scanf("%s", thn);
                printf("Filepath: ");
                scanf("%s", filePth);

                sprintf(pkt, "%s\t%s\t%s", filePth, temp, thn);

                // strcpy(pkt, "path.eks\tpubliser\ttahunpub");
                printf("||%s||\n", pkt);
                send(sock, pkt, sizeof(pkt), 0);

                //-----------------------------------
                uplodFile(sock, filePth);
                //-----------------------------------------

                valread = read(sock, buffer, BUFSIZ);
                if (buffer[0] == 's')
                {
                    printf("Data received by server ############################\n");
                }
                else
                {
                    printf("error\n");
                }
            }
            else if (strstr(cmd, "download") != NULL)
            {
                printf("Command : download\n");
                send(sock, "d", BUFSIZ, 0);

                char *buf[2];
                int i = 0;
                buf[i] = strtok(cmd, " ");
                while (buf[i] != NULL)
                {
                    buf[++i] = strtok(NULL, " ");
                }

                sprintf(pkt, "%s", buf[1]);
                printf("||%s||\n", pkt);
                send(sock, pkt, sizeof(pkt), 0);

                //-----------------------------------
                // downFile(sock);
                //-----------------------------------------

                valread = read(sock, buffer, BUFSIZ);
                if (buffer[0] == 's')
                {
                    printf("File Found\n");
                    downFile(sock, buf[1]);
                }
                else
                {
                    printf("FILE NOT FOUND\n");
                }
            }
            else if (strstr(cmd, "delete") != NULL)
            {
                printf("Command : delete\n");
                send(sock, "r", BUFSIZ, 0);

                char *buf[2];
                int i = 0;
                buf[i] = strtok(cmd, " ");
                while (buf[i] != NULL)
                {
                    buf[++i] = strtok(NULL, " ");
                }

                sprintf(pkt, "%s", buf[1]);
                printf("||%s||\n", pkt);
                send(sock, pkt, sizeof(pkt), 0);

                //-----------------------------------
                // deleteFile(sock);
                //-----------------------------------------

                valread = read(sock, buffer, BUFSIZ);
                if (buffer[0] == 's')
                {
                    printf("File Found\n");
                    // deleteFile(sock, buf[1]);
                }
                else
                {
                    printf("FILE NOT FOUND\n");
                }
            }
            else if (!strcmp(cmd, "see"))
            {
                printf("Command : see\n");
                send(sock, "l", BUFSIZ, 0);

                read(sock, buffer, BUFSIZ);
                printf("%s", buffer);
                clear_buffer(buffer);

                valread = read(sock, buffer, BUFSIZ);
                if (buffer[0] == 's')
                {
                    printf("SEE Completed\n");
                    // deleteFile(sock, buf[1]);
                }
                else
                {
                    printf("error seeing serever\n");
                }
            }
            else if (strstr(cmd, "find") != NULL)
            {
                printf("Command : find\n");
                send(sock, "f", BUFSIZ, 0);

                char *buf[2];
                int i = 0;
                buf[i] = strtok(cmd, " ");
                while (buf[i] != NULL)
                {
                    buf[++i] = strtok(NULL, " ");
                }

                sprintf(pkt, "%s", buf[1]);
                printf("||%s||\n", pkt);
                send(sock, pkt, sizeof(pkt), 0);

                read(sock, buffer, BUFSIZ);
                printf("%s", buffer);
                clear_buffer(buffer);

                valread = read(sock, buffer, BUFSIZ);
                if (buffer[0] == 's')
                {
                    printf("FIND Completed\n");
                    // deleteFile(sock, buf[1]);
                }
                else
                {
                    printf("error seeing serever\n");
                }
            }
            else if (strstr(cmd, "end") != NULL)
            {
                send(sock, "n", BUFSIZ, 0);
                break;
            }
            else
            {
                fprintf(stdout, "Command not valid\n");
            }
        }
    }
    printf("BREAKED !\n");
}

void connect_to_server(int socketfd)
{
    char buffer[BUFSIZ];
    int valread;

    send(socketfd, HANDSHAKE, strlen(HANDSHAKE), 0);

    fprintf(stdout, "Menunggu koneksi dari server...\n");

    clear_buffer(buffer);
    valread = read(socketfd, buffer, BUFSIZ);
    fprintf(stdout, "Terhubung dengan server\n");

    menuApp(socketfd);
}

int main(int argc, char const *argv[])
{
    int socketfd = create_socket();
    if (socketfd == -1)
        exit(0);

    connect_to_server(socketfd);

    return 0;
}