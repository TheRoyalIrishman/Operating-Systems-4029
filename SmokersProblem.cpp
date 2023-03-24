#include <iostream>
#include <thread>
#include <mutex>

using namespace std;


// declare the mutex and flags for ingredients
mutex mtx;
bool paper = false;
bool tobacco = false;
bool matches = false;

// function for agent process
void agentFunction() {
    while (true) {
        mtx.lock();
        // put out two random ingredients
        int randNum = rand() % 3;
        if (randNum == 0) {
            paper = true;
            tobacco = true;
        } else if (randNum == 1) {
            paper = true;
            matches = true;
        } else {
            tobacco = true;
            matches = true;
        }
        cout << "Agent: puts out ";
        if (paper) {
            cout << "paper, ";
        }
        if (tobacco) {
            cout << "tobacco, ";
        }
        if (matches) {
            cout << "matches, ";
        }
        cout << endl;
        mtx.unlock();
        // wait for smoker to finish smoking
        this_thread::sleep_for(chrono::milliseconds(1000));
    }
}

// function for smoker with paper
void paperFunction() {
    while (true) {
        mtx.lock();
        if (tobacco && matches) {
            cout << "Smoker with paper: rolls and smokes a cigarette." << endl;
            tobacco = false;
            matches = false;
            // signal agent
            mtx.unlock();
            break;
        }
        mtx.unlock();
    }
}

// function for smoker with tobacco
void tobaccoFunction() {
    while (true) {
        mtx.lock();
        if (paper && matches) {
            cout << "Smoker with tobacco: rolls and smokes a cigarette." << endl;
            paper = false;
            matches = false;
            // signal agent
            mtx.unlock();
            break;
        }
        mtx.unlock();
    }
}

// function for smoker with matches
void matchesFunction() {
    while (true) {
        mtx.lock();
        if (paper && tobacco) {
            cout << "Smoker with matches: rolls and smokes a cigarette." << endl;
            paper = false;
            tobacco = false;
            // signal agent
            mtx.unlock();
            break;
        }
        mtx.unlock();
    }
}

int main() {
    // create threads for agent and smokers
    thread agentThread(agentFunction);
    thread paperThread(paperFunction);
    thread tobaccoThread(tobaccoFunction);
    thread matchesThread(matchesFunction);

    // join threads
    agentThread.join();
    paperThread.join();
    tobaccoThread.join();
    matchesThread.join();

    return 0;
}