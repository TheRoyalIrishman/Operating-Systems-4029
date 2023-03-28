#include <iostream>
#include <pthread.h>
#include <semaphore.h>

using namespace std;

pthread_t taThread;
pthread_t studentThread[10];

sem_t studentSemaphore;
sem_t taSemaphore;

pthread_mutex_t m_mutex;

bool isTASleeping = true;

int numStudentsWaiting = 0;

void * taFunction(void * arg);
void * studentFunction(void * arg);

int main() {
    sem_init(&studentSemaphore, 0, 0);
    sem_init(&taSemaphore, 0, 1);
    pthread_mutex_init(&m_mutex, nullptr);

    return 0;
}