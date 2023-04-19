#include <fstream>
#include <cstdint>
#include <iostream>

using namespace std;

int main() {
    ifstream inputAddressFile("addresses.txt");

    uint32_t addr;
    while (inputAddressFile >> addr) {
        addr = addr & 0x0000ffff;
        cout << addr << endl;
    }

    return 0;
}
