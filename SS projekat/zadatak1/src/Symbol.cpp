#include "../inc/Symbol.hpp"

int Symbol::ID = 0;

Symbol::Symbol(std::string name, std::string sectionName, std::string type, int offset, int value, bool symbol)
{
    this->symbol = symbol;
    this->name = name;
    this->sectionName = sectionName;
    this->offset = offset;
    this->type = type;
    this->value = value;
    this->id = ++ID;
};

std::string Symbol::getName()
{
    return this->name;
}
std::string Symbol::getSectionName()
{
    return this->sectionName;
}
std::string Symbol::getType()
{
    return this->type;
}
int Symbol::getOffset()
{
    return this->offset;
};

int Symbol::getSymbolIndex()
{
    return this->id;
}

int Symbol::getValue()
{
    return this->value;
}

void Symbol::setName(std::string name)
{
    this->name = name;
}
void Symbol::setSectionName(std::string sectionName)
{
    this->sectionName = sectionName;
}
void Symbol::setOffset(int offset)
{
    this->offset = offset;
}
void Symbol::setSymbolIndex(int index)
{
    this->id = index;
}
void Symbol::setValue(int value)
{
    this->value = value;
}
void Symbol::setType(std::string type)
{
    this->type = type;
}

std::ostream &operator<<(std::ostream &os, const Symbol &sym)
{

    //print ID
    os << "|"<<std::to_string(sym.id);
    if (sym.id < 10)
    {
        os << (std::string) "      ";
    }
    else if (sym.id < 100)
    {
        os << (std::string) "     ";
    }
    else if (sym.id < 1000)
    {
        os << (std::string) "    ";
    }
    os << (std::string) "|";
    
    //print NAME
    os << sym.name;
    for (int i = 14 - sym.name.length(); i > 0; i--)
        os << (std::string) " ";
    os << (std::string) "|";

    //print SECT/SYMB
    if(sym.symbol == true) {
        os<<"symbol     |";
    } else {
        os << "section    |";
    }

    //print SECTION_NAME
    os << sym.sectionName;
    for (int i = 13 - sym.sectionName.length(); i > 0; i--)
        os << (std::string) " ";
    os << (std::string) "|";

    //print OFFSET
    os << std::to_string(sym.offset);
    if (sym.offset < 0x10)
    {
        os << (std::string) "           ";
    }
    else if (sym.offset < 0x100)
    {
        os << (std::string) "          ";
    }
    else if (sym.offset < 0x1000)
    {
        os << (std::string) "         ";
    }
    else if (sym.offset < 0x10000)
    {
        os << (std::string) "         ";
    }
    os << (std::string) "|";

    //print TYPE
    os << sym.type;
    for (int i = 10 - sym.type.length(); i > 0; i--)
        os << (std::string) " ";
    os << (std::string) "|";

    //print VALUE
    os << std::hex <<sym.value;
    if (sym.value < 0x10)
    {
        os << (std::string) "         ";
    }
    else if (sym.value < 0x100)
    {
        os << (std::string) "        ";
    }
    else if (sym.value < 0x1000)
    {
        os << (std::string) "       ";
    }
    else if (sym.value < 0x10000)
    {
        os << (std::string) "      ";
    }
    os << (std::string) "|";

    return os;
}