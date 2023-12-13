#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h> // rand 함수를 위한 헤더 추가

#define BUFFER_SIZE 5 // 제한 버퍼 크기SS
#define NUM_PRODUCERS 2 // 생산자 수
#define NUM_CONSUMERS 2 // 소비자 수

int buffer[BUFFER_SIZE]; // 제한 버퍼
int in = 0, out = 0; // 버퍼의 입출력 인덱스

sem_t empty, full, mutex; // 세마포어 선언

void *producer(void *arg) {
    int item;
    while (1) {
        item = rand() % 100; // 랜덤 데이터 생성
        sem_wait(&empty); // 빈 공간을 기다림
        sem_wait(&mutex); // 버퍼에 접근하는 것을 보호

        buffer[in] = item;
        printf("Produced: %d\n", item);
        in = (in + 1) % BUFFER_SIZE;

        sem_post(&mutex); // 버퍼 접근 보호 해제
        sem_post(&full); // 버퍼가 찼음을 알림
        sleep(1); // 생산자가 일시적으로 대기
    }
}

void *consumer(void *arg) {
    int item;
    while (1) {
        sem_wait(&full); // 버퍼가 찰 때까지 기다림
        sem_wait(&mutex); // 버퍼에 접근하는 것을 보호

        item = buffer[out];
        printf("Consumed: %d\n", item);
        out = (out + 1) % BUFFER_SIZE;

        sem_post(&mutex); // 버퍼 접근 보호 해제
        sem_post(&empty); // 버퍼가 비었음을 알림
        sleep(1); // 소비자가 일시적으로 대기
    }
}

int main() {
    pthread_t producer_threads[NUM_PRODUCERS];
    pthread_t consumer_threads[NUM_CONSUMERS];

    sem_init(&empty, 0, BUFFER_SIZE); // 빈 공간 개수 초기화
    sem_init(&full, 0, 0); // 찬 공간 개수 초기화
    sem_init(&mutex, 0, 1); // 세마포어 초기화

    int i;
    for (i = 0; i < NUM_PRODUCERS; i++)
        pthread_create(&producer_threads[i], NULL, producer, NULL);

    for (i = 0; i < NUM_CONSUMERS; i++)
        pthread_create(&consumer_threads[i], NULL, consumer, NULL);

    for (i = 0; i < NUM_PRODUCERS; i++)
        pthread_join(producer_threads[i], NULL);

    for (i = 0; i < NUM_CONSUMERS; i++)
        pthread_join(consumer_threads[i], NULL);

    sem_destroy(&empty);
    sem_destroy(&full);
    sem_destroy(&mutex);

    return 0;
}

