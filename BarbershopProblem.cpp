#include <pthread.h>
#include <iostream>
#include <chrono>
#include <string>
#include <vector>
#include <semaphore.h>
#include "clock.h"

// constants for how many chairs and customers

#define numChairs 10
#define numCustomers 12

// how we init a mutex

pthread_mutex_t taChairsMutex = PTHREAD_MUTEX_INITIALIZER;

// containers for barber chairs

vector<bool> takenTAChairs = {false};
vector<bool> emptyTAChairs = {false};

// flag for when to move on to next customer

bool stopOfficeHours = false;

// semaphores to actually deal with threading

sem_t customersWaiting[numChairs];
sem_t taSemaphore;

// processes the barber actually cutting hair

void * taFunction(void * args) {
    bool hasACustomer = true;

    while (true) {
        pthread_mutex_lock(&taChairsMutex);

        for (int i = 0; i < numChairs; i++) {
            if (takenTAChairs.at(i) && emptyTAChairs.at(i)) {
                pthread_mutex_unlock(&taChairsMutex);

                cout << "Barber started to cut customer number " << i << "'s hair" << endl;

                // this function lies in a header file that deals with clock sync

                clockDelayFunction(15);

                emptyTAChairs.at(i) = true;

                sem_post(&(customersWaiting[i]));

                cout << "Barber finished cutting customer number " << i << "'s hair" << endl;

                pthread_mutex_lock(&taChairsMutex);
            }
        }

        pthread_mutex_unlock(&taChairsMutex);

        hasACustomer = false;

        pthread_mutex_lock(&taChairsMutex);

        for (int i = 0; i < numChairs; i++) {
            if (takenTAChairs.at(i)) {
                hasACustomer = true;
                break;
            }
        }

        if (stopOfficeHours == true) {
            return nullptr;
        }

        // checks for if the barber doesn't have any hair to cut

        if (hasACustomer == false) {
            cout << "Barber fell asleep" << endl;

            pthread_mutex_unlock(&taChairsMutex);

            sem_wait(&taSemaphore);
            
            pthread_mutex_lock(&taChairsMutex);

            cout << "Barber woke up" << endl;
        }

        pthread_mutex_unlock(&taChairsMutex);
    }
}

void * studentFunction(void * args) {
    cout << "Customer entered store" << endl;

    int chairNumber = numChairs;

    pthread_mutex_lock(&taChairsMutex);

    bool hasACustomer = false;

    // alerts barber that they have a customer

    for (int i = 0; i < numChairs; i++) {
        if (takenTAChairs.at(i)) {
            hasACustomer = true;
            break;
        }
    }

    for (int i = 0; i < numChairs; i++) {
        if (!takenTAChairs.at(i)) {
            chairNumber = i;

            takenTAChairs.at(i) = true;

            if (!hasACustomer) {
                sem_post(&taSemaphore);
            }

            break;
        }
    }

    pthread_mutex_unlock(&taChairsMutex);

    if (chairNumber == numChairs) {
        cout << "No seats open - customer left store" << endl;
        return nullptr;
    }

    cout << "Customer sat down in chair number " << chairNumber << endl;

    pthread_mutex_lock(&taChairsMutex);

    while (!emptyTAChairs.at(chairNumber)) {
        pthread_mutex_unlock(&taChairsMutex);
        sem_wait(&(customersWaiting[chairNumber]));
        pthread_mutex_lock(&taChairsMutex);
    }

    takenTAChairs.at(chairNumber) = false;
    emptyTAChairs.at(chairNumber) = false;

    cout << "Customer in chair number " << chairNumber << " left the shop" << endl;

    pthread_mutex_unlock(&taChairsMutex);

    return nullptr;
}

int main() {
    for (int i = 0; i < numChairs; i++) {
        sem_init(&(customersWaiting[i]), 0, 0);
    }

    sem_init(&taSemaphore, 0, 0);

    // threads to handle process

    pthread_t barberThread;

    pthread_t customerThreadsArray[numCustomers];

    pthread_create(&barberThread, NULL, taFunction, NULL);


    for (int i = 0; i < numChairs; i++) {
        clockDelayFunction(8);

        pthread_create(&(customerThreadsArray[i]), NULL, studentFunction, NULL);
    }

    for (int i = 0; i < numCustomers; i++) {
        pthread_join((customerThreadsArray[i]), NULL);
    }

    pthread_mutex_lock(&taChairsMutex);

    stopOfficeHours = true;

    sem_post(&taSemaphore);

    pthread_mutex_unlock(&taChairsMutex);

    pthread_join(barberThread, NULL);

    for (int i = 0; i < numChairs; i++) {
        sem_destroy(&(customersWaiting[i]));
    }

    sem_destroy(&taSemaphore);

    return 0;
}