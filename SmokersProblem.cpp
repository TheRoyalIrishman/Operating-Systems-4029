#include <pthread.h>
#include <iostream>
#include <chrono>
#include <string>
#include <semaphore.h>
#include "clock.h"

using namespace std;

sem_t requestSemaphore;
sem_t itemSemaphore;

string neededItems[] = {""};
string itemsOnHand[] = {""};

bool stopSmoking = false;
bool stopAgent = false;

sem_t smokerWaiting;
sem_t agentWaiting;

// structure defining items had and items needed

struct smokerProps {
    string itemHad;
    string firstItemNeeded;
    string secondItemNeeded;
} properties;

// function to define the agent (hander of items)

void * agentFunction(void * args) {
    sem_wait(&itemSemaphore);

    while (!stopAgent) {
        sem_post(&itemSemaphore);
        sem_wait(&agentWaiting);
        sem_wait(&itemSemaphore);

        // this all will handle the items had and not had

        if (!neededItems[0].empty() && !neededItems[1].empty()) {
            itemsOnHand[0] = neededItems[0];
            itemsOnHand[1] = neededItems[1];
            neededItems[0] = "";
            neededItems[1] = "";

            cout << "Agent put " << itemsOnHand[0] << " and " << itemsOnHand[1] << " on the table" << endl;

            sem_post(&smokerWaiting);
        }
    }

    sem_post(&itemSemaphore);

    return nullptr;
}

// function to handle the smoker (roller)

void * smokerFunction(void * args) {
    string itemHad = properties.itemHad;
    string firstNeed = properties.firstItemNeeded;
    string secondNeed = properties.secondItemNeeded;

    sem_wait(&itemSemaphore);

    while (!stopSmoking) {
        sem_post(&itemSemaphore);

        sem_wait(&requestSemaphore);
        sem_wait(&itemSemaphore);

        neededItems[0] = firstNeed;
        neededItems[1] = secondNeed;

        cout << "Smoker has " << neededItems[0] << " and " << neededItems[1] << endl;

        sem_post(&agentWaiting);

        while (itemsOnHand[0].compare(firstNeed) != 0 || itemsOnHand[1].compare(secondNeed) != 0) {
            sem_post(&itemSemaphore);
            sem_wait(&smokerWaiting);
            sem_wait(&itemSemaphore);
        }

        itemsOnHand[0] = "";
        itemsOnHand[1] = "";

        sem_post(&itemSemaphore);
        sem_post(&requestSemaphore);

        // sleep for 0.5 seconds

        clockDelayFunction(500);

        // sleep for 2 seconds

        cout << "Smoker with " << itemHad << " has smoked a cigarette" << endl;

        sem_wait(&itemSemaphore);
    }

    sem_post(&itemSemaphore);

    return nullptr;
}

int main() {
    // setting up the semaphores

    sem_init(&agentWaiting, 0, 0);
    sem_init(&smokerWaiting, 0, 0);
    sem_init(&requestSemaphore, 0, 1);
    sem_init(&itemSemaphore, 0, 1);

    // threads

    pthread_t agentThread;
    pthread_t smokerThreads[3];

    struct smokerProps * tobaccoSmoker = (struct smokerProps *)malloc(sizeof(struct smokerProps));
    struct smokerProps * paperSmoker = (struct smokerProps *)malloc(sizeof(struct smokerProps));
    struct smokerProps * matchesSmoker = (struct smokerProps *)malloc(sizeof(struct smokerProps));

    tobaccoSmoker->itemHad = "Tobacco";
    tobaccoSmoker->firstItemNeeded = "Matches";
    tobaccoSmoker->secondItemNeeded = "Paper";

    paperSmoker->itemHad = "Paper";
    paperSmoker->firstItemNeeded = "Matches";
    paperSmoker->secondItemNeeded = "Tobacco";

    matchesSmoker->itemHad = "Matches";
    matchesSmoker->firstItemNeeded = "Paper";
    matchesSmoker->secondItemNeeded = "Tobacco";

    // agent and smokers thread

    pthread_create(&agentThread, NULL, agentFunction, NULL);
    clockDelayFunction(200);
    pthread_create(&(smokerThreads[0]), NULL, smokerFunction, (void *) tobaccoSmoker);
    pthread_create(&(smokerThreads[1]), NULL, smokerFunction, (void *) paperSmoker);
    pthread_create(&(smokerThreads[2]), NULL, smokerFunction, (void *) matchesSmoker);

    clockDelayFunction(10);

    // stop smokers

    sem_wait(&itemSemaphore);
    stopSmoking = true;
    sem_post(&itemSemaphore);

    // unifies each of the three threads

    for (int i = 0; i < 3; i++) {
        pthread_join((smokerThreads[i]), NULL);
    }

    sem_wait(&itemSemaphore);
    stopAgent = true;
    sem_post(&agentWaiting);
    sem_post(&itemSemaphore);

    // waiting for agent thread to stop

    pthread_join(agentThread, NULL);

    // clear out all semaphores

    sem_destroy(&agentWaiting);
    sem_destroy(&smokerWaiting);
    sem_destroy(&requestSemaphore);
    sem_destroy(&itemSemaphore);

    return 0;
}