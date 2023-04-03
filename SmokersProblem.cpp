#include <pthread.h>
#include <semaphore.h>
#include <string>
#include <errno.h>
#include <time.h>
#include "clock.h"

using namespace std;

void paperTobaccoFunction();
void tobaccoMatchFunction();
void paperMatchFunction();
void pushTobacco();
void pushPaper();
void pushMatch();
void smokerWithTobacco();
void smokerWithPaper();
void smokerWithMatch();

// agent semaphores

sem_t tobaccoAgentSemaphore;
sem_t paperAgentSemaphore;
sem_t matchAgentSemaphore;
sem_t agentSemaphore;

// flags

bool isTobacco = false;
bool isPaper = false;
bool isMatch = false;

// pusher semaphores

sem_t tobaccoPusherSemaphore;
sem_t paperPusherSemaphore;
sem_t matchPusherSemaphore;

pthread_mutex_t pMutex;

void * paperTobaccoFunction(void * args) {
    while (true) {
        sem_wait(&agentSemaphore);
        sem_post(&paperAgentSemaphore);
        sem_post(&tobaccoAgentSemaphore);
    }
}

void * tobaccoMatchFunction(void * args) {
    while (true) {
        sem_wait(&agentSemaphore);
        sem_post(&tobaccoAgentSemaphore);
        sem_post(&matchAgentSemaphore);
    }
}

void * paperMatchFunction(void * args) {
    while (true) {
        sem_wait(&agentSemaphore);
        sem_post(&paperAgentSemaphore);
        sem_post(&matchAgentSemaphore);
    }
}

void * pushTobacco(void * args) {
    while (true) {
        sem_wait(&tobaccoAgentSemaphore);
        pthread_mutex_lock(&pMutex);

        if (isPaper) {
            isPaper = false;
            sem_post(&matchPusherSemaphore);
        } else if (isMatch) {
            isMatch = false;
            sem_post(&paperPusherSemaphore);
        } else {
            isTobacco = true;
        }

        pthread_mutex_unlock(&pMutex);
    }
}

void * pushMatch(void * args) {
    sem_wait(&matchAgentSemaphore);
    pthread_mutex_lock(&pMutex);

    if (isPaper) {
        isPaper = false;
        sem_post(&tobaccoPusherSemaphore);
    } else if (isTobacco) {
        isTobacco = false;
        sem_post(&paperPusherSemaphore);
    } else {
        isMatch = true;
    }

    pthread_mutex_unlock(&pMutex);
}

void * pushPaper(void * args) {
    while (true) {
        sem_wait(&paperAgentSemaphore);
        pthread_mutex_lock(&pMutex);

        if (isMatch) {
            isMatch = false;
            sem_post(&tobaccoPusherSemaphore);
        } else if (isTobacco) {
            isTobacco = false;
            sem_post(&matchPusherSemaphore);
        } else {
            isPaper = true;
        }

        pthread_mutex_unlock(&pMutex);
    }
}

int main() {
    cout << "HI I LIKE CARRIE";
    return 0;
}