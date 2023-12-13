#include <stdio.h>
#include <pthread.h>
#include <stdlib.h> // malloc 및 free 함수 선언 포함

void *thread_function(void *arg) {
    printf("This is the thread function.\n");
    int *result = malloc(sizeof(int));
    *result = 123;
    pthread_exit((void *)result); // 스레드 종료 시 값을 반환
}

int main() {
    pthread_t thread_id;
    int *thread_result;

    pthread_create(&thread_id, NULL, thread_function, NULL);
    
    pthread_join(thread_id, (void **)&thread_result); // 스레드의 반환 값을 받음
    printf("Thread returned: %d\n", *thread_result);

    free(thread_result); // 메모리 해제
    return 0;
}

