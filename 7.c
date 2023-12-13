#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/wait.h>

#define PORT 3490  // 기본 포트
#define QLEN 10    // 요청 큐 크기
#define BUF_SIZE 1024

void handle_clnt(int client_sock);

int main(int argc, char *argv[]) {
    int sockfd, new_fd;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    int alen;
    fd_set readfds, activefds;
    int i, maxfd = 0, numbytes;
    char buf[100];

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket() failed");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind() failed");
        exit(1);
    }

    if (listen(sockfd, QLEN) < 0) {
        fprintf(stderr, "listen failed\n");
        exit(1);
    }

    alen = sizeof(client_addr);

    printf("Server up and running on port %d...\n", PORT);

    FD_ZERO(&activefds);
    FD_SET(sockfd, &activefds);
    maxfd = sockfd;

    while (1) {
        printf("SERVER: Waiting for contact...\n");
        readfds = activefds;

        if (select(maxfd + 1, &readfds, NULL, NULL, NULL) < 0) {
            perror("select");
            exit(EXIT_FAILURE);
        }

        for (i = 0; i <= maxfd; i++) {
            if (FD_ISSET(i, &readfds)) {
                if (i == sockfd) {
                    if ((new_fd = accept(sockfd, (struct sockaddr *)&client_addr, &alen)) < 0) {
                        fprintf(stderr, "accept failed\n");
                        exit(1);
                    }
                    FD_SET(new_fd, &activefds);
                    if (new_fd > maxfd)
                        maxfd = new_fd;
                } else {
                    printf("handle clnt\n");
                    handle_clnt(i);
                    close(i);
                    FD_CLR(i, &activefds);
                }
            }
        }
    }
    close(sockfd);
    return 0;
}

void handle_clnt(int client_sock) {
    int recv = 0, str_len = 0;
    char msg[BUF_SIZE];
    char method[10];
    char* contentLengthStr = "Content-Length:";
    int contentLength = 0;
    char* body = NULL;
    char* data;

    while ((str_len = read(client_sock, &msg[recv], BUF_SIZE - recv - 1)) > 0) {
        recv += str_len;
        // '\r\n\r\n'가 나타나면 바디 데이터 시작
        body = strstr(msg, "\r\n\r\n");
        if (body != NULL) {
            body += 4; // '\r\n\r\n'의 길이만큼 뒤로 이동하여 본문을 가리킴
            break;
        }
    }

    if (str_len <= 0) {
        printf("read() error or connection closed!\n");
        exit(EXIT_FAILURE);
    }

    strcpy(method, strtok(msg, " "));

    // POST 요청 처리
    if (strcmp(method, "POST") == 0) {
        char *contentPos = strstr(msg, contentLengthStr);
        if (contentPos != NULL) {
            contentLength = atoi(contentPos + strlen(contentLengthStr));
        }

        if (body != NULL) {
            data = malloc(contentLength + 1);
            strncpy(data, body, contentLength);
            data[contentLength] = '\0';

            // 받은 데이터를 서버 콘솔에 출력
            printf("Received POST data: %s\n", data);

            // 받은 데이터를 클라이언트로 응답으로 보냄
            char response[BUF_SIZE];
            snprintf(response, sizeof(response),
                     "HTTP/1.1 200 OK\r\n"
                     "Server: SimpleHTTPServer\r\n"
                     "Content-Type: text/plain\r\n"
                     "\r\n"
                     "Received data: %s\r\n",
                     data);
            write(client_sock, response, strlen(response));
            free(data);
        } else {
            printf("POST data not found\n");
            exit(EXIT_FAILURE);
        }
    } else if (strcmp(method, "GET") == 0) {
        char protocol[] = "HTTP/1.1 200 OK\r\n";
        char server[] = "Server: SimpleHTTPServer\r\n";
        char contenttype[] = "Content-Type: text/html\r\n";
        char end[] = "\r\n";
        char html[] = "<html><head><title>Simple HTTP Server</title></head><body><h1>Hello, World!</h1></body></html>\r\n";

        write(client_sock, protocol, strlen(protocol));
        write(client_sock, server, strlen(server));
        write(client_sock, contenttype, strlen(contenttype));
        write(client_sock, end, strlen(end));
        write(client_sock, html, strlen(html));
    } else {
        printf("Unsupported method\n");
    }
}


