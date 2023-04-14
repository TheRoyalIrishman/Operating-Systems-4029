#include <semaphore.h>
#include <iostream>
#include <pthread.h>

using namespace std;

class monitor {
    private:
        int readerCount;
        int writerCount;
        int waitingReaders;
        int waitingWriters;

        pthread_cond_t canRead;
        pthread_cond_t canWrite;

        pthread_mutex_t conditionalLock;

    public:
        monitor() {
            readerCount = 0;
            writerCount = 0;
            waitingReaders = 0;
            waitingWriters = 0;

            pthread_cond_init(&canRead, NULL);
            pthread_cond_init(&canWrite, NULL);
            pthread_mutex_init(&conditionalLock, NULL);
        }

        void startRead(int i) {
            pthread_mutex_lock(&conditionalLock);

            if (writerCount == 1 || waitingWriters > 0) {
                waitingReaders++;

                pthread_cond_wait(&canRead, &conditionalLock);
                waitingReaders--;
            }

            readerCount++;

            cout << "Reader " << i << " is reading\n";
            pthread_mutex_unlock(&conditionalLock);
            pthread_cond_broadcast(&canRead);
        }

        void stopRead(int i) {
            pthread_mutex_lock(&conditionalLock);

            if (--readerCount == 0) {
                pthread_cond_signal(&canWrite);
            }

            pthread_mutex_unlock(&conditionalLock);
        }

        void startWrite(int i) {
            pthread_mutex_lock(&conditionalLock);

            if (writerCount == 1 || readerCount > 0) {
                ++waitingWriters;
                pthread_cond_wait(&canWrite, &conditionalLock);
                --waitingReaders;
            }

            writerCount = 1;

            cout << "Writer " << i << " is writing \n";
        }

        void stopWrite(int i) {
            pthread_mutex_lock(&conditionalLock);

            writerCount = 0;

            if (waitingReaders > 0) {
                pthread_cond_signal(&canRead);
            } else {
                pthread_cond_signal(&canWrite);
            }
        }
} Monitor;


void * reader(void * id) {
    int c = 0;

    int i = *(int*) id;

    while (c < 5) {
        // find sleep function and call it here
        Monitor.startRead(i);
        Monitor.stopRead(i);
        c++;
    }
}

void * writer(void * id) {
    int c = 0;
    
    int i = *(int*) id;

    while (c < 5) {
        // find sleep function and call it here
        Monitor.startWrite(i);
        Monitor.stopWrite(i);
        c++;
    }
}

int main() {

    pthread_t readerArray[5];
    pthread_t writerArray[5];

    int threadID[5];

    for (int i = 0; i < 5; i++) {
        threadID[i] = i;

        pthread_create(&readerArray[i], NULL, &reader, &threadID[i]);

        pthread_create(&writerArray[i], NULL, &writer, &threadID[i]);
    }

    for (int i = 0; i < 5; i++) {
        pthread_join(readerArray[i], NULL);
    }

    for (int i = 0; i < 5; i++) {
        pthread_join(writerArray[i], NULL);
    }

    return 0;
}