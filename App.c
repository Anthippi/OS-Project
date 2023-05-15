#include <pthread.h>
#include "define.h"
#include <time.h>

pthread_mutex_t lockCook;
pthread_cond_t condCook;
pthread_mutex_t lockOven;
pthread_cond_t condOven;
pthread_mutex_t lockPack;
pthread_cond_t condPack;
pthread_mutex_t lockDeliver;
pthread_cond_t condDeliver;

int income = 0;

struct timespec waiting_for_ordering;
struct timespec payment_time;
struct timespec pizza_preparation_time;
struct timespec baking_time;
struct timespec delivering_time;


