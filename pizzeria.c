#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "pizzeria.h"
#include <time.h>
#include <unistd.h>

pthread_mutex_t lock;
pthread_mutex_t lockorder;
pthread_mutex_t lockcook;
pthread_mutex_t lockoven;
pthread_mutex_t deliverers_mutex;
int ordermaxtime =0 ;
int ordermaxcoolingtime=0 ;
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
const int arrsize ;
int* totaltimearr;
int *totalcoolingtimearr;
int seed2=0;
pthread_mutex_t availabledeliverers_mutex ;
pthread_cond_t condcook;
pthread_cond_t condoven;
pthread_cond_t condpacker;
pthread_cond_t deliverers_cond;

void* afterorder(void* argument) {
    struct timespec start_time;
    struct timespec bake_time;
    struct timespec pack_time;
    struct timespec end_time;
//-----------------------------initialization and check if  thread(order) gets destroyed-----------------------------------------
    pthread_mutex_lock(&lock);
    int totaltime = 0, totalcoolingtime = 0,totalpacktime =0,totaltimebeforecooling=0,endaftercooling=0,endbeforecooling =0, start, end, baking_completion, delivered;
    int* threadindexp = (int*)argument;
    int threadindex = *threadindexp;
     int seed = seed2 +threadindex;
    pthread_mutex_unlock(&lock);

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
    pthread_mutex_unlock(&lock);

    sleep(paymenttime);

    pthread_mutex_lock(&lockorder);
    if (rand_r(&seed) % 10 == 1) {
        printf("Η παραγγελία με αριθμό %d απέτυχε\n", threadindex);
        failedorders++;
        pthread_mutex_unlock(&lockorder);
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
    pthread_mutex_unlock(&lockorder);
    pthread_mutex_lock(&lockcook);
    //--------------------------------cook part of each thread individually ----------------------------------------------------------------/////////
    clock_gettime(CLOCK_REALTIME, &start_time);
    start = start_time.tv_sec;
    while (availablecook == 0) {
        pthread_cond_wait(&condcook, &lockcook);
    }
    availablecook--;
    printf("Ο μάγειρας %d ανέλαβε την παραγγελία %d\n", Ncook - availablecook, threadindex);
    pthread_mutex_unlock(&lockcook);

    int preptime = Tprep * pizzaamount;
    sleep(preptime);

    pthread_mutex_lock(&lockcook);
    availablecook++;
    pthread_cond_signal(&condcook);
    pthread_mutex_unlock(&lockcook);


    //--------------------------------baking part of each thread individually ----------------------------------------------------------------/////////
    pthread_mutex_lock(&lockoven);
    while (availableoven == 0) {
        pthread_cond_wait(&condoven, &lockoven);
    }
    availableoven--;
    printf("Ο φούρνος %d ανέλαβε την παραγγελία %d\n", Noven - availableoven, threadindex);
    pthread_mutex_unlock(&lockoven);

    int baketime = Tbake;
    sleep(baketime / 1000);
    //--------------------------------packing part of each thread individually ----------------------------------------------------------------/////////
    pthread_mutex_lock(&lockcook);
    while (availablepacker == 0) {
        pthread_cond_wait(&condpacker, &lockcook);
    }

    availablepacker--;
    clock_gettime(CLOCK_REALTIME, &bake_time);
    int b = bake_time.tv_sec;
    totaltimebeforecooling = b-start;
    printf("Ο συσκευαστής %d ανέλαβε την παραγγελία %d\n", Npacker - availablepacker, threadindex);
    availableoven++;
    pthread_cond_signal(&condoven);
    pthread_mutex_unlock(&lockcook);
    int packtime = pizzaamount * Tpack;
    packeditems+=pizzaamount;
    sleep(packtime);
    clock_gettime(CLOCK_REALTIME, &pack_time);
    endaftercooling = pack_time.tv_sec;
    totalpacktime = endaftercooling-start;
    printf("Η παραγγελία με αριθμό %d ετοιμάστηκε σε %d λεπτά\n",threadindex,totalpacktime);
    pthread_mutex_lock(&lockcook);
    availablepacker++;
    pthread_cond_signal(&condpacker);
    pthread_mutex_unlock(&lockcook);



    pthread_mutex_lock(&deliverers_mutex);
    //--------------------------------delivery part of each thread individually ----------------------------------------------------------------/////////
    while (availabledeliverers == 0) {
        pthread_cond_wait(&deliverers_cond, &deliverers_mutex);
    }
    availabledeliverers--;
    printf("deliveryman  ανέλαβε την παραγγελία %d\n",threadindex);

    pthread_mutex_unlock(&deliverers_mutex);

    int deliverytime = ((rand_r(&seed) % 5) + 1) * 3;
    sleep(deliverytime);
    clock_gettime(CLOCK_REALTIME, &end_time);
    end = end_time.tv_sec;
    totaltime = end - start;
    if(ordermaxcoolingtime<totaltimebeforecooling){
        ordermaxcoolingtime = totaltimebeforecooling;
    }
    printf("Η παραγγελία με αριθμό %d παραδόθηκε σε %d λεπτά.\n",threadindex,totaltime);
    if(ordermaxtime<totaltime){
        ordermaxtime = totaltime;
    }
    totaltimearr[threadindex] = totaltime;
    totalcoolingtimearr[threadindex] = totaltimebeforecooling;

    sleep(deliverytime);

    pthread_mutex_lock(&deliverers_mutex);

    availabledeliverers++;

    pthread_cond_signal(&deliverers_cond);

    pthread_mutex_unlock(&deliverers_mutex);

    pthread_exit(NULL);
}

int main(int argc, char *argv[]){
    if (argc != 3) {
        printf("starting the program needs 2 args\n");
        return 1;
    }

    int N= atoi(argv[1]);
    totaltimearr = malloc(N * sizeof(int));
    totalcoolingtimearr = malloc(N * sizeof(int));
    int seed = atoi(argv[2]);
    int id[N];
     seed2 = seed;
    pthread_t pthreads[N];

    pthread_mutex_init(&lock, NULL);
    pthread_mutex_init(&lockorder, NULL);
    pthread_mutex_init(&lockcook, NULL);
    pthread_mutex_init(&lockoven, NULL);
    pthread_mutex_init(&deliverers_mutex, NULL);
    pthread_cond_init(&deliverers_cond, NULL);
    pthread_cond_init(&condcook, NULL);
    pthread_cond_init(&condoven, NULL);
    pthread_cond_init(&condpacker, NULL);


    pthread_mutex_lock(&availabledeliverers_mutex);
    availabledeliverers = Ndeliverer;
    pthread_mutex_unlock(&availabledeliverers_mutex);

    for (int i = 0; i < N; i++) {
        id[i] = i+1;
        int variable = seed+id[i];
        pthread_create(&pthreads[i], NULL, afterorder, &id[i]);
        sleep( (rand_r(&variable) % Torderhigh) + Torderlow);
    }

    for (int i = 0; i < N; i++) {
        pthread_join(pthreads[i], NULL);
    }

    pthread_mutex_destroy(&lock);
    pthread_mutex_destroy(&lockorder);
    pthread_mutex_destroy(&lockcook);
    pthread_mutex_destroy(&lockoven);
    pthread_mutex_destroy(&deliverers_mutex);
    pthread_cond_destroy(&deliverers_cond);
    pthread_cond_destroy(&condcook);
    pthread_cond_destroy(&condoven);
    pthread_cond_destroy(&condpacker);


    printf("Total income: %d\n", totalincome);
    printf("Failed orders: %d\n", failedorders);
    printf("Successful orders: %d\n", successfulorders);
    printf("Total special sales: %d\n", totalspecialsales);
    printf("Total normal pizza sales: %d\n", totalnormalpizzasales);
    printf("Packed items: %d\n", packeditems);
    printf("Total special pizzas sold: %d\n", totalspecialpizzassold);
    printf("Total normal pizzas sold: %d\n", totalnormalpizzassold);

    int totaltimeadd = 0;
    int totalcoolingtimeadd = 0;
    for (int i = 0; i < N; i++) {
        totaltimeadd += totaltimearr[i];
        totalcoolingtimeadd += totalcoolingtimearr[i];
    }
    printf("Average time per order: %.2f minutes\n", (float)totaltimeadd / (N-failedorders));
    printf("max time of an order : %.2f minutes\n",(float ) ordermaxtime);
    printf("Average cooling time per order: %.2f minutes\n", (float)totalcoolingtimeadd / (N-failedorders));
    printf("max cooling time of an  order: %.2f minutes\n", (float)ordermaxcoolingtime);
    printf("percentage of ");
    free(totalcoolingtimearr);
    free(totalcoolingtimearr);
    return 0;
}
