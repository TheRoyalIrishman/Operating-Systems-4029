#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <iostream>
#include <semaphore.h>

using namespace std;

sem_t tobaccoSemaphore, paperSemaphore, matchesSemaphore, tableSemaphore;

void agentFunction() {
    while (true) {
        sem_wait(&tableSemaphore);

        int randValue = rand() % 3; // possible values are 0, 1, 2

        if (randValue == 0) {
            sem_post(&tobaccoSemaphore);
            sem_post(&paperSemaphore);
            cout << "Agent put tobacco and paper on the table.\n";
        } else if (randValue == 1) {
            sem_post(&tobaccoSemaphore);
            sem_post(&matchesSemaphore);
            cout << "Agent put tobacco and matches on the table.\n";
        } else {
            sem_post(&paperSemaphore);
            sem_post(&matchesSemaphore);
            cout << "Agent put paper and matches on the table.\n";
        }
    }
}

void tobbacoFunction() {
    while (true) {
        sem_wait(&tobaccoSemaphore);
        sem_wait(&tableSemaphore);
        cout << "Smoker with tobacco got paper and matches from the table and is now smoking.\n";

        this_thread::sleep_for(chrono::milliseconds(1000));
    }
}

int main() {
    cout << "hi I am Cam" << endl;

    return 0;
}