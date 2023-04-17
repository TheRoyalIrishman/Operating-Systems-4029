#include <pthread.h>
#include <iostream>
#include <chrono>
#include <string>
#include <vector>
#include <semaphore.h>
#include "clock.h"

// constants for how many chairs and students

#define numChairs 3
#define numStudents 7

// how we init a mutex

pthread_mutex_t taChairsMutex = PTHREAD_MUTEX_INITIALIZER;

// containers for barber chairs

vector<bool> takenTAChairs = {false};
vector<bool> emptyTAChairs = {false};

// flag for when to move on to next customer

bool stopOfficeHours = false;

// semaphores to actually deal with threading

sem_t studentsWaiting[numChairs];
sem_t taSemaphore;

// processes the barber actually cutting hair

void * taFunction(void * args) {
    bool hasAStudent = true;

    while (true) {
        pthread_mutex_lock(&taChairsMutex);

        for (int i = 0; i < numChairs; i++) {
            if (takenTAChairs.at(i) && emptyTAChairs.at(i)) {
                pthread_mutex_unlock(&taChairsMutex);

                cout << "TA started to help student number " << i << endl;

                // this function lies in a header file that deals with clock sync

                clockDelayFunction(15);

                emptyTAChairs.at(i) = true;

                sem_post(&(studentsWaiting[i]));

                cout << "TA finished helping student number " << i << endl;

                pthread_mutex_lock(&taChairsMutex);
            }
        }

        pthread_mutex_unlock(&taChairsMutex);

        hasAStudent = false;

        pthread_mutex_lock(&taChairsMutex);

        for (int i = 0; i < numChairs; i++) {
            if (takenTAChairs.at(i)) {
                hasAStudent = true;
                break;
            }
        }

        if (stopOfficeHours == true) {
            return nullptr;
        }

        // checks for if the barber doesn't have any hair to cut

        if (hasAStudent == false) {
            cout << "TA fell asleep" << endl;

            pthread_mutex_unlock(&taChairsMutex);

            sem_wait(&taSemaphore);
            
            pthread_mutex_lock(&taChairsMutex);

            cout << "TA woke up" << endl;
        }

        pthread_mutex_unlock(&taChairsMutex);
    }
}

void * studentFunction(void * args) {
    cout << "Student entered office hours" << endl;

    int chairNumber = numChairs;

    pthread_mutex_lock(&taChairsMutex);

    bool hasAStudent = false;

    // alerts TA that they have a student

    for (int i = 0; i < numChairs; i++) {
        if (takenTAChairs.at(i)) {
            hasAStudent = true;
            break;
        }
    }

    for (int i = 0; i < numChairs; i++) {
        if (!takenTAChairs.at(i)) {
            chairNumber = i;

            takenTAChairs.at(i) = true;

            if (!hasAStudent) {
                sem_post(&taSemaphore);
            }

            break;
        }
    }

    pthread_mutex_unlock(&taChairsMutex);

    if (chairNumber == numChairs) {
        cout << "No seats open - student left office hours" << endl;
        return nullptr;
    }

    cout << "Student sat down in chair number " << chairNumber << endl;

    pthread_mutex_lock(&taChairsMutex);

    while (!emptyTAChairs.at(chairNumber)) {
        pthread_mutex_unlock(&taChairsMutex);
        sem_wait(&(studentsWaiting[chairNumber]));
        pthread_mutex_lock(&taChairsMutex);
    }

    takenTAChairs.at(chairNumber) = false;
    emptyTAChairs.at(chairNumber) = false;

    cout << "Student in chair number " << chairNumber << " left the office" << endl;

    pthread_mutex_unlock(&taChairsMutex);

    return nullptr;
}

int main() {
    for (int i = 0; i < numChairs; i++) {
        sem_init(&(studentsWaiting[i]), 0, 0);
    }

    sem_init(&taSemaphore, 0, 0);

    // threads to handle process

    pthread_t taThread;

    pthread_t studentsThreads[numChairs];

    pthread_create(&taThread, NULL, taFunction, NULL);


    for (int i = 0; i < numChairs; i++) {
        clockDelayFunction(8);

        pthread_create(&(studentsThreads[i]), NULL, studentFunction, NULL);
    }

    for (int i = 0; i < numStudents; i++) {
        pthread_join((studentsThreads[i]), NULL);
    }

    pthread_mutex_lock(&taChairsMutex);

    stopOfficeHours = true;

    sem_post(&taSemaphore);

    pthread_mutex_unlock(&taChairsMutex);

    pthread_join(taThread, NULL);

    for (int i = 0; i < numChairs; i++) {
        sem_destroy(&(studentsWaiting[i]));
    }

    sem_destroy(&taSemaphore);

    return 0;
}