#include "../inc/RelocationSymbol.hpp"


RelocationSymbol::RelocationSymbol(std::string sectionName, std::string type, int id, int offset)
{
    this->sectionName = sectionName;
    this->type = type;
    this->offset = offset;
    this->id = id;
}

 std::ostream &operator<<(std::ostream &os, const RelocationSymbol& sym)
{


    //print OFFSET
    os << (std::string)"|";
    if (sym.offset < 10)
    {
        os << (std::string) "000" + std::to_string(sym.offset) + ":       ";
    }
    else if (sym.offset < 100)
    {
         os << (std::string) "00" + std::to_string(sym.offset) + ":       ";
    }
    else if (sym.offset < 1000)
    {
         os << (std::string) "0" + std::to_string(sym.offset) + ":       ";
    }
    else if (sym.offset < 10000)
    {
         os << std::to_string(sym.offset) + ":      ";
    }
    os << (std::string) "|";

    //print TYPE
    os << sym.type;
    for (int i = 10 - sym.type.length(); i > 0; i--)
        os << (std::string) " ";
    os << (std::string) "|";

    //print ID of symbol used in Symbol Table
    os << std::to_string(sym.id);
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


    // //print SECTION_NAME
    // os << sym.sectionName;
    // for (int i = 13 - sym.sectionName.length(); i > 0; i--)
    //     os << (std::string) " ";
    // os << (std::string) "|";
    return os;
}