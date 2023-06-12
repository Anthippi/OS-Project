#include <pthread.h>
#include "define.h"
#include <time.h>

pthread_mutex_t lockCook, lockOven, lockPack, lockDeliver;
pthread_cond_t condCook, condOven, condPack, condDeliver;

int NumOfCooks = Ncook;
int NumOfOvens = Noven;
int NumOfPackers = Npacker;
int NumOfDelivers = Ndeliverer;

int total_income = 0;
int plain_sales = 0, special_sales = 0;
int successful_orders = 0, failed_orders = 0;
int max_service_time = 0, total_service_time = 0;
int max_cooling_time = 0,  total_cooling_time = 0;
int seed;

struct timespec receive_order_time;
struct timespec waiting_for_ordering;
struct timespec payment_time;
struct timespec pizza_preparation_time;
struct timespec baking_time;
struct timespec delivering_time;

void *order(void *x){
    int order_number = *(int *)x;
    int service_time;
    int baking_time;
    int preparation_time;
    int delivering_time;

    clock_getting(CLOCK_REALTIME, &receive_order_time);
    service_time = receive_order_time.tv_sec;

}