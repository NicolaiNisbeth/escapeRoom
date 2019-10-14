#include <stdio.h>

// 2 escape rooms
// 1 visit rooms
    // 4 stole
// 1 instructor


#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
int myglobal = 10;
void *thread_function(void *arg) {

    while (myglobal > 0){
        myglobal=myglobal-1;
        printf("B:%d\n",myglobal);
        fflush(stdout);
        sleep(1);
    }
    return NULL;
}
int main(void) {
    pthread_t mythread;
    if ( pthread_create( &mythread, NULL, thread_function,NULL) ) {
        abort();
    }
    while (myglobal > 0){
        myglobal=myglobal-1;
        printf("A:%d\n",myglobal);
        fflush(stdout);
        sleep(1);
    }

    if ( pthread_join ( mythread, NULL ) ) {
        printf("error joining thread.");
        abort();
    }
    printf("\nmyglobal equals %d\n", myglobal);
    exit(0);
}

