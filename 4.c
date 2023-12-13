#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define MAX_CLIENTS 5 // 최대 클라이언트 수

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // 뮤텍스 초기화
pthread_cond_t cond = PTHREAD_COND_INITIALIZER; // 조건 변수 초기화

void *client_thread(void *arg) {
    int client_id = *((int *)arg);

    // 클라이언트 쓰레드는 여기서 메시지 전송 요청 후 서버로부터 메시지를 받는 대기 상태로 들어감
    printf("Client %d: Waiting for message from server...\n", client_id);

    // 서버가 메시지를 방송할 때까지 대기 (조건 변수를 이용한 대기)
    pthread_mutex_lock(&mutex);
    pthread_cond_wait(&cond, &mutex);
    pthread_mutex_unlock(&mutex);
S
    printf("Client %d: Received message from server!\n", client_id);

    return NULL;
}

void *server_thread(void *arg) {
    // 서버 쓰레드는 모든 클라이언트에게 메시지를 방송
    printf("Server: Broadcasting message to all clients...\n");

    // 모든 클라이언트에게 메시지를 방송 (조건 변수를 이용하여 클라이언트들을 깨움)
    pthread_mutex_lock(&mutex);
    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mutex);

    return NULL;
}

int main() {
    pthread_t server_tid;
    pthread_t client_tids[MAX_CLIENTS];

    int client_ids[MAX_CLIENTS];
    for (int i = 0; i < MAX_CLIENTS; ++i)
        client_ids[i] = i + 1;

    // 클라이언트 쓰레드 생성
    for (int i = 0; i < MAX_CLIENTS; ++i)
        pthread_create(&client_tids[i], NULL, client_thread, &client_ids[i]);

    // 서버 쓰레드 생성
    pthread_create(&server_tid, NULL, server_thread, NULL);

    // 대기
    sleep(1); // 서버가 메시지를 방송하기 위한 대기 시간

    // 클라이언트 쓰레드 종료
    for (int i = 0; i < MAX_CLIENTS; ++i)
        pthread_join(client_tids[i], NULL);

    // 서버 쓰레드 종료
    pthread_join(server_tid, NULL);

    return 0;
}

