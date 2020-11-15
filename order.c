#define _GNU_SOURCE
#include <pthread.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

volatile int X, Y, r1, r2;
volatile bool t1block, t2block, t1fin, t2fin;

void *thread1Func(void *param){
    for(;;){
        while(t1block); // Wait for blocker to be removed
        t1block = true; // Set up blocker

        X = 1;
        asm volatile("" ::: "memory"); // Prevent any compiler reordering
        r1 = Y;

        t1fin = true; // Signal iteration end
    }
    return NULL; // Never returns
}

void *thread2Func(void *param){
    for(;;){
        while(t2block); // Wait for blocker to be removed
        t2block = true; // Set up blocker

        Y = 1;
        asm volatile("" ::: "memory"); // Prevent any compiler reordering
        r2 = X;

        t2fin = true; // Signal iteration end
    }
    return NULL; // Never returns
}

int main(){
    //Init
    t1block = t2block = true; // Act as blockers for threads
    t1fin = t2fin = false; // Act as finish signals for threads

    // Spawn the threads
    pthread_t thread0, thread1, thread2;
    thread0 = pthread_self();
    pthread_create(&thread1, NULL, thread1Func, NULL);
    pthread_create(&thread2, NULL, thread2Func, NULL);

    /* Set the main thread affinity to CPU 0 */
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(0, &cpuset);
    int s = pthread_setaffinity_np(thread0, sizeof(cpu_set_t), &cpuset);
    if (s != 0){
        printf("Failed to set main thread affinity\n");
        return 0;
    }
           
    // Repeat the experiment
    int detected = 0, i = 1;
    for (i = 1; i < 1000000; i++){
        // Reset all variables
        X = 0;
        Y = 0;
        t1fin = t2fin = false;
        t1block = t2block = false;

        // Wait for threads to complete their iteration
        while(!(t1fin && t2fin));

        // Check if there was a memory reordering
        if (r1 == 0 && r2 == 0){
            detected++;
        }
    }
    printf("%d memory re-orderings detected in %d iterations - %d percent\n", detected, i, 100*detected/i);
    return 0;  // Never returns
}