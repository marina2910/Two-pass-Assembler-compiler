#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <fstream>
#include <regex>
#include <list>


class Parser
{

public:
    Parser();
    ~Parser();

    std::string** parseFile();
    int countFileLines();

    bool isLineComment(std::string line);
    bool resolveInstructionNoParams(std::string line, std::string inst);
    std::string getEquSymbolName(std::string label);
    std::string getLiterals(std::string literal);
    std::string getSymbol(std::string inst, std::string line);
    std::string isLineLabel(std::string line);
    std::string isLineDirective(std::string line);
    std::string isLineInstruction(std::string line);
    std::string getSectionName(std::string line);
    std::string trimRightSideSpace(std::string line);
    std::list<std::string> getParameterList(std::string line);
    std::list<std::string> getRegiserList(std::string inst, std::string line);
    std::list<std::string> getBracketParameterList(std::string line);
private:

    std::string getParameters(std::string line);
};

#endif //PARSER_H