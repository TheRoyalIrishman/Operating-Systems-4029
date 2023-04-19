#include <fstream>
#include <cstdint>
#include <iostream>
#include <map>

using namespace std;

int main() {
    ifstream inputAddressFile("addresses.txt");
    map<uint8_t, int8_t[]> pageTable;

    uint32_t addr;
    while (inputAddressFile >> addr) {
        uint16_t maskedAddr = addr & 0x0000ffff;

        uint8_t pageNumber = maskedAddr >> 8;
        uint8_t pageOffset = maskedAddr & 0x000000ff;

        auto pageLocation = pageTable.find(pageNumber);
        if (pageLocation == pageTable.end()) {
            // TODO: Load in page from BACKING_STORE.bin file
        } else {
            // already loaded in
            auto page = pageLocation->second;
            int8_t value = page[pageOffset];

            cout << addr << ' ' << maskedAddr << ' ' << (int)value << endl;
        }
    }

    return 0;
}
