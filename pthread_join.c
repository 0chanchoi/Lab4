#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void *thread_function(void *arg) {
    printf("This is the thread function.\n");
    sleep(2);
    return NULL;
}

int main() {
    pthread_t thread_id;

    pthread_create(&thread_id, NULL, thread_function, NULL);
    
    // 생성된 스레드가 종료될 때까지 기다림
    pthread_join(thread_id, NULL);
    
    printf("Thread has finished.\n");

    return 0;
}

