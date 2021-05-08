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
#define SEPARATOR ":"
#define SUCCESS_CODE "1"
#define FAILED_CODE "0"
#define LOGIN_CODE "1"
#define REGISTER_CODE "2"
#define BUFFER_LENGTH 1024

int logIn(int sock)
{
    char buffer[BUFFER_LENGTH];
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
    char buffer[BUFFER_LENGTH];
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
    printf("mari ngirim\n");
    close(fd);
}

int main(int argc, char const *argv[])
{
    struct sockaddr_in address;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }

    int isLoggedIn = 0;
    char cmd[10];
    char pub[100];
    char thn[4];
    char filePth[100];
    while (1)
    {
        char pkt[1024];
        clear_buffer(buffer);

        if (!isLoggedIn)
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
        else if (isLoggedIn)
        {
            int fd, remData;
            off_t offset;
            int sentBytes = 0;
            struct stat file_stat;
            sleep(1);

            printf("Enter command : ");
            scanf("%s", cmd);
            if (!strcmp(cmd, "add"))
            {
                printf("cmd : add\n");
                printf("Publisher: ");
                scanf("%s", pub);
                printf("Tahun Publikasi: ");
                scanf("%s", thn);
                printf("Filepath: ");
                scanf("%s", filePth);

                sprintf(pkt, "%s\t%s\t%s", filePth, pub, thn);

                // strcpy(pkt, "path.eks\tpubliser\ttahunpub");
                printf("||%s||\n", pkt);
                send(sock, pkt, sizeof(pkt), 0);

                //-----------------------------------
                uplodFile(sock, filePth);
                //-----------------------------------------

                valread = read(sock, buffer, BUFFER_LENGTH);
                if (buffer[0] == 's')
                {
                    printf("data received by server\n");
                }
                else
                {
                    printf("error\n");
                }
            }
        }
    }

    return 0;
}