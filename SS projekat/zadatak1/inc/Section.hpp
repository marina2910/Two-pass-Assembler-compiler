#ifndef SECTION_HPP
#define SECTION_HPP
#include "RelocationSymbol.hpp"

#include<string>
#include <list>
#include <iostream>
#include <iterator>
class Section {
 public:
    Section(std::string name, int size);
    std::string getName();
    int getSize();

    void setName(std::string name);
    void setSize(int size);
    void addData(int total, long bytes); //total - number of bytes to be added; bytes - info to be added
    void addToRelocTable(RelocationSymbol* rs);
    void printRelocTable();
    std::list<RelocationSymbol*> getRelocTable();
   
    friend std::ostream& operator<<(std::ostream& os, const Section* sec);

 private: 
    std::string name;
    int size;
    std::list<int> data;
    std::list<RelocationSymbol*> relocationTable;
};

#endif