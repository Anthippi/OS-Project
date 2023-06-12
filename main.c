#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "define.h"
#include <time.h>
#include <unistd.h>
#include <stdbool.h>
#define UNINITIALIZED_VALUE -1

pthread_mutex_t lock;
pthread_mutex_t lock2;
pthread_mutex_t lock3;
pthread_mutex_t lock5;
int totalincome = 0;
int failedorders = 0;
int successfulorders = 0;
int totalspecialsales = 0;
int totalnormalpizzasales = 0;
int availablecook = Ncook;
int availableoven = Noven;
int availablepacker = Npacker;
int packeditems = 0;
int availabledeliverers = Ndeliverer;
int totalspecialpizzassold = 0;
int totalnormalpizzassold = 0;
int totaltimearr[100];
int totalcoolingtimearr[100];
pthread_cond_t cond1;
pthread_cond_t cond2;
pthread_cond_t cond3;
pthread_cond_t cond4;

struct timespec baking_t; //o xronos gia na pshthoun oi pitses
struct timespec etoimasia_t; //o xronos gia na etoimastoun oi pitses
struct timespec paradosh_t; //o xronos gia na paradothoun oi pitses
struct timespec anamonh_t; //o xronos kata th dhmiourgia enos thread
struct timespec thlefwnhma_t; //o xronos pou perimene sthn anamonh o kathe pelaths


void* afterorder(void* argument) {
    pthread_mutex_lock(&lock);
    int totaltime = 0;
    int totalcoolingtime = 0;
    int* threadindexp = (int*)argument;
    int threadindex = *threadindexp;
    unsigned int seed = time(NULL) + threadindex;
    int ordertime = (rand_r(&seed) % Torderhigh) + 1; //minutes
    printf("Thread %d: time to make the order = %d minutes\n", threadindex, ordertime);
    totaltime += ordertime; // να κανω τις αλλαγε με timespec.
    //clock_gettime(CLOCK_REALTIME, &anamonh_t);
    //totaltime = anamonh_t.tv_sec;

    pthread_mutex_unlock(&lock);

    sleep(ordertime);

    pthread_mutex_lock(&lock);
    int pizzaamount = (rand_r(&seed) % Norderhigh) + 1;
    int pizzas[pizzaamount];
    for (int i = 0; i < pizzaamount; i++) {
        if ((rand_r(&seed) % 10) >= (Pplain * 10)) { //40% special
            totalspecialpizzassold++;
            pizzas[i] = 1;
        }
        else {
            totalnormalpizzassold++;
            pizzas[i] = 0; //60% plain
        }
    }
    int paymenttime = (rand_r(&seed) % Tpaymenthigh) + 1;
    totaltime += paymenttime;
    pthread_mutex_unlock(&lock);

    sleep(paymenttime);

    pthread_mutex_lock(&lock2);
    if (rand_r(&seed) % 10 == 1) {
        printf("Η παραγγελία με αριθμό %d απέτυχε\n", threadindex);
        failedorders++;
        pthread_mutex_unlock(&lock2);
        pthread_exit(NULL);
    }
    successfulorders++;
    printf("Η παραγγελία με αριθμό %d καταχωρήθηκε.\n", threadindex);

    for (int i = 0; i < pizzaamount; i++) {
        if (pizzas[i] == 0) {
            totalincome += Cplain;
            totalnormalpizzasales++;
        }
        else {
            totalincome += Cspecial;
            totalspecialsales++;
        }
    }
    pthread_mutex_unlock(&lock2);

    pthread_mutex_lock(&lock3);
    while (availablecook == 0) {
        pthread_cond_wait(&cond1, &lock3);
    }
    availablecook--;
    printf("Ο μάγειρας %d ανέλαβε την παραγγελία %d\n", Ncook - availablecook, threadindex);
    pthread_mutex_unlock(&lock3);

    int preptime = Tprep * pizzaamount;
    totaltime += preptime;
    sleep(preptime);

    pthread_mutex_lock(&lock3);
    availablecook++;
    pthread_cond_signal(&cond1);
    pthread_mutex_unlock(&lock3);

    pthread_mutex_lock(&lock5);
    while (availableoven == 0) {
        pthread_cond_wait(&cond2, &lock5);
    }
    availableoven--;
    printf("Ο φούρνος %d ανέλαβε την παραγγελία %d\n", Noven - availableoven, threadindex);
    pthread_mutex_unlock(&lock5);

    int baketime = Tbake;
    totaltime += baketime;
    sleep(baketime / 1000);

    pthread_mutex_lock(&lock5);
    availableoven++;
    pthread_cond_signal(&cond2);
    pthread_mutex_unlock(&lock5);

    pthread_mutex_lock(&lock3);
    while (availablepacker == 0) {
        pthread_cond_wait(&cond3, &lock3);
    }
    availablepacker--;
    printf("Ο συσκευαστής %d ανέλαβε την παραγγελία %d\n", Npacker - availablepacker, threadindex);
    pthread_mutex_unlock(&lock3);

    int packtime = pizzaamount * Tpack;
    sleep(packtime);

    pthread_mutex_lock(&lock3);
    availablepacker++;
    pthread_cond_signal(&cond3);
    pthread_mutex_unlock(&lock3);

    pthread_mutex_lock(&lock);
    totaltimearr[threadindex] = totaltime;
    totalcoolingtimearr[threadindex] = totalcoolingtime;
    pthread_mutex_unlock(&lock);

    pthread_mutex_lock(&lock3);
    while (availabledeliverers == 0) {
        pthread_cond_wait(&cond4, &lock3);
    }
    availabledeliverers--;
    printf("Ο διανομέας %d ανέλαβε την παραγγελία %d\n", Ndeliverer - availabledeliverers, threadindex);
    pthread_mutex_unlock(&lock3);

    int deliverytime = ((rand_r(&seed) % 5) + 1) * 3;
    sleep(deliverytime);

    pthread_mutex_lock(&lock3);
    availabledeliverers++;
    pthread_cond_signal(&cond4);
    pthread_mutex_unlock(&lock3);

    pthread_exit(NULL);
}

int main(int argc, char *argv[]){
    if (argc != 3) {
        printf("starting the program needs 2 args\n");
        return 1;
    }
    int rc;
    int N = atoi(argv[1]);
    int seed = atoi(argv[2]);
    int id[N];
    pthread_t pthreads[N];

    pthread_mutex_init(&lock, NULL);
    pthread_mutex_init(&lock2, NULL);
    pthread_mutex_init(&lock3, NULL);
    pthread_mutex_init(&lock5, NULL);
    pthread_cond_init(&cond1, NULL);
    pthread_cond_init(&cond2, NULL);
    pthread_cond_init(&cond3, NULL);
    pthread_cond_init(&cond4, NULL);

    for (int i = 0; i < N; i++) {
        id[i] = i+1;
        pthread_create(&pthreads[i], NULL, afterorder, &id[i]);
    }

    for (int i = 0; i < N; i++) {
        pthread_join(pthreads[i], NULL);
    }

    pthread_mutex_destroy(&lock);
    pthread_mutex_destroy(&lock2);
    pthread_mutex_destroy(&lock3);
    pthread_mutex_destroy(&lock5);
    pthread_cond_destroy(&cond1);
    pthread_cond_destroy(&cond2);
    pthread_cond_destroy(&cond3);
    pthread_cond_destroy(&cond4);

    printf("Total income: %d\n", totalincome);
    printf("Failed orders: %d\n", failedorders);
    printf("Successful orders: %d\n", successfulorders);
    printf("Total special sales: %d\n", totalspecialsales);
    printf("Total normal pizza sales: %d\n", totalnormalpizzasales);
    printf("Packed items: %d\n", packeditems);
    printf("Total special pizzas sold: %d\n", totalspecialpizzassold);
    printf("Total normal pizzas sold: %d\n", totalnormalpizzassold);

    int totaltime = 0;
    int totalcoolingtime = 0;
    for (int i = 0; i < N; i++) {
        totaltime += totaltimearr[i];
        totalcoolingtime += totalcoolingtimearr[i];
    }
    printf("Average time per order: %.2f minutes\n", (float)totaltime / N);
    printf("Average cooling time per order: %.2f minutes\n", (float)totalcoolingtime / N);

    return 0;
}
