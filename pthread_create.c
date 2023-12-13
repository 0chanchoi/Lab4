#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// 스레드에서 실행될 함수
void *thread_function(void *arg) {
    int *value = (int *)arg;
    printf("This is a thread function. Received argument: %d\n", *value);
    pthread_exit(NULL);
}

int main() {
    pthread_t thread_id;
    int argument_value = 20;
    
    // 새로운 스레드를 생성하여 thread_function 실행
    if (pthread_create(&thread_id, NULL, thread_function, (void *)&argument_value) != 0) {
        fprintf(stderr, "Failed to create thread.\n");
        return 1;
    }
    
    // 메인 스레드에서 다른 작업 수행
    
    pthread_join(thread_id, NULL); // 새로운 스레드가 종료될 때까지 기다림
    return 0;
}

S
