#ifndef RELOCATIONSYMBOL_H
#define RELOCATIONSYMBOL_H

#include <string>

class RelocationSymbol
{
public:
    RelocationSymbol(std::string sectionName, std::string type, int id, int offset);
    friend std::ostream& operator<< (std::ostream& os, const RelocationSymbol& rs);

private:
    int id;
    int offset;
    std::string sectionName;
    std::string type; //Type of address mode ABS or PC_REL
    

};

#endif