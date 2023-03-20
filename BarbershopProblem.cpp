#include <semaphore.h>
#include <thread>
#include <chrono>
#include <queue>
#include <iostream>
#include <mutex>

using namespace std;

int numCustomers = 0;

sem_t barber_waiting_room;

sem_t barber_chair;