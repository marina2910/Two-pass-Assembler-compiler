#include "../inc/Section.hpp"

Section::Section(std::string name, int size = 0)
{
    this->name = name;
    this->size = size;
    this->relocationTable = std::list<RelocationSymbol *>();
}

std::string Section::getName()
{
    return this->name;
}

int Section::getSize()
{
    return this->size;
}

void Section::setName(std::string name)
{
    this->name = name;
}

void Section::setSize(int size)
{
    this->size = size;
}

void Section::addData(int total, long bytes)
{
    if (total == 1)
    {
        data.push_back(bytes & 255);
    }
    else if (total == 2)
    {

        data.push_back((bytes >> 8) & 255);
        data.push_back(bytes & 255);
    }
    else if (total == 3)
    {
        data.push_back((bytes >> 16) & 255);
        data.push_back((bytes >> 8) & 255);
        data.push_back(bytes & 255);
    }
    else if (total == 4)
    {
        data.push_back((bytes >> 24) & 255);
        data.push_back((bytes >> 16) & 255);
        data.push_back((bytes >> 8) & 255);
        data.push_back(bytes & 255);
    }
    else if (total == 5)
    {
        data.push_back((bytes >> 32) & 255);
        data.push_back((bytes >> 24) & 255);
        data.push_back((bytes >> 16) & 255);
        data.push_back((bytes >> 8) & 255);
        data.push_back(bytes & 255);
    }
}

std::list<RelocationSymbol *> Section::getRelocTable()
{
    return this->relocationTable;
}

void Section::addToRelocTable(RelocationSymbol *rs)
{
    this->relocationTable.push_back(rs);
}

void Section::printRelocTable()
{

    std::cout << "=========RELOCATION TABLE========\n";
    std::cout << "|   OFFSET   |   TYPE   |  ID   |\n";
    std::cout << "=================================\n";

    for (auto it = relocationTable.begin(); it != relocationTable.end(); it++)
    {
        RelocationSymbol *symb = *it;
        std::cout << *symb << "\n";
    };
    std::cout << "=================================\n";
}

std::ostream &operator<<(std::ostream &os, const Section *sec)
{
    os << (std::string) "# " << sec->name << " " <<  sec->size << " ";
    int i = 0;
    for (auto it = sec->data.begin(); it != sec->data.end(); ++it)
    {
        if (i % 8 == 0)
        {
            os << '\n';
            if (i < 10)
                os << "000" << i << ":";
            else if (i < 100)
                os << "00" << i << ":";
            else if (i < 1000)
                os << "0" << i << ":";
            else
                os << i << ":";
        }
        if (*it < 16)
            os << "0";
        os << std::hex << *it << " ";
        i++;
    }
    return os;
}
