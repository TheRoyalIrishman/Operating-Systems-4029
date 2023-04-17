#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <string>
#include <errno.h>
#include <time.h>
#include "clock.h"

int streetCustomers; // in main function, be sure to make this a cin
int numChairs; // in main function, be sure to make this a cin
int secondsToWait; // in main function, be sure to make this a cin

sem_t waitingCustomersSemaphore;
sem_t waitingBarbersSemaphore;
pthread_mutex_t pMutex;
sem_t servedCustomersSemaphore;
sem_t workingBarbersSemaphore;

struct timespec m_timespec;

int numWaitingCustomers = 0;

void cutCustomerHair() {
    cout << "Currently cutting hair....";

    auto currentTime = 0;

    while (currentTime < 50) {
        currentTime += clockFunction();
    }

    sem_post(&servedCustomersSemaphore);
}

void * barberFunction(void * args) {
    cout << "Waiting for customers..." << endl;

    int waitedTooLong = sem_timedwait(&waitingCustomersSemaphore, &m_timespec);

    if (waitedTooLong == -1) {
        cout << "Barber slept too long, and the customer left" << endl;
    }

    pthread_mutex_lock(&pMutex);

    cout << "# waiting customers: " << numWaitingCustomers << endl;

    numWaitingCustomers--;

    sem_post(&waitingBarbersSemaphore);

    pthread_mutex_unlock(&pMutex);

    cutCustomerHair();

    sem_wait(&workingBarbersSemaphore);

    if (numWaitingCustomers < 1) {
        cout << "Barber going to sleep";
    }

    return NULL;
}

void * customerFunction(void * args) {
    pthread_mutex_lock(&pMutex);

    cout << "Customer enters barbershop...." << endl;

    if (numWaitingCustomers < numChairs) {
        cout << "Customer finds a chair" << endl;

        numWaitingCustomers++;

        sem_post(&waitingCustomersSemaphore);

        pthread_mutex_unlock(&pMutex);

        sem_wait(&waitingBarbersSemaphore);
        sem_wait(&servedCustomersSemaphore);

        cout << "Customer's hair was cut...." << endl;

        sem_post(&workingBarbersSemaphore);
    } else {
        cout << "Customer leaves the shop...." << endl;
        pthread_mutex_unlock(&pMutex);
    }

    return NULL;
}

void createCustomers(int numCustomers) {
    pthread_t customers[numCustomers];

    for (int i = 0; i < numCustomers; i++) {
        pthread_create(&customers[i], NULL, customerFunction, NULL);
    }

    for (int j = 0; j < numCustomers; j++) {
        pthread_join(customers[j], NULL);
    }
}

int main() {
    cout << "Enter number of customers from the street: ";
    cin >> streetCustomers;
    cout << "Enter number of chairs in shop: ";
    cin >> numChairs;
    cout << "Enter expected wait time: ";
    cin >> secondsToWait;

    pthread_t barber_thread;
    pthread_t customer1;
    pthread_t customer2;
    pthread_t customer3;
    pthread_t customer4;
    sem_init(&waitingCustomersSemaphore, 0, 0);
    sem_init(&waitingBarbersSemaphore, 0, 0);
    sem_init(&servedCustomersSemaphore, 0, 0);
    sem_init(&workingBarbersSemaphore, 0, 0);

    cout << "The number of chairs is: "  << numChairs << endl;
    cout << "The number of customers that will come in is: "  << streetCustomers << endl << endl;

    pthread_create(&barber_thread, NULL, barberFunction, NULL);
    createCustomers(streetCustomers);
    pthread_join(barber_thread, NULL);

    return 0;
}