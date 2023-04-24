#include <fstream>
#include <cstdint>
#include <iostream>
#include <map>
#include <array>

using namespace std;

map<uint8_t, array<int8_t, 256>> pageTable;
map<uint8_t, array<int8_t, 256>> TLB;

void readPageFromFile(uint8_t position);

int main() {
    ifstream inputAddressFile("addresses.txt");

    // virtual address
    double pageFaults = 0;
    double TLBHits = 0;
    double addrTotal = 0;
    uint32_t addr;
    while (inputAddressFile >> addr) {
        // physical address
        addrTotal++;
        uint16_t maskedAddr = addr & 0x0000ffff;

        uint8_t pageNumber = maskedAddr >> 8;
        uint8_t pageOffset = maskedAddr & 0x000000ff;

        if (TLB.find(pageNumber) != TLB.end()){
            TLBHits++;
            auto page = TLB.find(pageNumber)->second;
            int8_t value = page[pageOffset];
            cout << addr << ' ' << maskedAddr << ' ' << (int)value << endl;
        }
        else {
            auto pageLocation = pageTable.find(pageNumber);
            if (pageLocation == pageTable.end()) {
                pageFaults++;
                readPageFromFile(pageNumber);

                int8_t value = pageTable.at(pageNumber)[pageOffset];
                cout << addr << ' ' << maskedAddr << ' ' << (int)value << endl;
            } else {
                // already loaded in
                auto page = pageLocation->second;
                int8_t value = page[pageOffset];
                cout << addr << ' ' << maskedAddr << ' ' << (int)value << endl;
                if (TLB.size() == 16) {
                    TLB.erase(TLB.begin()->first);
                    TLB.insert({pageNumber, page});
                }
                else {
                    TLB.insert({pageNumber, page});
                }
            }
        }
    }
    cout << 'TLB hit rate: ' << TLBHits/addrTotal << endl;
    cout << 'Page fault rate: ' << pageFaults/addrTotal << endl;
    return 0;
}

void readPageFromFile(uint8_t position) {
    fstream fileStream;

    // opens binary file and should be input and in binary format
    fileStream.open("BACKING_STORE.bin", ios::in | ios::binary);

    // gets position of item within text file and multiplies by 256 (page size)
    fileStream.seekg(position * 256);
    
    array<int8_t, 256> page;

    // read the whole page in
    fileStream.read((char *)&page, sizeof(page));

    fileStream.close();

    // set the page in the page table
    pageTable.emplace(position, page);
}