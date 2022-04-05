#ifndef SYBMOL_H
#define SYNBOL_H

#include <string>
#include <iostream>
class Symbol {
public:

Symbol (std::string name, std::string sectionName, std::string type, int offset, int value, bool symbol);


std::string getName();
std::string getSectionName();
std::string getType();
int getOffset();
int getSymbolIndex();
int getValue();


void setName(std::string name);
void setSectionName(std::string section_name);
void setOffset(int offset);
void setSymbolIndex(int index);
void setValue(int value);
void setType(std::string type);

friend std::ostream& operator<< (std::ostream& os, const Symbol& sym);

private:

std::string name; 
std::string sectionName;
std::string type; //Local or global
bool symbol; //if true - symbol, false - section
int value; 
int offset;
int id; 
static int ID;

};

#endif