#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void *thread_function(void *arg) {
    printf("This is the thread function.\n");
    return NULL;
}

int main() {
    pthread_t thread_id;

    pthread_create(&thread_id, NULL, thread_function, NULL);
    pthread_detach(thread_id); // 스레드를 detached 상태로 설정

    // 이후에는 pthread_join을 호출하지 않아도 됩니다.
    
    // 메인 스레드가 종료되면 detached 스레드의 자원이 자동으로 회수됩니다.
    sleep(1); // 메인 스레드가 바로 종료되는 것을 막기 위해 잠시 기다림

    return 0;
}
vi 
